# Architecture

## Layers

### Core

`game_core` is Qt-independent and owns gameplay state:

- `Engine`: deterministic update loop, collision, jump, dodge, combat, enemies, collectibles, exits, and snapshots.
- `Player`: health, score, coins, movement state, attack state, and damage cooldown.
- `Enemy`: health, contact damage, and movement state.
- `Stage` and `Position`: bounded world coordinates.

### Application

- `GameController`: maps input into engine intents and owns game/menu mode transitions.
- `InputState` and `InputMapper`: held keys and one-shot actions.
- `MenuSystem`: reusable stack-based menus.
- `SettingsRepository`: persistent Qt settings.
- `SaveGameRepository`: versioned, atomic JSON save files.
- `TileMapper`: JSON level loading, validation, rendering, editing, and serialization.

### Presentation

- `AssetRepository`: loads and caches Qt resources.
- `PlayerPresentation`: resolves animation phases independently from physics.
- `SceneRenderer`: camera, world rendering, HUD, menu overlays, and editor overlay.
- `MainWindow`: Qt event wiring, audio, persistence callbacks, and frame presentation.

## Runtime Flow

1. `MainWindow` receives keyboard or mouse input.
2. `GameController` updates `InputState` or menu selection.
3. The fixed 16 ms timer calls `GameController::tick()`.
4. `Engine` advances deterministic gameplay state only when gameplay/editor mode permits it.
5. Presentation controllers advance animation timelines.
6. `SceneRenderer` renders a complete frame.
7. `MainWindow` scales the frame with preserved aspect ratio and maps pointer coordinates back to source space.

## Data Ownership

- Gameplay state belongs to `Engine`.
- Level definitions belong to `TileMapper`.
- Visual assets belong to `AssetRepository`.
- Persistent settings and saves belong to their repositories.
- `MainWindow` coordinates these systems but does not implement gameplay rules.
