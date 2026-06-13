#ifndef GAME_CORE_LEVEL_DEFINITION_H
#define GAME_CORE_LEVEL_DEFINITION_H

#include <optional>
#include <string>
#include <vector>
#include "game/core/position.h"
#include "game/core/world_geometry.h"

class LevelDefinition
{
public:
  LevelDefinition(int width = 1, int height = 1, int tileSize = 24);

  const std::string& id() const { return id_; }
  const std::string& displayName() const { return displayName_; }
  int width() const { return width_; }
  int height() const { return height_; }
  int tileSize() const { return tileSize_; }
  const Position& playerSpawn() const { return playerSpawn_; }
  const std::vector<Position>& enemySpawns() const { return enemySpawns_; }
  const std::vector<Position>& coinSpawns() const { return coinSpawns_; }
  const std::optional<Position>& exit() const { return exit_; }

  void setId(std::string id) { id_ = std::move(id); }
  void setDisplayName(std::string name) { displayName_ = std::move(name); }
  void setPlayerSpawn(Position spawn) { playerSpawn_ = spawn; }
  void setEnemySpawns(std::vector<Position> spawns) { enemySpawns_ = std::move(spawns); }
  void setCoinSpawns(std::vector<Position> spawns) { coinSpawns_ = std::move(spawns); }
  void setExit(std::optional<Position> exit) { exit_ = exit; }

  bool isInside(Position cell) const;
  bool isSolid(Position cell) const;
  bool isSolid(int x, int y) const { return isSolid(Position(x, y)); }
  void setSolid(Position cell, bool solid);
  Position resolveStandingSpawn(
      Position requested,
      WorldSize bodySize = {}) const;
  WorldPoint worldPositionForStandingCell(Position cell, WorldSize bodySize) const;
  WorldRect cellBounds(Position cell) const;

private:
  std::size_t index(Position cell) const;

  std::string id_ = "level";
  std::string displayName_ = "Level";
  int width_ = 1;
  int height_ = 1;
  int tileSize_ = 24;
  Position playerSpawn_;
  std::vector<bool> solidCells_;
  std::vector<Position> enemySpawns_;
  std::vector<Position> coinSpawns_;
  std::optional<Position> exit_;
};

#endif // GAME_CORE_LEVEL_DEFINITION_H
