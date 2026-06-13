# Blade of Shadows

Blade of Shadows is a 2D action platformer built with C++23 and Qt 6. It combines responsive movement, melee combat, double jumping, dodging, collectibles, campaign progression, save games, localized menus, audio settings, and an integrated level editor.

## Features

- Two playable campaign levels with camera scrolling and solid-tile collision.
- Idle, run, jump, fall, dodge, damage, death, and attack animation support.
- Melee combat, enemy health, contact damage, player hearts, score, and coins.
- Variable-height jump, double jump, apex handling, ceiling collision, and dodge invulnerability.
- Main, pause, game-over, victory, campaign-complete, settings, credits, and editor menus.
- Mouse and keyboard menu navigation.
- Persistent save game and settings storage through Qt application data paths.
- English and Polish UI text.
- A 46-track shuffled soundtrack with named tracks, history-aware previous/next controls, and automatic progression.
- Music and sound toggles with independent volume controls.
- Data-driven JSON levels and an in-game tile/entity editor.
- GoogleTest coverage and GitHub/GitLab CI.

## Requirements

- CMake 3.19 or newer
- A C++23 compiler
- Qt 6 with Widgets and Multimedia
- Git, used to fetch GoogleTest

## Build And Run

```bash
./scripts/build.sh
./scripts/start.sh
```

Run tests:

```bash
./scripts/test.sh
```

Create a distributable build:

```bash
./scripts/package.sh
```

All scripts accept an optional build directory.

## Controls

| Action | Keys |
| --- | --- |
| Move | Arrow Left/Right or `A`/`D` |
| Jump / double jump | Arrow Up or `W` |
| Attack | `Space` or `J` |
| Dodge | Arrow Down or `S` |
| Pause / back | `Esc` |
| Menu navigation | Arrow Up/Down or `W`/`S` |
| Menu confirm | `Enter` or `Space` |

## Level Editor

Open **Level Editor** from the main menu.

| Action | Input |
| --- | --- |
| Paint selected object | Left click |
| Remove tile and entities | Right click |
| Select tile/spawn/enemy/coin/exit tool | `1`-`5` |
| Previous/next tileset tile | `Q` / `E` |
| Toggle solid/decorative tile | `F` |
| Save custom level | `Ctrl+S` |
| Editor menu | `Esc` |

Custom levels and save games are stored under the platform-specific Qt application data directory for `Dhunanyan/Blade of Shadows`.

## Project Layout

```text
assets/                 Images, audio, levels, and Qt resources
include/game/core/      Qt-independent gameplay model
include/game/app/       Input, menus, settings, and orchestration
include/game/renderer/  Animation and rendering interfaces
src/core/               Gameplay implementation
src/app/                Controllers and Qt application integration
src/renderer/           Scene and animation rendering
tests/                  GoogleTest suites
scripts/                Build, run, package, and placeholder generators
docs/                   Architecture, development, and level documentation
```

## Documentation

- [Architecture](docs/architecture.md)
- [Audio](docs/audio.md)
- [Development workflow](docs/development.md)
- [Level format](docs/level-format.md)

## Asset Notes

The current enemy, heart, coin, and sound-effect assets are original placeholders intended to be replaced by final production art and audio without changing their Qt resource aliases.

The uncompressed music WAV files are retained locally as source masters and
ignored by Git to keep the repository manageable. Runtime AAC copies are
versioned with the game and can be regenerated with
`./scripts/prepare_music_assets.sh`; see [Audio](docs/audio.md) for the catalog
and integration details.
