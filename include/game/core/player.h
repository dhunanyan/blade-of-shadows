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
  bool attackHeld_ = false;
  bool attackRequested_ = false;
  bool attackInProgress_ = false;
  int attackTicksLeft_ = 0;
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
  bool attackHeld() const
  {
    return attackHeld_;
  }
  void setAttackHeld(bool attackHeld)
  {
    attackHeld_ = attackHeld;
  }
  bool attackRequested() const
  {
    return attackRequested_;
  }
  void setAttackRequested(bool attackRequested)
  {
    attackRequested_ = attackRequested;
  }
  bool attackInProgress() const
  {
    return attackInProgress_;
  }
  void setAttackInProgress(bool attackInProgress)
  {
    attackInProgress_ = attackInProgress;
  }
  int attackTicksLeft() const
  {
    return attackTicksLeft_;
  }
  void setAttackTicksLeft(int ticks)
  {
    attackTicksLeft_ = ticks;
  }
};

#endif // GAME_CORE_PLAYER_H
