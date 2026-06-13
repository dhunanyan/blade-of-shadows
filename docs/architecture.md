# Architecture

## Layers

### Core

`game_core` is Qt-independent and owns gameplay state:

- `LevelDefinition`: validated level dimensions, collision cells, spawn points, entities, and coordinate conversion.
- `PhysicsBody`: world-space position, size, velocity, and bounds.
- `PlayerPhysics`: spawn placement, movement, jump/double jump, gravity, grounding, and tile collision.
- `Engine`: deterministic update order, combat, enemies, collectibles, exits, snapshots, and gameplay events.
- `Player`: health, score, coins, body, movement state, attack state, and damage cooldown.
- `Enemy`: health, contact damage, and movement state.
- `Position`: integer grid coordinates used by level data.

### Application

- `GameController`: maps input into engine intents and owns game/menu mode transitions.
- `GameSession`: loads campaign/custom levels and coordinates new, saved, editor, restart, and next-level sessions.
- `InputState` and `InputMapper`: held keys and one-shot actions.
- `MenuSystem`: reusable stack-based menus.
- `SettingsRepository`: persistent Qt settings.
- `SaveGameRepository`: versioned, atomic JSON save files.
- `TileMapper`: Qt JSON/tile adapter used for loading, rendering, editing, and serialization.

### Presentation

- `AssetRepository`: loads and caches Qt resources.
- `PlayerPresentation`: resolves animation phases independently from physics.
- `SceneRenderer`: camera, world rendering, HUD, menu overlays, and editor overlay.
- `MainWindow`: Qt event wiring, audio, controller/session coordination, and frame presentation.

## Runtime Flow

1. `MainWindow` receives keyboard or mouse input.
2. `GameController` updates `InputState` or menu selection.
3. The fixed 16 ms timer calls `GameController::tick()`.
4. `Engine` delegates player movement to `PlayerPhysics`, then advances combat and world state.
5. Presentation controllers advance animation timelines.
6. `SceneRenderer` renders a complete frame.
7. `MainWindow` scales the frame with preserved aspect ratio and maps pointer coordinates back to source space.

## Data Ownership

- Gameplay state and events belong to `Engine`.
- Collision and entity definitions belong to `LevelDefinition`.
- The player body is the single source of truth for world position.
- Grid position is derived from the body and is not a second physics coordinate.
- Level lifecycle and persistence operations belong to `GameSession`.
- Visual assets belong to `AssetRepository`.
- Persistent settings and saves belong to their repositories.
- `MainWindow` coordinates these systems but does not implement gameplay rules.

## Coordinate Contract

- Level JSON uses grid cells.
- `playerStart` identifies the cell directly above the surface on which the player stands.
- `LevelDefinition` converts that cell into the player's world-space top-left body position.
- Physics and rendering share the same body bounds; renderers anchor sprites at the body's bottom center.
- Save files store world-space body coordinates. Version 1 saves are migrated to the current body dimensions when loaded.
- Collision resolution snaps the body to exact tile edges, preventing sub-pixel floor gaps and tile penetration.

## Dependency Direction

```text
MainWindow -> GameController / GameSession / SceneRenderer
GameSession -> TileMapper / SaveGameRepository / Engine
TileMapper -> LevelDefinition
Engine -> LevelDefinition / PlayerPhysics / Player / Enemy
SceneRenderer -> read-only Engine state / AssetRepository
```

Core code does not include Qt types. Qt adapters translate JSON, input, settings,
audio, and drawing into core-friendly values.
