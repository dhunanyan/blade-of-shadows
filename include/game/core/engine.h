#ifndef ENGINE_H
#define ENGINE_H

#include <algorithm>
#include <functional>
#include <vector>
#include <memory> // std::shared_ptr && std::unique_ptr
#include "game/core/direction.h"
#include "game/core/player.h"
#include "game/core/stage.h"

struct Position;
class Bullet;
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
  void setPlayerMoveIntentX(int intentX)
  {
    playerMoveIntentX_ = std::clamp(intentX, -1, 1);
  }
  void setPlayerDirection(Direction direction)
  {
    player_.setDirection(direction);
  }
  void playerShoots();
  void movePlayerUp();
  void movePlayerDown();
  void movePlayerLeft();
  void movePlayerRight();
  void movePlayerUpRight();
  void movePlayerUpLeft();
  void movePlayerDownRight();
  void movePlayerDownLeft();
  void setSolidQuery(std::function<bool(int, int)> solidQuery);
  void applyGravity(Player& player);
  void applyGravity(Enemy& enemy);

  const std::vector<Bullet>& bullets() const
  {
    return bullets_;
  }
  void moveBulletTowardsDirection(Bullet& bullet);
  void update();
  const std::vector<std::shared_ptr<Enemy>>& enemies() const
  {
    return enemies_;
  }

protected:
  void randEnemies(Position (*positionGenerator)(int, int)=generateNewEnemyPosition);
  void updateEnemies();
  void updateBullets();
  void applyHorizontalMovement(Player& player);
  void syncPlayerGridPosition();

private:
  bool isBulletOutOfBounds(const Bullet& bullet) const;
  bool isSolidAt(int gridX, int gridY) const;

private:
  Player player_;
  Stage stage_;
  std::vector<Bullet> bullets_;

  constexpr static std::size_t maxEnemies_ = 10;
  std::vector<std::shared_ptr<Enemy>> enemies_;
  std::function<bool(int, int)> solidQuery_;
  float playerPixelX_ = 0.0f;
  float playerPixelY_ = 0.0f;
  int playerMoveIntentX_ = 0;
};

#endif // ENGINE_H
