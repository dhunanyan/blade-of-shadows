#include "game/renderer/player_presentation.h"

PlayerPresentation::PlayerPresentation()
{
  animationController_.setClip(PlayerAnimationState::Attack, AnimationClip{0.33, false});
  animationController_.setClip(PlayerAnimationState::Damage, AnimationClip{0.20, true});
  animationController_.setClip(PlayerAnimationState::Death, AnimationClip{0.20, true});
  animationController_.setClip(PlayerAnimationState::Dodge, AnimationClip{0.20, true});
  animationController_.setClip(PlayerAnimationState::DodgeMove, AnimationClip{0.20, true});
  animationController_.setClip(PlayerAnimationState::Fall, AnimationClip{0.20, true});
  animationController_.setClip(PlayerAnimationState::Idle, AnimationClip{0.067, true});
  animationController_.setClip(PlayerAnimationState::Jump, AnimationClip{0.20, true});
  animationController_.setClip(PlayerAnimationState::Run, AnimationClip{0.15, true});
}

void PlayerPresentation::update(const InputState& input, const AssetRepository& assets)
{
  if (input.attackJustPressed)
  {
    attackRequested_ = true;
  }

  setState(resolveState(input));

  const int frameCount = static_cast<int>(assets.playerClip(currentState_).size());
  animationController_.tick(frameCount);

  if (currentState_ == PlayerAnimationState::Attack && frameCount > 0 &&
      animationController_.isOneShotFinished(frameCount))
  {
    attackInProgress_ = false;
    if (input.attackPressed)
    {
      attackRequested_ = true;
    }
  }
}

QPixmap PlayerPresentation::currentFrame(const AssetRepository& assets) const
{
  const auto& clip = assets.playerClip(currentState_);
  if (clip.empty())
  {
    return QPixmap();
  }

  const int frameCount = static_cast<int>(clip.size());
  const int index = animationController_.frameIndex() % frameCount;
  return clip[static_cast<std::size_t>(index)];
}

bool PlayerPresentation::isFacingLeft(Direction direction) const
{
  return direction == Direction::LEFT ||
         direction == Direction::UPPER_LEFT ||
         direction == Direction::DOWNER_LEFT;
}

PlayerAnimationState PlayerPresentation::state() const
{
  return currentState_;
}

PlayerAnimationState PlayerPresentation::resolveState(const InputState& input) const
{
  if (attackInProgress_ || attackRequested_ || input.attackPressed)
  {
    return PlayerAnimationState::Attack;
  }

  if ((input.leftPressed || input.rightPressed) && input.downPressed)
  {
    return PlayerAnimationState::DodgeMove;
  }

  if (input.downPressed)
  {
    return PlayerAnimationState::Dodge;
  }

  if (input.upPressed)
  {
    return PlayerAnimationState::Jump;
  }

  if (input.leftPressed || input.rightPressed)
  {
    return PlayerAnimationState::Run;
  }

  return PlayerAnimationState::Idle;
}

void PlayerPresentation::setState(PlayerAnimationState nextState)
{
  if (currentState_ == nextState)
  {
    if (currentState_ == PlayerAnimationState::Attack && !attackInProgress_ &&
        (attackRequested_))
    {
      animationController_.reset();
      attackInProgress_ = true;
      attackRequested_ = false;
    }
    return;
  }

  currentState_ = nextState;
  animationController_.setState(currentState_);

  if (currentState_ == PlayerAnimationState::Attack)
  {
    attackInProgress_ = true;
    attackRequested_ = false;
  }
}
