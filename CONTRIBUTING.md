# Contributing

Keep contributions source-only. Do not commit disc images, extracted game code
or assets, generated code (`generated/`), captured overlays, proprietary BIOS files,
binaries, saves, captures, logs, credentials, or machine-specific paths.

Edit project source, configuration, seeds or the framework patches, then rebuild
locally with `.\build.ps1`. Save a framework change by regenerating its patch (see
[docs/BUILD.md](docs/BUILD.md#the-framework-patches)). Do not hand-edit
`generated/*.c` or include unrelated framework changes.

## Changes

- Open a pull request against `main`. It merges by squash or rebase once the
  `build` checks pass.
- Write commit subjects in the Conventional Commits style: `feat:`, `fix:`,
  `docs:`, `build:`, `ci:`, `test:`, `refactor:` or `chore:`.
- Run the relevant checks and review both the working diff and staged paths.
- Keep experimental visual features optional.
- Show a change the player can see with a before and after pair: two pictures
  of the same moment, or a short video when it only shows in motion. Put the pair
  in the pull request and in the release notes.
- Update [STATUS.md](docs/STATUS.md) when behavior, verified coverage, or a known
  limitation changes. Keep it focused on the current build. Update the README
  and [PLAYING.md](docs/PLAYING.md) when a setting or its default changes.

## Versions

A release is tagged `vMAJOR.MINOR.PATCH` in the [Semantic
Versioning](https://semver.org) form, with no label: a major version of 0
already says that anything may still change.

- `0.MINOR.0` brings new features or changed behavior.
- `0.MINOR.PATCH` brings fixes only.
- `1.0.0` when the whole campaign is verified.

A release's section in [CHANGELOG.md](CHANGELOG.md) is headed `## v0.1.0` and
so on: the packager finds it by that heading, and `VERSION` must match.

## Reporting a problem

- Say the level, the steps to reproduce, the render scale and aspect, the
  enhancements in use, and whether restarting fixes the problem.
- Attach `psx_last_run_report.json` from the game folder when there is one.
- Never attach game data or proprietary BIOS files.
- Report a security issue as [SECURITY.md](SECURITY.md) says, not in a public
  issue.

## License

By contributing you agree that your contribution is licensed under the
[MIT License](LICENSE).

See [docs/BUILD.md](docs/BUILD.md) for setup and release commands, and
[licenses/README.md](licenses/README.md) for dependency licenses.