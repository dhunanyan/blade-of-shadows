#include "game/renderer/animation_controller.h"

void AnimationController::setClip(PlayerAnimationState state, const AnimationClip& clip)
{
  clips_[state] = clip;
}

void AnimationController::setState(PlayerAnimationState state)
{
  if (state_ == state)
  {
    return;
  }

  state_ = state;
  reset();
}

void AnimationController::tick(int frameCount)
{
  if (frameCount <= 0)
  {
    frameIndex_ = 0;
    frameAccumulator_ = 0.0;
    return;
  }

  const auto it = clips_.find(state_);
  const AnimationClip clip = (it == clips_.end()) ? AnimationClip{} : it->second;
  if (clip.framesPerTick <= 0.0)
  {
    return;
  }

  frameAccumulator_ += clip.framesPerTick;
  while (frameAccumulator_ >= 1.0)
  {
    frameAccumulator_ -= 1.0;

    if (clip.looping)
    {
      frameIndex_ = (frameIndex_ + 1) % frameCount;
    }
    else if (frameIndex_ < frameCount - 1)
    {
      ++frameIndex_;
    }
  }
}

bool AnimationController::isOneShotFinished(int frameCount) const
{
  if (frameCount <= 0)
  {
    return true;
  }

  const auto it = clips_.find(state_);
  if (it == clips_.end() || it->second.looping)
  {
    return false;
  }

  return frameIndex_ >= frameCount - 1;
}

void AnimationController::reset()
{
  frameIndex_ = 0;
  frameAccumulator_ = 0.0;
}
