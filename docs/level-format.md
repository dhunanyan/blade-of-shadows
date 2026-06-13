# Level Format

Levels are JSON files loaded by `TileMapper`.

## Required Fields

```json
{
  "id": "level_01",
  "name": "Moonlit Approach",
  "width": 60,
  "height": 25,
  "tileSize": 24,
  "tileset": ":/tiles/tileset.png",
  "playerStart": { "x": 3, "y": 19 }
}
```

## Tiles

Individual tiles:

```json
{
  "x": 8,
  "y": 19,
  "srcX": 10,
  "srcY": 4,
  "solid": false
}
```

Compact rectangles:

```json
{
  "x": 0,
  "y": 20,
  "width": 60,
  "height": 5,
  "srcX": 6,
  "srcY": 14,
  "topSrcX": 6,
  "topSrcY": 13,
  "solid": true
}
```

`topSrcX` and `topSrcY` are optional and allow the first row to use a surface tile.

## Entities

```json
{
  "enemies": [{ "x": 14, "y": 19 }],
  "coins": [{ "x": 7, "y": 19 }],
  "decorations": [
    { "type": "grass_1", "x": 9, "y": 19 },
    { "type": "lamp", "x": 18, "y": 19 }
  ],
  "exit": { "x": 58, "y": 19 }
}
```

The exit unlocks after all coins are collected.

`playerStart` is a standing grid cell, not a raw sprite or pixel coordinate. The
player body's feet are placed exactly on the bottom edge of that cell. Spawn
validation checks the entire player body rather than only the selected cell. If
the body would overlap a solid tile, the closest valid standing cell is used.

Enemy, coin, and exit cells must not overlap solid tiles. The editor prevents
placing entities inside collision cells and removes entities when a solid tile
is painted over their cell.

Terrain tiles are collision-bearing world geometry. Decorative, non-solid
visuals use the `decorations` collection instead of terrain entries with
`"solid": false`.

## Custom Levels

The editor serializes custom levels as individual `.json` files. It preserves
player spawn, enemies, coins, exit, collision flags, and source tileset
coordinates. New levels use sequential filenames such as
`custom_level_001.json`; saving an already loaded custom level updates its
existing file.

Custom levels live in the platform-specific
`Dhunanyan/Blade of Shadows/levels` application data directory and are always
shown in the level-selection menu.
