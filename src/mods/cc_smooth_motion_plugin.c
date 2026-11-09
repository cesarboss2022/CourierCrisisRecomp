#include <stdlib.h>
#include <string.h>

#include "mod_plugins.h"

/*
 * Courier Crisis "Smooth Motion": presentation-only temporal blending.
 *
 * The OpenGL presenter blends completed guest frames at the chosen output rate.
 * Guest VBlank, game logic, timers and audio keep their stock cadence, and no
 * motion is invented (moving objects ghost). Options are read at activation and
 * re-applied live: the option-changed callback only records the request, and the
 * next guest VBlank applies it on the emulation thread, which the
 * psx_mod_set_frame_interpolation_* API allows.
 *
 * FLIP is the default source: the game flips every second VBlank (30 Hz), and
 * the VBLANK source would blend for one VBlank and then hold for the next.
 */
#define CC_PLUGIN_ID "courier.smooth-motion"

static unsigned s_rate;    /* 0 = follow the display refresh, else 60..1000 */
static unsigned s_blend = PSX_MOD_FRAME_INTERPOLATION_LINEAR;
static unsigned s_source = PSX_MOD_FRAME_SOURCE_FLIP;
static volatile int s_dirty;

static void cc_read_options(void) {
    char v[24];

    s_rate = 0u;
    if (psx_mod_current_option_value("rate", v, sizeof v) && strcmp(v, "display") != 0) {
        int n = atoi(v);
        if (n >= 60 && n <= 1000) s_rate = (unsigned)n;
    }

    s_blend = PSX_MOD_FRAME_INTERPOLATION_LINEAR;
    if (psx_mod_current_option_value("blend", v, sizeof v) && strcmp(v, "adaptive") == 0)
        s_blend = PSX_MOD_FRAME_INTERPOLATION_MOTION_ADAPTIVE;

    s_source = PSX_MOD_FRAME_SOURCE_FLIP;
    if (psx_mod_current_option_value("source", v, sizeof v) && strcmp(v, "vblank") == 0)
        s_source = PSX_MOD_FRAME_SOURCE_VBLANK;
}

static void cc_apply(void) {
    (void)psx_mod_set_frame_interpolation_source(s_source);
    (void)psx_mod_set_frame_interpolation_blend(s_blend);
    (void)psx_mod_set_frame_interpolation(s_rate);
}

static void cc_activate(void) {
    s_dirty = 0;
    cc_read_options();
    cc_apply();
}

static int cc_option_changed(const char *option_id, const char *value) {
    (void)option_id;
    (void)value;
    cc_read_options();   /* answers the new value in this plugin's context */
    s_dirty = 1;
    return 1;            /* applied at the next VBlank */
}

static void cc_vblank(void) {
    if (!s_dirty) return;
    s_dirty = 0;
    cc_apply();
}

PSX_MOD_CONSTRUCTOR(cc_register_smooth_motion_plugin) {
    (void)psx_mod_register_activation_plugin(CC_PLUGIN_ID, cc_activate);
    (void)psx_mod_register_option_changed_plugin(CC_PLUGIN_ID, cc_option_changed);
    (void)psx_mod_register_vblank_plugin(CC_PLUGIN_ID, cc_vblank);
}
