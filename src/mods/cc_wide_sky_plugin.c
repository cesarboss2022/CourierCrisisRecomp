#include "mod_plugins.h"

/*
 * Courier Crisis widescreen sky.
 *
 * Each frame the game draws its sky as screen-space primitives at the back of the
 * ordering table: 3 rows x 5 columns of 64-pixel GT4 tiles (the pointers are in
 * three tables of five, one set per display buffer, at 0x800AD6F8, 0x800C1CE0 and
 * 0x800EA970, 20 bytes per buffer) and a G4 gradient for the ground band (one
 * pointer per buffer at 0x800A1744). Their X coordinates are written once, to
 * exactly 0..320, so a 16:9 picture shows an empty strip on both sides.
 *
 * Every VBlank, in a level, the X of each tile and of the gradient is set to the
 * same 0..320 layout stretched to -m..320+m, where m is the per-side reveal of the
 * widescreen view (0 at 4:3, so nothing changes there). The values are rewritten from
 * the column number, never read back, so they stay correct when the margin changes.
 */
#define CC_PLUGIN_ID     "courier.wide-sky"
#define CC_IN_LEVEL_ADDR 0x800A12D4u
#define CC_BUFFER_COUNT  2u
#define CC_COLUMNS       5u
#define CC_TILE_W        64
#define CC_SCREEN_W      320

static const uint32_t k_tile_tables[3] = { 0x800AD6F8u, 0x800C1CE0u, 0x800EA970u };
#define CC_TABLE_STRIDE  20u
#define CC_GRADIENT_PTRS 0x800A1744u

static int32_t cc_map(int32_t x, int32_t m) {
    return -m + (x * (CC_SCREEN_W + 2 * m)) / CC_SCREEN_W;
}

static int cc_is_ram_ptr(uint32_t p) {
    return p >= 0x80010000u && p < 0x80200000u;
}

static void cc_put(uint32_t packet, uint32_t offset, int32_t x) {
    psx_mod_write_half(packet + offset, (uint16_t)(int16_t)x);
}

static void cc_vblank(void) {
    if (!psx_mod_game_started()) return;
    if (psx_mod_read_word(CC_IN_LEVEL_ADDR) != 1u) return;
    const int32_t m = psx_mod_widescreen_view_x_margin();
    for (uint32_t buf = 0; buf < CC_BUFFER_COUNT; buf++) {
        for (uint32_t t = 0; t < 3u; t++) {
            for (uint32_t col = 0; col < CC_COLUMNS; col++) {
                const uint32_t p = psx_mod_read_word(k_tile_tables[t] + buf * CC_TABLE_STRIDE + col * 4u);
                if (!cc_is_ram_ptr(p) || (psx_mod_read_byte(p + 7u) & 0xFCu) != 0x3Cu) continue;   /* GT4, also its raw-texture forms */
                const int32_t left  = cc_map((int32_t)col * CC_TILE_W, m);
                const int32_t right = cc_map(((int32_t)col + 1) * CC_TILE_W, m);
                cc_put(p, 0x08u, left);  cc_put(p, 0x14u, right);
                cc_put(p, 0x20u, left);  cc_put(p, 0x2Cu, right);
            }
        }
        const uint32_t g = psx_mod_read_word(CC_GRADIENT_PTRS + buf * 4u);
        if (cc_is_ram_ptr(g) && (psx_mod_read_byte(g + 7u) & 0xFCu) == 0x38u) {  /* G4 */
            cc_put(g, 0x08u, -m);               cc_put(g, 0x10u, CC_SCREEN_W + m);
            cc_put(g, 0x18u, -m);               cc_put(g, 0x20u, CC_SCREEN_W + m);
        }
    }
}

PSX_MOD_CONSTRUCTOR(cc_register_wide_sky_plugin) {
    (void)psx_mod_register_vblank_plugin(CC_PLUGIN_ID, cc_vblank);
}
