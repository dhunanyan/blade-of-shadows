#include "game/core/enemy.h"

constexpr const float MAX_LIFE = 100.0f;

Enemy::Enemy(Position position, float maxLife)
  : Object(ObjectType::OBJECT_ENEMY, position), 
    life_(maxLife), maxLife_(maxLife),
    lastMoveTime_(std::chrono::steady_clock::now())
  {}
Enemy::Enemy(Position position): Enemy(position, MAX_LIFE) {}
float Enemy::lifePercent() const
{
  return (life_ / maxLife_) * 100.0f;
}
bool Enemy::shouldIMoveThisTime() const
{
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastMoveTime_);
    return elapsedTime >= moveInterval_;
}
bool Enemy::isAlive() const
{
  return lifePercent() > 0;
}
void Enemy::decreaseLife(float damage)
{
  if (life_ - damage <= 0) 
  {
    life_ = 0.0f;
    return;
  }
  life_ -= damage;
}
void Enemy::setLastMoveTime(std::chrono::steady_clock::time_point lastMoveTime)
{
  lastMoveTime_ = lastMoveTime;
}
void Enemy::moveLeft()
{
  Object::moveLeft();
  setLastMoveTime(std::chrono::steady_clock::now());
}
void Enemy::moveUp()
{
  Object::moveUp();
  setLastMoveTime(std::chrono::steady_clock::now());
}
void Enemy::moveRight()
{
  Object::moveRight();
  setLastMoveTime(std::chrono::steady_clock::now());
}
void Enemy::moveDown()
{
  Object::moveDown();
  setLastMoveTime(std::chrono::steady_clock::now());
}
