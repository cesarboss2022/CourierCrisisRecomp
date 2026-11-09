#include "psx_menu_host.h"
#include "recomp_ui_psx_menu.h"

/*
 * Courier Crisis in-game menu.
 *
 * psxrecomp offers recomp-ui's in-game menu to every title, but when a title
 * registers no provider the runtime marks it "default_selection" and the provider
 * attaches only for multi-disc titles. Registering it here is the title's own
 * choice, so it attaches for this single-disc game too.
 *
 * Opens with F10 or Ctrl+L, or Select+L3 on a controller. Stage 1 is the stock
 * menu (display, graphics, system); rows for the mod options (internal
 * resolution, widescreen, smooth motion, audio) are the next stage.
 */
__attribute__((constructor))
static void cc_register_ingame_menu(void) {
    (void)psx_menu_set_provider(recomp_ui_psx_menu_provider());
}
