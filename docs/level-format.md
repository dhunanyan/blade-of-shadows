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

## Custom Levels

The editor serializes custom levels as individual tiles. It preserves player spawn, enemies, coins, exit, collision flags, and source tileset coordinates.
