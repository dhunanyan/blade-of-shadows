# Development

## Standard Workflow

```bash
./scripts/build.sh
./scripts/test.sh
./scripts/start.sh
```

Use an isolated build directory when testing build-system changes:

```bash
./scripts/build.sh build-local
./scripts/test.sh build-local-tests
```

## Quality Checks

- Keep Qt types out of `include/game/core` and `src/core`.
- Add focused tests for gameplay rules and regressions.
- Build both `game` and `tests` before committing.
- Preserve `assets/levels/sample_level.json` when it contains local editor experiments.
- Do not commit `build-*`, `dist`, or runtime-fetched `tests/lib`.

## Placeholder Assets

The generated placeholder sound effects can be recreated with:

```bash
.venv/bin/python scripts/generate_placeholder_sfx.py
```

Final enemy, heart, and coin files can replace the current PNGs while keeping the aliases in `assets/qt/resources.qrc`.

## Campaign Generation

The 20 campaign JSON files and the editor's terrain catalog are deterministic
generated assets:

```bash
node scripts/generate_campaign_levels.mjs
```

The generator reads every classified tile from
`assets/images/tiles/groups`, uses the complete terrain catalog in every level,
adds every decoration type, and rejects entity/terrain overlap or unsupported
spawn and exit cells before writing files.

Terrain entries generated for a level are always solid. Non-solid visual detail
is represented only through the separate `decorations` array.

## Packaging

```bash
./scripts/package.sh build-release dist
```

On macOS, the script uses `macdeployqt` when available and creates a DMG. On other platforms it installs into a staging directory and creates a compressed archive.

Local macOS packages receive an ad-hoc signature automatically. To create a
Developer ID-signed bundle before notarization, provide the signing identity:

```bash
CODESIGN_IDENTITY="Developer ID Application: Your Name (TEAMID)" \
  ./scripts/package.sh build-release dist
```
