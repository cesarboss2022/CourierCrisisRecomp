cmake_minimum_required(VERSION 3.20)
set(PSXRECOMP_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/psxrecomp" CACHE PATH
    "Path to psxrecomp")
if(NOT EXISTS "${PSXRECOMP_ROOT}/runtime/runtime.cmake")
    message(FATAL_ERROR
        "PSXRecomp runtime is missing: ${PSXRECOMP_ROOT}/runtime/runtime.cmake\n"
        "This project expects the psxrecomp framework checkout at ${PSXRECOMP_ROOT}.\n"
        "Run: git submodule update --init --recursive\n"
        "If psxrecomp/ is not a submodule, set -DPSXRECOMP_ROOT=<path-to-psxrecomp>.")
endif()

project(CourierCrisisRecomp LANGUAGES C CXX)

# --- Opt-in product surfaces (OFF by default in the framework) -------------
# Scaffold prompts / flags fill these. Without recomp-ui, PSX_RECOMP_UI is OFF.
# The wizard (WIZARD_CMAKE_BLOCK / ENABLE_SETUP_WIZARD) is the first-run disc
# picker; releases are compiled from the committed generated/ C, so it never
# needs to generate or rebuild anything on a player's machine.
# recomp-ui submodule present (PSX_RECOMP_UI defaults ON).
# Full netplay UI -- enable after adding recomp-ui + testing:
# if(EXISTS "${PSXRECOMP_ROOT}/lib/recomp-net/CMakeLists.txt")
#     set(PSX_NETPLAY ON CACHE BOOL "..." FORCE)
# endif()
set(PSX_SETUP_WIZARD ON CACHE BOOL
    "Advertise first-run setup wizard + Generate & rebuild in recomp-ui" FORCE)
include("${PSXRECOMP_ROOT}/runtime/runtime.cmake")

# This project's own mod catalog. The FRAMEWORK stages it -- together with its
# builtin psx.* packages -- into <exe-dir>/mods/bundled, and refuses to
# configure if a catalog exists here but is not declared. Shared Internal
# Resolution is included even with a builtin allowlist, enabled at 4x on fresh
# profiles, with 1x..32x choices; do not supply a title-local copy of that id.
# Never copy the catalog by hand: five titles did, and framework 4cc04be3's rename of the staged
# directory broke all five silently (bead beads-eio.3.101).
set(PSX_PRELOADED_MODS "${CMAKE_CURRENT_SOURCE_DIR}/mods/preloaded")
if(NOT IS_DIRECTORY "${PSX_PRELOADED_MODS}")
    set(PSX_PRELOADED_MODS "NONE")
endif()

# Trusted mod plugins compiled into the game (src/mods/*.c).
file(GLOB CC_MOD_PLUGIN_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/mods/*.c")

psxrecomp_add_game_runtime(psx-runtime
    PRELOADED_MODS_DIR "${PSX_PRELOADED_MODS}"
    # ENABLE_NETPLAY_IF_PRESENT
    # NETPLAY_LOBBY_URL ws://netplay.retcomm.net:8765
    ENABLE_SETUP_WIZARD
    # PGXP hook flavor (-DPSX_PGXP=1): required for sub-pixel geometry and
    # perspective-correct textures to have any effect.
    PGXP
    WINDOW_TITLE "Courier Crisis Recompiled"
    # Without this the executable is named after the window title
    # (Courier_Crisis_Recompiled.exe); the release docs and smoke test use this name.
    EXE_NAME "CourierCrisis_Recompiled"
    DEFAULT_GAME_CONFIG_PATH "game.toml"
    MAX_PLAYERS 1
    LAUNCHER_BOXART "${CMAKE_CURRENT_SOURCE_DIR}/launcher_assets/img/boxart.tga"
    LAUNCHER_BRAND "${CMAKE_CURRENT_SOURCE_DIR}/launcher_assets/img/brand_courier.tga"
    APP_ICON "${CMAKE_CURRENT_SOURCE_DIR}/assets/courier.ico"
    # The game code is GAME.EXE, loaded from the disc by the boot EXE.
    # build.ps1 (psxrecomp_cli generate) compiles it ahead of time from
    # the player's own disc using aot/overlays.json.
    GAME_OVERLAY_STATIC_C "${CMAKE_CURRENT_SOURCE_DIR}/generated/overlays_static.c"
    # Committed game C (psxrecomp_cli.py generate; git add generated). Release
    # CI configures with -DPSXRECOMP_REQUIRE_GAME_C=ON so a checkout without
    # it fails instead of building a setup host.
    GEN_MARKER "generated/SLUS_004.42_dispatch.c"
    GEN_FULL_GLOB "generated/SLUS_004.42_full_*.c"
    CODEGEN_SETUP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/codegen_setup.c"
)

# The launcher's About page shows this.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" CC_VERSION_TEXT LIMIT_COUNT 1)
if(NOT CC_VERSION_TEXT)
    set(CC_VERSION_TEXT "dev")
endif()
target_compile_definitions(psx-runtime PRIVATE "CC_GAME_VERSION=\"${CC_VERSION_TEXT}\"")

# Compile the trusted mod plugins into the game executable.
if(CC_MOD_PLUGIN_SOURCES)
    target_sources(psx-runtime PRIVATE ${CC_MOD_PLUGIN_SOURCES})
    target_include_directories(psx-runtime PRIVATE
        "${PSXRECOMP_ROOT}/runtime/include")
endif()

# Stage runtime configs next to the exe for direct build/ launches. Only copy
# files that exist — never fail POST_BUILD when game_options.toml is absent.
foreach(_PSX_RUNTIME_CONFIG game.toml game_options.toml)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_PSX_RUNTIME_CONFIG}")
        add_custom_command(TARGET psx-runtime POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/${_PSX_RUNTIME_CONFIG}"
                "$<TARGET_FILE_DIR:psx-runtime>/${_PSX_RUNTIME_CONFIG}"
            COMMENT "Staging ${_PSX_RUNTIME_CONFIG}"
            VERBATIM)
    endif()
endforeach()
