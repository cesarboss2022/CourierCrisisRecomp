#include "cpu_state.h"
#include "func_override.h"
#include "mod_plugins.h"

/*
 * Courier Crisis widescreen edge fix.
 *
 * Before an object is drawn, 0x80078FEC asks "is its screen rectangle hidden
 * behind nearer geometry?". The rectangle (a0+0x22 left, a0+0x28 right, as
 * halfwords) is clamped to the 320 columns of the original screen and compared
 * with a list of covered column spans that only exists for those 320 columns.
 * In 16:9 an object near an edge projects outside 0..320: its rectangle is
 * clamped onto the last column and is tested against whatever covers that one
 * column, so it is dropped although it is in plain view, and it pops in once
 * its rectangle gets inside the original screen.
 *
 * The test has no side effects, so an object whose rectangle leaves 0..320 is
 * simply reported as visible (v0 = 1, the original's "not hidden" result) and
 * every other object runs the original test untouched.
 */
#define CC_OCCLUSION_FN 0x80078FECu
#define CC_RECT_LEFT    0x22u
#define CC_RECT_RIGHT   0x28u
#define CC_SCREEN_W     320

static int cc_occlusion_override(CPUState *cpu) {
    const uint32_t rect = cpu->gpr[4];
    const int left  = (int16_t)psx_mod_read_half(rect + CC_RECT_LEFT);
    const int right = (int16_t)psx_mod_read_half(rect + CC_RECT_RIGHT);
    if (left >= 0 && right <= CC_SCREEN_W) return 0;   /* inside the original screen */
    cpu->gpr[2] = 1u;                                  /* visible */
    return 1;
}

PSX_MOD_CONSTRUCTOR(cc_register_wide_edges_plugin) {
    static const uint32_t guard[4] = {
        0x8F820B58u,   /* lw  v0,0xB58(gp)  */
        0x84880024u,   /* lh  t0,0x24(a0)   */
        0x8C460010u,   /* lw  a2,0x10(v0)   */
        0x05010002u,   /* bgez t0,+2        */
    };
    (void)psx_mod_register_function_override("courier.wide-edges:occlusion",
                                             CC_OCCLUSION_FN, cc_occlusion_override,
                                             guard, 4, FO_CREDIT_SELF);
}
