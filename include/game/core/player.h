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
  bool jumpHeld_ = false;
  bool jumpRequested_ = false;
  float jumpHoldTime_ = 0.0f;
  int remainingAirJumps_ = 1;
  int doubleJumpFxTicks_ = 0;
  bool attackHeld_ = false;
  bool attackRequested_ = false;
  bool attackInProgress_ = false;
  int attackTicksLeft_ = 0;

public:
  Player();
  Direction direction() const { return direction_; }
  void setDirection(Direction v) { direction_ = v; }
  bool isAlive() const { return isAlive_; }
  void setIsAlive(bool v) { isAlive_ = v; }
  float velocityY() const { return velocityY_; }
  void setVelocityY(float v) { velocityY_ = v; }
  bool isGrounded() const { return isGrounded_; }
  void setIsGrounded(bool v) { isGrounded_ = v; }
  bool jumpHeld() const { return jumpHeld_; }
  void setJumpHeld(bool v) { jumpHeld_ = v; }
  bool jumpRequested() const { return jumpRequested_; }
  void setJumpRequested(bool v) { jumpRequested_ = v; }
  float jumpHoldTime() const { return jumpHoldTime_; }
  void setJumpHoldTime(float v) { jumpHoldTime_ = v; }
  int remainingAirJumps() const { return remainingAirJumps_; }
  void setRemainingAirJumps(int v) { remainingAirJumps_ = v; }
  int doubleJumpFxTicks() const { return doubleJumpFxTicks_; }
  void setDoubleJumpFxTicks(int v) { doubleJumpFxTicks_ = v; }
  bool attackHeld() const { return attackHeld_; }
  void setAttackHeld(bool v) { attackHeld_ = v; }
  bool attackRequested() const { return attackRequested_; }
  void setAttackRequested(bool v) { attackRequested_ = v; }
  bool attackInProgress() const { return attackInProgress_; }
  void setAttackInProgress(bool v) { attackInProgress_ = v; }
  int attackTicksLeft() const { return attackTicksLeft_; }
  void setAttackTicksLeft(int v) { attackTicksLeft_ = v; }
};

#endif // GAME_CORE_PLAYER_H
