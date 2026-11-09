#include "mod_plugins.h"

/*
 * Courier Crisis "always high-detail textures".
 *
 * Each building is a list of face variants. While the renderer walks that list
 * (0x80021634..0x800216D4) it compares the face's depth, in scratchpad 0x28, with
 * 0x19B and uses the variant for the distance: close faces take the sharper
 * texture, farther faces keep the coarse one, so the texture visibly changes as
 * the player approaches. The three `slti v0, v0, 0x19B` below are those tests.
 * Selecting immediate group 1 turns the limit into 0x7FFF, so every face counts
 * as close. The instruction bytes are untouched (see [mod_immediate] in game.toml);
 * the choice is made once at start-up and applies to the whole session.
 */
#define CC_PLUGIN_ID "courier.hq-textures"
#define CC_LOD_GROUP 1u

static void cc_activate(void) {
    (void)psx_mod_set_immediate_selector(CC_LOD_GROUP, 1u);
}

PSX_MOD_CONSTRUCTOR(cc_register_hq_textures_plugin) {
    (void)psx_mod_register_activation_plugin(CC_PLUGIN_ID, cc_activate);
}
