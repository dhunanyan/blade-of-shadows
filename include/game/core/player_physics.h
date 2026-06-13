#ifndef GAME_CORE_PLAYER_PHYSICS_H
#define GAME_CORE_PLAYER_PHYSICS_H

#include <vector>
#include "game/core/game_event.h"
#include "game/core/level_definition.h"
#include "game/core/player.h"

class PlayerPhysics
{
public:
  explicit PlayerPhysics(const LevelDefinition& level)
      : level_(level)
  {
  }

  void spawn(Player& player, Position requestedSpawn) const;
  void update(
      Player& player,
      int movementIntent,
      std::vector<GameEvent>& events) const;

  bool canOccupy(const WorldRect& bounds) const;
  bool isGrounded(const PhysicsBody& body, float lookAhead = 0.5f) const;
  bool willTouchGroundSoon(const Player& player, float lookAhead) const;
  bool setWorldPosition(Player& player, WorldPoint position) const;
  void synchronizeGridPosition(Player& player) const;

private:
  void handleJump(Player& player, std::vector<GameEvent>& events) const;
  void moveHorizontally(Player& player, int movementIntent) const;
  void applyGravity(Player& player) const;
  void moveAlongAxis(Player& player, float amount, bool horizontal) const;

  const LevelDefinition& level_;
};

#endif // GAME_CORE_PLAYER_PHYSICS_H
