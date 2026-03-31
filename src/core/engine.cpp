#include <algorithm>
#include <cstdlib>
#include "game/core/engine.h"
#include "game/core/player.h"
#include "game/core/stage.h"
#include "game/core/bullet.h"
#include "game/core/enemy.h"


float Bullet::speed_ = 2;
float Bullet::damage_ = 23;

namespace
{
inline auto signum(int x)
{
  return (x > 0) - (x < 0);
}

std::size_t defaultLaneY(int height)
{
  return static_cast<std::size_t>(height / 2);
}
} // namespace


Position generateNewEnemyPosition(int width, int height)
{
  const auto yLane = defaultLaneY(height);
  const auto xEdge = (rand() % 2 == 0) ? 0 : static_cast<std::size_t>(width - 1);
  return Position(xEdge, yLane);
}


Engine::~Engine() = default;


Engine::Engine(std::size_t stageWidth, std::size_t stageHeight):
  stage_(stageWidth, stageHeight)
{
  const decltype(Position::x_) middleX = stage_.width() / 2;
  const decltype(Position::y_) middleY = stage_.height() / 2;
  player_.setPosition(Position{middleX, middleY});
  player_.setDirection(Direction::RIGHT);
  player_.setIsAlive(true);
}

void Engine::update()
{
  updateBullets();
  updateEnemies();
  randEnemies();
}

void Engine::updateBullets()
{
  for (auto& bullet : bullets_)
  {
    moveBulletTowardsDirection(bullet);
    for (const auto& enemy : enemies_)
    {
      if(enemy->position().x() == bullet.position().x() && enemy->position().y() == bullet.position().y())
      {
        enemy->decreaseLife(bullet.damage());
      }
    }
  }

  bullets_.erase(
    std::remove_if(
      bullets_.begin(), bullets_.end(),
      [this](const Bullet& bullet) 
      { 
        return isBulletOutOfBounds(bullet); 
      }),
      bullets_.end()
  );
}

void Engine::moveBulletTowardsDirection(Bullet& bullet)
{
  switch (bullet.direction())
  {
    case Direction::UP:
      bullet.moveUp();
      break;
    case Direction::DOWN:
      bullet.moveDown();
      break;
    case Direction::LEFT:
      bullet.moveLeft();
      break;
    case Direction::UPPER_LEFT:
      bullet.moveUpLeft();
      break;
    case Direction::UPPER_RIGHT:
      bullet.moveUpRight();
      break;
    case Direction::DOWNER_LEFT:
      bullet.moveDownLeft();
      break;
    case Direction::DOWNER_RIGHT:
      bullet.moveDownRight();
      break;
    default:
      bullet.moveRight();
      break;
  }
}

void Engine::updateEnemies()
{
  enemies_.erase(
      std::remove_if(
          enemies_.begin(), enemies_.end(),
          [](const std::shared_ptr<Enemy>& enemy)
          { 
            return !enemy || !enemy->isAlive();
          }
      ),
      enemies_.end());

  const Position playerPos = player_.position();
  for (const auto& enemy : enemies_)
  {
    if (!enemy || !enemy->shouldIMoveThisTime())
    {
      continue;
    }

    const int dx = signum(static_cast<int>(playerPos.x()) - static_cast<int>(enemy->position().x()));
    if (dx > 0)
    {
      enemy->moveRight();
    }
    else if (dx < 0)
    {
      enemy->moveLeft();
    }
  }
}

void Engine::movePlayerUp()
{
  // Side-scroller mode: vertical movement is disabled for now (jump is planned).
}

void Engine::movePlayerDown()
{
  // Side-scroller mode: vertical movement is disabled for now (jump is planned).
}

void Engine::movePlayerLeft()
{
  player_.setDirection(Direction::LEFT);
  Position next_position = Position(player_.position().x_ - 1, player_.position().y_);
  if(!stage_.isInside(next_position))
  {
    return;
  }
  player_.moveLeft();
}
void Engine::movePlayerRight()
{
  player_.setDirection(Direction::RIGHT);
  Position next_position = Position(player_.position().x_ + 1, player_.position().y_);
  if(!stage_.isInside(next_position))
  {
    return;
  }
  player_.moveRight();
}

void Engine::movePlayerUpRight()
{
  // Side-scroller mode: diagonal movement is disabled for now.
}

void Engine::movePlayerUpLeft()
{
  // Side-scroller mode: diagonal movement is disabled for now.
}

void Engine::movePlayerDownRight()
{
  // Side-scroller mode: diagonal movement is disabled for now.
}

void Engine::movePlayerDownLeft()
{
  // Side-scroller mode: diagonal movement is disabled for now.
}

void Engine::playerShoots()
{
  Position bulletPosition = player_.position();
  switch (player_.direction())
  {
    case Direction::UP:
      bulletPosition.moveUp();
      break;
    case Direction::DOWN:
      bulletPosition.moveDown();
      break;
    case Direction::LEFT:
      bulletPosition.moveLeft();
      break;
    case Direction::UPPER_LEFT:
      bulletPosition.moveUpLeft();
      break;
    case Direction::UPPER_RIGHT:
      bulletPosition.moveUpRight();
      break;
    case Direction::DOWNER_LEFT:
      bulletPosition.moveDownLeft();
      break;
    case Direction::DOWNER_RIGHT:
      bulletPosition.moveDownRight();
      break;
    default:
      bulletPosition.moveRight();
      break;
  }
  bullets_.push_back(Bullet(player_.direction(), bulletPosition));
}

Position Engine::playerPosition() const
{
  return player_.position();
}

Direction Engine::playerDirection() const
{
  return player_.direction();
}

bool Engine::isPlayerAlive() const
{
  return player_.isAlive();
}

std::size_t Engine::stageWidthCells() const
{
  return stage_.width();
}
std::size_t Engine::stageHeightCells() const
{
  return stage_.height();
}

const std::vector<Bullet>& Engine::bullets() const
{
  return bullets_;
}

const std::vector<std::shared_ptr<Enemy>>& Engine::enemies() const
{
  return enemies_;
}

bool Engine::isBulletOutOfBounds(const Bullet& bullet) const
{
  return !stage_.isInside(bullet.position());
}

void Engine::randEnemies(Position (*positionGenerator)(int,int))
{
  while (enemies_.size() < maxEnemies_)
  {
    const Position enemyPosition = positionGenerator(static_cast<int>(stageWidthCells()), static_cast<int>(stageHeightCells()));
    enemies_.push_back(std::make_shared<Enemy>(enemyPosition));
  }
}
