#ifndef PLAYER_ANIMATION_STATE_H
#define PLAYER_ANIMATION_STATE_H

#include <cstdint> // std::uint8_t

enum class PlayerAnimationState : std::uint8_t
{
  Attack,
  Damage,
  Death,
  Dodge,
  DodgeMove,
  Fall,
  Idle,
  Jump,
  Run,
};

#endif // PLAYER_ANIMATION_STATE_H
