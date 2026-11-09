#include "cpu_state.h"
#include "func_override.h"
#include "mod_plugins.h"
#include "psx_cycles.h"

/*
 * Courier Crisis "wait skip".
 *
 * The game's frame loop at 0x80012930 spins on 0x80075A80, "is the next display
 * buffer free?":
 *
 *     while (!acquire_buffer()) ;
 *
 * A buffer is released by the VBlank handler, so that loop runs for the rest of
 * the frame. With [timing] guest_cycle_scale above 1 the game finishes its work
 * sooner and spins longer, and every spin iteration is real host time: the
 * emulation thread sat at 100% and slipped under real time in heavy scenes,
 * which drained the audio queue (a music cut every ~15 s). The runtime's generic
 * idle_skip cannot see this loop because it calls a function.
 *
 * When the buffer is busy the original returns 0 and changes nothing, so this
 * override does the same and lets the guest clock run on by one slice. When the
 * buffer is free it declines and the original acquires it.
 */
#define CC_ACQUIRE_FN        0x80075A80u
#define CC_BUFFER_STATE_BASE 0x800A1248u   /* word per buffer: 0 free, 1 building, 2 ready, 3 shown */
#define CC_BUFFER_INDEX_OFF  0x0AD8u       /* gp-relative: current buffer index */
#define CC_SKIP_CYCLES       8192u         /* ~15 us of guest time per skipped poll */

static int cc_acquire_override(CPUState *cpu) {
    const uint32_t gp = cpu->gpr[28];
    const uint32_t idx = psx_mod_read_word(gp + CC_BUFFER_INDEX_OFF);
    if (idx > 1u) return 0;                       /* unexpected: let the original decide */
    const uint32_t slot = CC_BUFFER_STATE_BASE + idx * 4u;
    if (psx_mod_read_word(slot) == 0u) return 0;  /* free: the original acquires it */

    /* Busy: same result and register state as the original's early exit. */
    cpu->gpr[5] = idx;                            /* a1 */
    cpu->gpr[3] = slot;                           /* v1 */
    cpu->gpr[2] = 0u;                             /* v0 = "not acquired" */
    psx_advance_cycles(CC_SKIP_CYCLES);           /* this body owns the timing */
    return 1;
}

PSX_MOD_CONSTRUCTOR(cc_register_wait_skip_plugin) {
    static const uint32_t guard[4] = {
        0x8F850AD8u,   /* lw    a1,0xAD8(gp)   */
        0x27BDFFE8u,   /* addiu sp,sp,-24      */
        0x3C02800Au,   /* lui   v0,0x800A      */
        0x24421248u,   /* addiu v0,v0,0x1248   */
    };
    (void)psx_mod_register_function_override("courier.wait-skip:acquire",
                                             CC_ACQUIRE_FN, cc_acquire_override,
                                             guard, 4, FO_CREDIT_SELF);
}
