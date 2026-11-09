#include <string.h>

#include "mod_plugins.h"

/*
 * Courier Crisis draw distance.
 *
 * The game keeps its draw distance in gp+0x70 (0x800A0D80). Every frame
 * 0x80029480 moves it by +-50 (down by 100 while the frame counter is low) and
 * clamps it to 2000..4000; the renderer reset (0x80021910, 0x800294C0) sets it
 * to 4000. The world traversal at 0x80026A70 and the face loop use it, so a larger
 * value draws streets, buildings and cars farther out.
 *
 * game.toml lists the six clamp and reset immediates as [mod_immediate] group 2.
 * Selector N (1..5) turns 2000..4000 into 1.25x, 1.5x, 2.0x, 2.5x or 3.0x of that
 * range; selector 0 is the stock game. The choice is made once at start-up.
 */
#define CC_PLUGIN_ID  "courier.draw-distance"
#define CC_DIST_GROUP 2u

static void cc_activate(void) {
    static const char *const steps[] = { "1.25", "1.5", "2.0", "2.5", "3.0" };
    char v[16];
    if (!psx_mod_current_option_value("distance", v, sizeof v)) return;
    for (uint32_t i = 0; i < sizeof steps / sizeof steps[0]; i++)
        if (strcmp(v, steps[i]) == 0) {
            (void)psx_mod_set_immediate_selector(CC_DIST_GROUP, i + 1u);
            return;
        }
}

PSX_MOD_CONSTRUCTOR(cc_register_draw_distance_plugin) {
    (void)psx_mod_register_activation_plugin(CC_PLUGIN_ID, cc_activate);
}
