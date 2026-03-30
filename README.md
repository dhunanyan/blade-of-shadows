# pp2-lab14-game

Compact 2D shooter project in C++ with:
- core gameplay logic (`game_core` library),
- Qt GUI app (`game` target),
- unit tests (`tests` target with GoogleTest).

## Quick Start

Requirements:
- CMake >= 3.19
- C++ compiler with C++23 support
- Qt5/Qt6 (Widgets + Multimedia)
- `git` (used by scripts to fetch test dependencies)

Build GUI app:

```bash
./scripts/build.sh
```

Run GUI app (without rebuilding):

```bash
./scripts/start.sh
```

Run tests:

```bash
./scripts/test.sh
```

Use custom build directory (optional):

```bash
./scripts/build.sh my-build
./scripts/start.sh my-build
./scripts/test.sh my-build
```

## Project Layout

```text
.
├── assets/              # Images, audio, Qt resources
├── include/game/core/   # Public headers for core gameplay
├── src/core/            # Core gameplay implementation
├── src/app/cli/         # CLI entrypoint
├── src/app/qt/          # Qt GUI app
├── tests/               # Unit tests + CMake for tests target
└── scripts/             # Build/start/test helper scripts
```

## Notes

- `tests/lib` is fetched at runtime from `google/googletest` by scripts and is ignored by git.
- `scripts/build.sh` builds only.
- `scripts/start.sh` runs only.
- `scripts/test.sh` builds tests only when needed, then runs them.

## CLI Runner

The CLI target (`game_cli`) is a lightweight headless simulation runner useful for:
- quick gameplay smoke checks without GUI,
- debugging movement/spawn behavior in CI/terminal,
- future balancing experiments (difficulty, enemy count, bullet tuning).

Build and run manually:

```bash
cmake -S . -B build
cmake --build build --target game_cli
./build/bin/game_cli --help
./build/bin/game_cli 60 38 25
```

## Documentation

- Architecture overview: [`docs/architecture.md`](docs/architecture.md)
- Development workflow: [`docs/development.md`](docs/development.md)
