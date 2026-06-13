#ifndef GAME_CORE_ENEMY_H
#define GAME_CORE_ENEMY_H

#include "game/core/object.h"

class Enemy : public Object {
private:
  float life_;
  float maxLife_;
  int contactDamage_ = 1;
public:
  Enemy(Position position);
  Enemy(Position position, float maxLife);
  float lifePercent() const
  {
    return (life_ / maxLife_) * 100.0f;
  }
  float life() const { return life_; }
  float maxLife() const { return maxLife_; }
  int contactDamage() const { return contactDamage_; }
  void setContactDamage(int value);
  bool isAlive() const
  {
    return lifePercent() > 0;
  }
  void decreaseLife(float damage);
};

#endif // GAME_CORE_ENEMY_H
