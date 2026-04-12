#ifndef GAME_CORE_PLAYER_H
#define GAME_CORE_PLAYER_H

#include "game/core/object.h"
#include "game/core/direction.h"
 
class Player : public Object
{
private:
  bool isAlive_;
  Direction direction_;
  float velocityY_ = 0.0f;
  bool isGrounded_ = false;
public:
  Player();
  Direction direction() const
  {
    return direction_;
  }
  void setDirection(Direction direction)
  {
    direction_ = direction;
  }
  bool isAlive() const
  {
    return isAlive_;
  }
  void setIsAlive(bool isAlive)
  {
    isAlive_ = isAlive;
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

#endif // GAME_CORE_PLAYER_H
