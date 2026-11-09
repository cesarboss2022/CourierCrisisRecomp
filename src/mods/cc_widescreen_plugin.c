#include <string.h>

#include "mod_plugins.h"

/*
 * Courier Crisis widescreen (first pass).
 *
 * The runtime clamps any aspect that arrives from game.toml/settings.toml to
 * 4:3 ("widescreen is mod-owned on PSX"), so the view is selected here, from a
 * trusted activation callback, exactly like the other psxrecomp titles.
 *
 * Only the display aspect is requested. Culling comes from the generated code
 * ([widescreen.cull] auto_screen_x in game.toml); HUD tagging and background
 * work are added later, only if playtesting shows they are needed.
 */
#define CC_FEATURE_ASPECT "aspect"

/* Fit to Window follows live resizing up to this ratio. */
#define CC_FIT_MAX_NUM 21u
#define CC_FIT_MAX_DEN 9u

static void cc_widescreen_activate(void) {
    char aspect[16];

    if (!psx_mod_current_option_value(CC_FEATURE_ASPECT, aspect, sizeof aspect))
        strcpy(aspect, "16:9");

    if (strcmp(aspect, "21:9") == 0) {
        (void)psx_mod_set_fixed_display_aspect(21u, 9u);
    } else if (strcmp(aspect, "Fit") == 0) {
        /* Start from 16:9, then follow the window. */
        (void)psx_mod_set_fixed_display_aspect(16u, 9u);
        (void)psx_mod_set_adaptive_display_aspect(CC_FIT_MAX_NUM, CC_FIT_MAX_DEN);
    } else {
        (void)psx_mod_set_fixed_display_aspect(16u, 9u);
    }
}

PSX_MOD_CONSTRUCTOR(cc_register_widescreen_plugin) {
    (void)psx_mod_register_activation_plugin(
        "courier.widescreen", cc_widescreen_activate);
}
