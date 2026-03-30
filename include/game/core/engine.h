#ifndef ENGINE_H
#define ENGINE_H

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

  std::size_t stageWidthCells() const;
  std::size_t stageHeightCells() const;
  Position playerPosition() const;
  Direction playerDirection() const;
  bool isPlayerAlive() const;
  void playerShoots();
  void movePlayerUp();
  void movePlayerDown();
  void movePlayerLeft();
  void movePlayerRight();
  void movePlayerUpRight();
  void movePlayerUpLeft();
  void movePlayerDownRight();
  void movePlayerDownLeft();

  const std::vector<Bullet>& bullets() const;
  void moveBulletTowardsDirection(Bullet& bullet);
  void update();
  const std::vector<std::shared_ptr<Enemy>>& enemies() const;

protected:
  void randEnemies(Position (*positionGenerator)(int, int)=generateNewEnemyPosition);
  void updateEnemies();
  void updateBullets();

private:
  bool isBulletOutOfBounds(const Bullet& bullet) const;

private:
  Player player_;
  Stage stage_;
  std::vector<Bullet> bullets_;

  constexpr static std::size_t maxEnemies_ = 10;
  std::vector<std::shared_ptr<Enemy>> enemies_;
};

#endif // ENGINE_H
