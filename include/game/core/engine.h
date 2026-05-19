#ifndef GAME_CORE_ENGINE_H
#define GAME_CORE_ENGINE_H

#include <algorithm>
#include <functional>
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
  void setPlayerPosition(const Position& position)
  {
    if (!stage_.isInside(position))
    {
      return;
    }
    player_.setPosition(position);
  }
  void setPlayerPixelX(float pixelX)
  {
    playerPixelX_ = pixelX;
    syncPlayerGridPosition();
  }
  void setPlayerPixelY(float pixelY)
  {
    playerPixelY_ = pixelY;
    syncPlayerGridPosition();
  }
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
  void applyHorizontalMovement(Player& player);
  void syncPlayerGridPosition();

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
};

#endif // GAME_CORE_ENGINE_H
