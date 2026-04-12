#ifndef GAME_RENDERER_ANIMATION_CONTROLLER_H
#define GAME_RENDERER_ANIMATION_CONTROLLER_H

#include <unordered_map>
#include "game/renderer/player_animation_state.h"

struct AnimationClip
{
  double framesPerTick = 0.1;
  bool looping = true;
};

class AnimationController
{
public:
  void setClip(PlayerAnimationState state, const AnimationClip& clip);
  void setState(PlayerAnimationState state);
  void tick(int frameCount);

  PlayerAnimationState state() const
  {
    return state_;
  }

  int frameIndex() const
  {
    return frameIndex_;
  }

  bool isOneShotFinished(int frameCount) const;
  void reset();

private:
  std::unordered_map<PlayerAnimationState, AnimationClip> clips_;
  PlayerAnimationState state_ = PlayerAnimationState::Idle;
  int frameIndex_ = 0;
  double frameAccumulator_ = 0.0;
};

#endif // GAME_RENDERER_ANIMATION_CONTROLLER_H
