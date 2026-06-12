#ifndef GAME_CORE_ENGINE_H
#define GAME_CORE_ENGINE_H

#include <algorithm>
#include <functional>
#include <optional>
#include <utility>
#include <vector>
#include <memory> // std::shared_ptr && std::unique_ptr
#include "game/core/direction.h"
#include "game/core/player.h"
#include "game/core/stage.h"

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
    return stage_.width();
  }
  std::size_t stageHeightCells() const
  {
    return stage_.height();
  }
  Position playerPosition() const
  {
    return player_.position();
  }
  float playerPixelX() const
  {
    return playerPixelX_;
  }
  float playerPixelY() const
  {
    return playerPixelY_;
  }
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
  void setPlayerPosition(const Position& position)
  {
    if (!stage_.isInside(position))
    {
      return;
    }
    player_.setPosition(position);
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
  void setSolidQuery(std::function<bool(int, int)> solidQuery)
  {
    solidQuery_ = std::move(solidQuery);
  }
  void applyGravity(Player& player);
  void applyGravity(Enemy& enemy);

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
  void applyHorizontalMovement(Player& player);
  void syncPlayerGridPosition();
  void resolvePlayerAttack();
  void resolveEnemyContact();
  void resolveCollectibles();
  void resolveLevelExit();
  bool canEnemyOccupy(const Position& position) const;

private:
  bool isSolidAt(int gridX, int gridY) const;
  void handlePlayerJump(Player& player);

private:
  Player player_;
  Stage stage_;

  constexpr static std::size_t maxEnemies_ = 10;
  std::vector<std::shared_ptr<Enemy>> enemies_;
  std::function<bool(int, int)> solidQuery_;
  float playerPixelX_ = 0.0f;
  float playerPixelY_ = 0.0f;
  int playerMoveIntentX_ = 0;
  std::vector<Position> coins_;
  std::optional<Position> levelExit_;
  bool levelComplete_ = false;
  int updateTick_ = 0;
};

#endif // GAME_CORE_ENGINE_H
