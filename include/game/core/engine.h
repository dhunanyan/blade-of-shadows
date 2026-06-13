#ifndef GAME_CORE_ENGINE_H
#define GAME_CORE_ENGINE_H

#include <algorithm>
#include <optional>
#include <utility>
#include <vector>
#include <memory> // std::shared_ptr && std::unique_ptr
#include "game/core/direction.h"
#include "game/core/game_event.h"
#include "game/core/level_definition.h"
#include "game/core/player.h"
#include "game/core/player_physics.h"

struct Position;
class Enemy;


Position generateNewEnemyPosition(int width, int height);

class Engine
{
public:
  struct Snapshot
  {
    struct EnemyState
    {
      Position position;
      float life = 100.0f;
      float maxLife = 100.0f;
    };

    float playerPixelX = 0.0f;
    float playerPixelY = 0.0f;
    float playerVelocityY = 0.0f;
    Direction playerDirection = Direction::RIGHT;
    int remainingAirJumps = 1;
    int playerHealth = 5;
    int playerMaxHealth = 5;
    int coins = 0;
    int score = 0;
    std::vector<Position> remainingCoins;
    std::vector<EnemyState> enemies;
    bool levelComplete = false;
  };

  Engine(std::size_t stageWidth, std::size_t stageHeight);
  ~Engine();

  std::size_t stageWidthCells() const
  {
    return static_cast<std::size_t>(level_.width());
  }
  std::size_t stageHeightCells() const
  {
    return static_cast<std::size_t>(level_.height());
  }
  Position playerPosition() const
  {
    return player_.body().occupiedCell(level_.tileSize());
  }
  float playerPixelX() const
  {
    return player_.worldPosition().x;
  }
  float playerPixelY() const
  {
    return player_.worldPosition().y;
  }
  WorldRect playerBounds() const { return player_.worldBounds(); }
  const LevelDefinition& level() const { return level_; }
  Direction playerDirection() const
  {
    return player_.direction();
  }
  bool isPlayerAlive() const
  {
    return player_.isAlive();
  }
  int playerHealth() const { return player_.health(); }
  int playerMaxHealth() const { return player_.maxHealth(); }
  int playerCoins() const { return player_.coins(); }
  int playerScore() const { return player_.score(); }
  bool isLevelComplete() const { return levelComplete_; }
  const std::vector<Position>& coins() const { return coins_; }
  const std::optional<Position>& levelExit() const { return levelExit_; }
  std::vector<GameEvent> takeEvents()
  {
    std::vector<GameEvent> result;
    result.swap(events_);
    return result;
  }
  void setPlayerPosition(const Position& position)
  {
    if (!level_.isInside(position))
    {
      return;
    }
    player_.setPosition(position);
    playerPhysics_.spawn(player_, position);
  }
  void setPlayerPixelX(float pixelX);
  void setPlayerPixelY(float pixelY);
  void setPlayerMoveIntentX(int intentX)
  {
    playerMoveIntentX_ = std::clamp(intentX, -1, 1);
  }
  int playerMoveIntentX() const
  {
    return playerMoveIntentX_;
  }
  void setPlayerDirection(Direction direction)
  {
    player_.setDirection(direction);
  }
  void requestPlayerAttack()
  {
    player_.setAttackRequested(true);
  }
  void setPlayerAttackHeld(bool isHeld)
  {
    player_.setAttackHeld(isHeld);
  }
  bool isPlayerAttackInProgress() const
  {
    return player_.attackInProgress();
  }
  int playerAttackTicksLeft() const
  {
    return player_.attackTicksLeft();
  }
  void requestPlayerJump()
  {
    player_.setJumpRequested(true);
  }
  void requestPlayerDodge() { player_.setDodgeRequested(true); }
  bool isPlayerDodging() const { return player_.isDodging(); }
  void setPlayerJumpHeld(bool isHeld)
  {
    player_.setJumpHeld(isHeld);
  }
  bool playerIsGrounded() const
  {
    return player_.isGrounded();
  }
  float playerVelocityY() const
  {
    return player_.velocityY();
  }
  int playerDoubleJumpFxTicks() const
  {
    return player_.doubleJumpFxTicks();
  }
  bool willPlayerTouchGroundSoon(float lookAheadPx) const;
  void resetSession(
      Position playerSpawn,
      const std::vector<Position>& enemySpawns,
      const std::vector<Position>& coinSpawns,
      std::optional<Position> levelExit);
  Snapshot snapshot() const;
  void restoreSnapshot(const Snapshot& snapshot);
  void loadLevel(LevelDefinition level);
  void resetSession();
  void update();
  const std::vector<std::shared_ptr<Enemy>>& enemies() const
  {
    return enemies_;
  }

protected:
  void randEnemies(Position (*positionGenerator)(int, int)=generateNewEnemyPosition);
  void updateEnemies();
  void updatePlayerAttackState(Player& player);
  void updatePlayerDodgeState(Player& player);
  void resolvePlayerAttack();
  void resolveEnemyContact();
  void resolveCollectibles();
  void resolveLevelExit();
  bool canEnemyOccupy(const Position& position) const;

private:
  bool isSolidAt(int gridX, int gridY) const;

private:
  Player player_;
  LevelDefinition level_;
  PlayerPhysics playerPhysics_;

  constexpr static std::size_t maxEnemies_ = 10;
  std::vector<std::shared_ptr<Enemy>> enemies_;
  int playerMoveIntentX_ = 0;
  std::vector<Position> coins_;
  std::optional<Position> levelExit_;
  bool levelComplete_ = false;
  int updateTick_ = 0;
  std::vector<GameEvent> events_;
};

#endif // GAME_CORE_ENGINE_H
