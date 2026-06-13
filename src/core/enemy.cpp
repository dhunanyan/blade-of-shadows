#include "game/core/enemy.h"
#include <algorithm>

constexpr const float MAX_LIFE = 100.0f;

Enemy::Enemy(Position position, float maxLife)
  : Object(ObjectType::OBJECT_ENEMY, position), 
    life_(maxLife), maxLife_(maxLife)
  {}
Enemy::Enemy(Position position): Enemy(position, MAX_LIFE) {}
void Enemy::setContactDamage(int value)
{
  contactDamage_ = std::max(0, value);
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
