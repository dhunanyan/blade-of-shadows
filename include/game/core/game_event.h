#ifndef GAME_CORE_GAME_EVENT_H
#define GAME_CORE_GAME_EVENT_H

enum class GameEvent
{
  AttackStarted,
  JumpStarted,
  DoubleJumpStarted,
  CoinCollected,
  PlayerDamaged,
  PlayerDefeated,
  LevelCompleted
};

#endif // GAME_CORE_GAME_EVENT_H
