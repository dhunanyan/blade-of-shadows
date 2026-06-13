#ifndef GAME_CORE_PLAYER_H
#define GAME_CORE_PLAYER_H

#include "game/core/object.h"
#include "game/core/direction.h"
#include "game/core/world_geometry.h"
 
class Player : public Object
{
private:
  bool isAlive_ = true;
  Direction direction_ = Direction::RIGHT;
  int maxHealth_ = 5;
  int health_ = 5;
  int coins_ = 0;
  int score_ = 0;
  int invulnerabilityTicks_ = 0;
  bool isGrounded_ = false;
  bool jumpHeld_ = false;
  bool jumpRequested_ = false;
  float jumpHoldTime_ = 0.0f;
  int remainingAirJumps_ = 1;
  int doubleJumpFxTicks_ = 0;
  int jumpApexHangTicks_ = 0;
  bool attackHeld_ = false;
  bool attackRequested_ = false;
  bool attackInProgress_ = false;
  bool attackDamageApplied_ = false;
  int attackTicksLeft_ = 0;
  bool dodgeRequested_ = false;
  int dodgeTicks_ = 0;
  int dodgeCooldownTicks_ = 0;
  PhysicsBody body_{{16.0f, 32.0f}};

public:
  Player();
  Direction direction() const { return direction_; }
  void setDirection(Direction v) { direction_ = v; }
  bool isAlive() const { return isAlive_; }
  void setIsAlive(bool v) { isAlive_ = v; }
  int maxHealth() const { return maxHealth_; }
  int health() const { return health_; }
  int coins() const { return coins_; }
  int score() const { return score_; }
  int invulnerabilityTicks() const { return invulnerabilityTicks_; }
  void setMaxHealth(int value);
  void setHealth(int value);
  void setCoins(int value);
  void setScore(int value);
  void setInvulnerabilityTicks(int value);
  void takeDamage(int damage);
  void heal(int amount);
  void addCoin(int value = 1);
  void addScore(int value);
  void resetProgress();
  float velocityY() const { return body_.velocity().y; }
  void setVelocityY(float value) { body_.setVelocityY(value); }
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
  int jumpApexHangTicks() const { return jumpApexHangTicks_; }
  void setJumpApexHangTicks(int v) { jumpApexHangTicks_ = v; }
  bool attackHeld() const { return attackHeld_; }
  void setAttackHeld(bool v) { attackHeld_ = v; }
  bool attackRequested() const { return attackRequested_; }
  void setAttackRequested(bool v) { attackRequested_ = v; }
  bool attackInProgress() const { return attackInProgress_; }
  void setAttackInProgress(bool v) { attackInProgress_ = v; }
  bool attackDamageApplied() const { return attackDamageApplied_; }
  void setAttackDamageApplied(bool v) { attackDamageApplied_ = v; }
  int attackTicksLeft() const { return attackTicksLeft_; }
  void setAttackTicksLeft(int v) { attackTicksLeft_ = v; }
  bool dodgeRequested() const { return dodgeRequested_; }
  void setDodgeRequested(bool value) { dodgeRequested_ = value; }
  int dodgeTicks() const { return dodgeTicks_; }
  void setDodgeTicks(int value) { dodgeTicks_ = value; }
  int dodgeCooldownTicks() const { return dodgeCooldownTicks_; }
  void setDodgeCooldownTicks(int value) { dodgeCooldownTicks_ = value; }
  bool isDodging() const { return dodgeTicks_ > 0; }
  const PhysicsBody& body() const { return body_; }
  PhysicsBody& body() { return body_; }
  WorldPoint worldPosition() const { return body_.position(); }
  WorldRect worldBounds() const { return body_.bounds(); }
  void setWorldPosition(WorldPoint position) { body_.setPosition(position); }
};

#endif // GAME_CORE_PLAYER_H
