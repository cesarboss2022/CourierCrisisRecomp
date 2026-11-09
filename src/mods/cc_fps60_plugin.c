#include <string.h>

#include "mod_plugins.h"

/*
 * Courier Crisis "60 FPS".
 *
 * The game flips its display buffer only when more than gp+0x28C VBlanks have
 * passed since the last flip (VBlank handler at 0x80075CEC). The game sets that
 * minimum interval to 1, which is its 30 fps cap. With 0 it flips on every VBlank.
 *
 * Findings behind this plugin (see the project notes):
 *  - the game's logic is time based, so a higher frame rate does not speed it up;
 *  - the limiter value is plain game RAM, no code has to change;
 *  - the host can sustain 60 fps with the wait-skip override and native CPU spans.
 *
 * The plugin only touches the value while a level is running (0x800A12D4 == 1)
 * and only when it holds the game's own default of 1, so any other value the
 * game chooses for itself is left alone. Switching the option off writes the
 * default back.
 */
#define CC_MIN_FLIP_ADDR  0x800A0F9Cu   /* gp+0x28C: minimum VBlanks between flips */
#define CC_IN_LEVEL_ADDR  0x800A12D4u   /* 1 from level load until the level is left */
#define CC_PLUGIN_ID      "courier.fps60"

static volatile int s_unlock = 1;

static void cc_read_options(void) {
    char v[8];
    s_unlock = 1;
    if (psx_mod_current_option_value("unlock", v, sizeof v) && strcmp(v, "false") == 0)
        s_unlock = 0;
}

static void cc_activate(void) {
    cc_read_options();
}

static int cc_option_changed(const char *option_id, const char *value) {
    (void)option_id;
    (void)value;
    cc_read_options();     /* answers the new value in this plugin's context */
    return 1;              /* applied at the next VBlank */
}

static void cc_vblank(void) {
    if (!psx_mod_game_started()) return;
    if (psx_mod_read_word(CC_IN_LEVEL_ADDR) != 1u) return;
    const uint32_t cur = psx_mod_read_word(CC_MIN_FLIP_ADDR);
    if (s_unlock) {
        if (cur == 1u) psx_mod_write_word(CC_MIN_FLIP_ADDR, 0u);   /* 30 fps cap -> every VBlank */
    } else if (cur == 0u) {
        psx_mod_write_word(CC_MIN_FLIP_ADDR, 1u);                  /* restore the game's default */
    }
}

PSX_MOD_CONSTRUCTOR(cc_register_fps60_plugin) {
    (void)psx_mod_register_activation_plugin(CC_PLUGIN_ID, cc_activate);
    (void)psx_mod_register_option_changed_plugin(CC_PLUGIN_ID, cc_option_changed);
    (void)psx_mod_register_vblank_plugin(CC_PLUGIN_ID, cc_vblank);
}
