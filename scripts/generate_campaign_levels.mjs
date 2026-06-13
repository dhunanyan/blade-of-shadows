import fs from "node:fs";
import path from "node:path";
import { fileURLToPath } from "node:url";

const scriptDir = path.dirname(fileURLToPath(import.meta.url));
const projectRoot = path.resolve(scriptDir, "..");
const groupsDir = path.join(projectRoot, "assets/images/tiles/groups");
const levelsDir = path.join(projectRoot, "assets/levels");

const levelNames = [
  "Moonlit Approach",
  "Cavern of Echoes",
  "Lanternwood Crossing",
  "Ruins Beneath Rain",
  "The Ember Stair",
  "Moss-Crowned Ramparts",
  "Hollow Market",
  "Bridges of Blackwater",
  "The Broken Aqueduct",
  "Warden's Garden",
  "Ashen Bell District",
  "Thornwatch Passage",
  "The Sunken Archive",
  "Gloamstone Foundry",
  "Pilgrim's Last Road",
  "Citadel of Quiet Knives",
  "The Ironroot Depths",
  "Palace of Falling Stars",
  "Shadowgate Ascent",
  "The Last Moon Shrine",
];

const decorationIds = [
  "shop",
  "shop_anim",
  "fence_1",
  "fence_2",
  "sign",
  "rock_1",
  "rock_2",
  "rock_3",
  "grass_1",
  "grass_2",
  "grass_3",
  "lamp",
];

function groupCoordinates(groupName) {
  const directory = fs.readdirSync(groupsDir).find((name) => name.includes(groupName));
  if (!directory) {
    throw new Error(`Missing tile group: ${groupName}`);
  }
  return fs.readdirSync(path.join(groupsDir, directory))
    .map((name) => name.match(/_x(\d+)_y(\d+)\.png$/))
    .filter(Boolean)
    .map((match) => ({ srcX: Number(match[1]), srcY: Number(match[2]) }))
    .sort((a, b) => a.srcY - b.srcY || a.srcX - b.srcX);
}

const allTerrain = fs.readdirSync(groupsDir)
  .flatMap((directory) => fs.readdirSync(path.join(groupsDir, directory)))
  .map((name) => name.match(/_x(\d+)_y(\d+)\.png$/))
  .filter(Boolean)
  .map((match) => ({ srcX: Number(match[1]), srcY: Number(match[2]) }))
  .filter((tile, index, tiles) =>
    tiles.findIndex((other) => other.srcX === tile.srcX && other.srcY === tile.srcY) === index)
  .sort((a, b) => a.srcY - b.srcY || a.srcX - b.srcX);

const groundTop = groupCoordinates("ground_strip_bottom").filter((tile) => tile.srcY === 13);
const groundFill = groupCoordinates("ground_strip_bottom").filter((tile) => tile.srcY === 14);
const platformSets = [
  groupCoordinates("platform_top_long_4"),
  groupCoordinates("platform_mid_3"),
  groupCoordinates("platform_mid_2"),
  groupCoordinates("platform_low_3"),
  groupCoordinates("platform_low_2"),
  groupCoordinates("platform_bottom_3"),
];
const caps = [
  ...groupCoordinates("platform_top_cap_a"),
  ...groupCoordinates("platform_top_cap_b"),
  ...groupCoordinates("platform_top_cap_c"),
  ...groupCoordinates("platform_top_cap_d"),
  ...groupCoordinates("platform_top_cap_e"),
  ...groupCoordinates("platform_top_cap_f"),
  ...groupCoordinates("platform_top_cap_g"),
];
const pillars = [
  groupCoordinates("pillar_single_2_high_a"),
  groupCoordinates("pillar_double_2wide_5high"),
  groupCoordinates("pillar_double_2x3"),
];

function rng(seed) {
  let state = seed >>> 0;
  return () => {
    state = (state * 1664525 + 1013904223) >>> 0;
    return state / 0x100000000;
  };
}

function buildLevel(index) {
  const random = rng(0xB10DE + index * 7919);
  const width = 190 + index * 6;
  const height = 25;
  const tiles = new Map();
  const key = (x, y) => `${x},${y}`;
  const setTile = (x, y, source) => {
    if (x < 0 || y < 0 || x >= width || y >= height) return;
    tiles.set(key(x, y), {
      x,
      y,
      srcX: source.srcX,
      srcY: source.srcY,
      solid: true,
    });
  };

  // A readable surface above a dense, varied cross-section. Every campaign
  // map uses the complete classified terrain atlas below its walkable layer.
  for (let x = 0; x < width; ++x) {
    setTile(x, 20, groundTop[(x * 3 + index) % groundTop.length]);
    for (let y = 21; y < height; ++y) {
      const atlasIndex =
        (x + y * width + index * 17) % allTerrain.length;
      setTile(x, y, allTerrain[atlasIndex]);
    }
  }
  for (let atlasIndex = 0; atlasIndex < allTerrain.length; ++atlasIndex) {
    const x = 8 + atlasIndex;
    const y = 21 + (atlasIndex % 4);
    setTile(x, y, allTerrain[atlasIndex]);
  }

  // Alternating platform routes form a skyline rather than a flat obstacle
  // course. Gaps remain within the player's jump/double-jump envelope.
  let platformX = 10;
  let platformNumber = 0;
  while (platformX < width - 12) {
    const sourceSet = platformSets[(platformNumber + index) % platformSets.length];
    const platformY = 17 - ((platformNumber + index) % 3);
    const length = 3 + ((platformNumber + index) % 4);
    for (let offset = 0; offset < length; ++offset) {
      setTile(
        platformX + offset,
        platformY,
        sourceSet[offset % sourceSet.length]);
    }
    if (platformNumber % 3 === 1) {
      setTile(
        platformX + Math.floor(length / 2),
        platformY - 3,
        caps[(platformNumber + index) % caps.length]);
    }
    platformX += 8 + Math.floor(random() * 5);
    ++platformNumber;
  }

  // Architectural punctuation: pillars, arches, and stepped ruins.
  for (let section = 0; section < 8; ++section) {
    const startX = 22 + section * Math.floor((width - 38) / 8);
    const pillar = pillars[(section + index) % pillars.length];
    const columns = Math.max(...pillar.map((tile) => tile.srcX)) -
      Math.min(...pillar.map((tile) => tile.srcX)) + 1;
    const minX = Math.min(...pillar.map((tile) => tile.srcX));
    const minY = Math.min(...pillar.map((tile) => tile.srcY));
    const rows = Math.max(...pillar.map((tile) => tile.srcY)) - minY + 1;
    for (const tile of pillar) {
      setTile(
        startX + tile.srcX - minX,
        20 - rows + tile.srcY - minY,
        tile);
    }
    for (let step = 0; step < 3; ++step) {
      setTile(
        startX + columns + step,
        19 - step,
        caps[(section * 3 + step + index) % caps.length]);
    }
  }

  // Small intentional surface pits with stepping stones make later stages
  // less linear without creating impossible mandatory jumps.
  if (index >= 3) {
    for (let pit = 0; pit < 3 + Math.floor(index / 4); ++pit) {
      const start = 34 + pit * Math.floor((width - 60) / (4 + Math.floor(index / 4)));
      const size = 1 + ((pit + index) % 2);
      for (let x = start; x < start + size; ++x) {
        tiles.delete(key(x, 20));
      }
      setTile(start, 18, caps[(pit + index) % caps.length]);
    }
  }

  const decorations = [];
  const usedDecorationCells = new Set();
  for (let decorationIndex = 0; decorationIndex < decorationIds.length; ++decorationIndex) {
    let x = 7 + decorationIndex * Math.floor((width - 16) / decorationIds.length);
    x += (index * 3 + decorationIndex * 5) % 5;
    while (usedDecorationCells.has(x) || tiles.has(key(x, 19))) ++x;
    usedDecorationCells.add(x);
    decorations.push({
      type: decorationIds[decorationIndex],
      x,
      y: 19,
    });
  }
  for (let cluster = 0; cluster < 18; ++cluster) {
    let x = 12 + Math.floor((cluster + 1) * (width - 24) / 19);
    while (tiles.has(key(x, 19))) ++x;
    decorations.push({
      type: decorationIds[(cluster * 3 + index) % decorationIds.length],
      x,
      y: 19,
    });
  }

  const enemies = [];
  for (let x = 20 + (index % 4); x < width - 12; x += Math.max(13, 21 - Math.floor(index / 2))) {
    if (!tiles.has(key(x, 19))) {
      enemies.push({ x, y: 19 });
    }
  }

  const coins = [];
  for (let x = 8; x < width - 5; x += 8 + (index % 3)) {
    const platformY = [16, 17, 18].find((candidate) => tiles.has(key(x, candidate)));
    const y = platformY === undefined ? 19 : platformY - 1;
    if (!tiles.has(key(x, y))) {
      coins.push({ x, y });
    }
  }

  return {
    id: `level_${String(index + 1).padStart(2, "0")}`,
    name: levelNames[index],
    displayName: levelNames[index],
    width,
    height,
    tileSize: 24,
    tileset: ":/tiles/tileset.png",
    playerStart: { x: 3, y: 19 },
    tiles: [...tiles.values()].sort((a, b) => a.y - b.y || a.x - b.x),
    decorations,
    enemies,
    coins,
    exit: { x: width - 4, y: 19 },
  };
}

fs.mkdirSync(levelsDir, { recursive: true });
fs.writeFileSync(
  path.join(projectRoot, "assets/images/tiles/catalog.json"),
  `${JSON.stringify(allTerrain, null, 2)}\n`);
for (let index = 0; index < levelNames.length; ++index) {
  const level = buildLevel(index);
  const usedTerrain = new Set(level.tiles.map((tile) => `${tile.srcX},${tile.srcY}`));
  const usedDecorations = new Set(level.decorations.map((decoration) => decoration.type));
  const solidCells = new Set(level.tiles.map((tile) => `${tile.x},${tile.y}`));
  if (usedTerrain.size !== allTerrain.length) {
    throw new Error(`${level.id} does not use the complete terrain catalog`);
  }
  if (usedDecorations.size !== decorationIds.length) {
    throw new Error(`${level.id} does not use the complete decoration catalog`);
  }
  for (const entity of [
    level.playerStart,
    level.exit,
    ...level.enemies,
    ...level.coins,
  ]) {
    if (solidCells.has(`${entity.x},${entity.y}`)) {
      throw new Error(
        `${level.id} places an entity inside solid terrain at ${entity.x},${entity.y}`);
    }
  }
  if (!solidCells.has(`${level.playerStart.x},${level.playerStart.y + 1}`) ||
      !solidCells.has(`${level.exit.x},${level.exit.y + 1}`)) {
    throw new Error(`${level.id} spawn or exit is not supported by terrain`);
  }
  const file = path.join(
    levelsDir,
    `level_${String(index + 1).padStart(2, "0")}.json`);
  fs.writeFileSync(file, `${JSON.stringify(level)}\n`);
}

console.log(
  `Generated ${levelNames.length} campaign levels using ${allTerrain.length} terrain tiles and ${decorationIds.length} decorations.`);
