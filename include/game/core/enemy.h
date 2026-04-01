#ifndef ENEMY_H
#define ENEMY_H

#include <chrono>
#include "game/core/object.h"

class Enemy : public Object {
private:
  float life_;
  float maxLife_;
  float velocityY_ = 0.0f;
  bool isGrounded_ = false;
  static constexpr std::chrono::milliseconds moveInterval_{500};
  mutable std::chrono::steady_clock::time_point lastMoveTime_;
public:
  Enemy(Position position);
  Enemy(Position position, float maxLife);
  float lifePercent() const;
  bool shouldIMoveThisTime() const;
  bool isAlive() const;
  void decreaseLife(float damage);
  void moveLeft();
  void moveUp();
  void moveRight();
  void moveDown();
  void setLastMoveTime(std::chrono::steady_clock::time_point lastMoveTime)
  {
    lastMoveTime_ = lastMoveTime;
  }
  float velocityY() const
  {
    return velocityY_;
  }
  void setVelocityY(float newVelocityY)
  {
    velocityY_ = newVelocityY;
  }
  bool isGrounded() const
  {
    return isGrounded_;
  }
  void setIsGrounded(bool newIsGrounded)
  {
    isGrounded_ = newIsGrounded;
  }
};

#endif // ENEMY_H
