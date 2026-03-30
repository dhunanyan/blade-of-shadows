# Architecture Overview

## Targets

- `game_core` (library)
  - Pure gameplay/domain logic.
  - Contains engine, stage, objects, enemies, bullets, player, movement rules.
- `game` (Qt executable)
  - GUI and rendering.
  - Uses `game_core`.
- `tests` (GoogleTest executable)
  - Unit tests for core logic.
  - Links `game_core`.

## Layering

1. `src/core` and `include/game/core` do not depend on Qt.
2. `src/app/qt` depends on `game_core`.
3. Tests depend on `game_core`.

This keeps gameplay logic reusable and testable outside GUI code.

## Assets

- `assets/images/*` for textures/sprites.
- `assets/audio/*` for music/sound.
- `assets/qt/resources.qrc` maps files into Qt resource system.
