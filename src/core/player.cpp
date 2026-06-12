#include "game/core/player.h"
#include <algorithm>

Player::Player()
  : Object(ObjectType::OBJECT_PLAYER)
{
}

void Player::setMaxHealth(int value)
{
  maxHealth_ = std::max(1, value);
  health_ = std::clamp(health_, 0, maxHealth_);
  isAlive_ = health_ > 0;
}

void Player::setHealth(int value)
{
  health_ = std::clamp(value, 0, maxHealth_);
  isAlive_ = health_ > 0;
}

void Player::setCoins(int value)
{
  coins_ = std::max(0, value);
}

void Player::setScore(int value)
{
  score_ = std::max(0, value);
}

void Player::setInvulnerabilityTicks(int value)
{
  invulnerabilityTicks_ = std::max(0, value);
}

void Player::takeDamage(int damage)
{
  if (damage <= 0 || invulnerabilityTicks_ > 0 || !isAlive_)
  {
    return;
  }
  setHealth(health_ - damage);
}

void Player::heal(int amount)
{
  if (amount > 0 && isAlive_)
  {
    setHealth(health_ + amount);
  }
}

void Player::addCoin(int value)
{
  coins_ += std::max(0, value);
}

void Player::addScore(int value)
{
  score_ += std::max(0, value);
}

void Player::resetProgress()
{
  health_ = maxHealth_;
  coins_ = 0;
  score_ = 0;
  invulnerabilityTicks_ = 0;
  isAlive_ = true;
}
