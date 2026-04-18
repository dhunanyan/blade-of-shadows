#include <algorithm>
#include "game/renderer/player_presentation.h"

PlayerPresentation::PlayerPresentation()
{
  setVisualWindow(ClipWindow{
      PlayerAnimationState::Idle,
      1,
      6,
      0.067,
      true});
  attackAnimationController_.setClip(PlayerAnimationState::Attack, AnimationClip{0.33, false});
  attackAnimationController_.setState(PlayerAnimationState::Attack);
}

void PlayerPresentation::update(const InputState& input, const Engine& engine, const AssetRepository& assets)
{
  const bool attackActive = engine.isPlayerAttackInProgress();
  const int attackTicksLeft = engine.playerAttackTicksLeft();
  const bool grounded = engine.playerIsGrounded();
  if (!grounded && airPhase_ == AirPhase::None)
  {
    const float vy = engine.playerVelocityY();
    if (vy < 0.0f)
    {
      transitionToAirPhase(AirPhase::JumpStart);
    }
    else
    {
      transitionToAirPhase(AirPhase::FallStart);
    }
  }

  if (airPhase_ != AirPhase::None)
  {
    setVisualWindow(clipForAirPhase(airPhase_));
  }
  else
  {
    const PlayerAnimationState groundedState = resolveGroundedState(input);
    switch (groundedState)
    {
    case PlayerAnimationState::DodgeMove:
      setVisualWindow(ClipWindow{PlayerAnimationState::DodgeMove, 1, 3, 0.20, true});
      break;
    case PlayerAnimationState::Dodge:
      setVisualWindow(ClipWindow{PlayerAnimationState::Dodge, 1, 3, 0.20, true});
      break;
    case PlayerAnimationState::Run:
      setVisualWindow(ClipWindow{PlayerAnimationState::Run, 1, 8, 0.15, true});
      break;
    case PlayerAnimationState::Idle:
    default:
      setVisualWindow(ClipWindow{PlayerAnimationState::Idle, 1, 6, 0.067, true});
      break;
    }
  }

  const int frameCount = windowFrameCount(assets);
  animationController_.tick(frameCount);

  if (airPhase_ != AirPhase::None)
  {
    updateAirPhase(engine, frameCount);
  }

  wasGrounded_ = grounded;

  if (attackActive)
  {
    if (!attackVisualActive_ || attackTicksLeft > previousAttackTicksLeft_)
    {
      attackAnimationController_.reset();
    }

    const int attackFrameCount =
        static_cast<int>(assets.playerAttackVariantClip(currentState_).size());
    attackAnimationController_.tick(attackFrameCount);
    attackVisualActive_ = true;
  }
  else
  {
    attackVisualActive_ = false;
  }

  previousAttackTicksLeft_ = attackTicksLeft;
}

QPixmap PlayerPresentation::currentFrame(const AssetRepository& assets) const
{
  const auto& baseClip = assets.playerClip(currentState_);
  if (baseClip.empty())
  {
    return QPixmap();
  }

  const std::vector<QPixmap>* selectedClip = &baseClip;
  if (attackVisualActive_)
  {
    const auto& attackVariantClip = assets.playerAttackVariantClip(currentState_);
    if (!attackVariantClip.empty())
    {
      selectedClip = &attackVariantClip;
    }
  }

  const auto& clip = *selectedClip;
  if (clip.empty())
  {
    return QPixmap();
  }

  const int frameIndex = (attackVisualActive_ && selectedClip != &baseClip)
                             ? currentAttackFrameIndex(clip)
                             : currentWindowFrameIndex(clip);
  return clip[static_cast<std::size_t>(frameIndex)];
}

bool PlayerPresentation::isFacingLeft(Direction direction) const
{
  return direction == Direction::LEFT ||
         direction == Direction::UPPER_LEFT ||
         direction == Direction::DOWNER_LEFT;
}

PlayerAnimationState PlayerPresentation::resolveGroundedState(const InputState& input) const
{
  if ((input.leftPressed || input.rightPressed) && input.downPressed)
  {
    return PlayerAnimationState::DodgeMove;
  }

  if (input.downPressed)
  {
    return PlayerAnimationState::Dodge;
  }

  if (input.leftPressed || input.rightPressed)
  {
    return PlayerAnimationState::Run;
  }

  return PlayerAnimationState::Idle;
}

void PlayerPresentation::setVisualWindow(const ClipWindow& window)
{
  const PlayerAnimationState previousState = currentState_;
  const bool changed =
      (currentState_ != window.state) ||
      (activeFrameFrom_ != window.from) ||
      (activeFrameTo_ != window.to) ||
      (activeFramesPerTick_ != window.framesPerTick) ||
      (activeLoop_ != window.loop);

  if (!changed)
  {
    return;
  }

  currentState_ = window.state;
  activeFrameFrom_ = window.from;
  activeFrameTo_ = window.to;
  activeFramesPerTick_ = window.framesPerTick;
  activeLoop_ = window.loop;

  animationController_.setClip(currentState_, AnimationClip{activeFramesPerTick_, activeLoop_});
  if (previousState != currentState_)
  {
    animationController_.setState(currentState_);
  }
  else
  {
    // Same state with a different frame window (for example FallLoop -> FallEnd):
    // restart local playback so the new window starts from its first frame.
    animationController_.reset();
  }
}

int PlayerPresentation::windowFrameCount(const AssetRepository& assets) const
{
  const auto& clip = assets.playerClip(currentState_);
  if (clip.empty())
  {
    return 0;
  }

  const int clipSize = static_cast<int>(clip.size());
  const int start = std::clamp(activeFrameFrom_ - 1, 0, clipSize - 1);
  const int end = std::clamp(activeFrameTo_ - 1, start, clipSize - 1);
  return end - start + 1;
}

int PlayerPresentation::currentWindowFrameIndex(const std::vector<QPixmap>& clip) const
{
  if (clip.empty())
  {
    return 0;
  }

  const int clipSize = static_cast<int>(clip.size());
  const int start = std::clamp(activeFrameFrom_ - 1, 0, clipSize - 1);
  const int end = std::clamp(activeFrameTo_ - 1, start, clipSize - 1);
  const int frameCount = end - start + 1;
  if (frameCount <= 0)
  {
    return start;
  }

  int localIndex = animationController_.frameIndex();
  if (activeLoop_)
  {
    localIndex %= frameCount;
  }
  else
  {
    localIndex = std::min(localIndex, frameCount - 1);
  }

  return start + localIndex;
}

int PlayerPresentation::currentAttackFrameIndex(const std::vector<QPixmap>& clip) const
{
  if (clip.empty())
  {
    return 0;
  }

  const int frameCount = static_cast<int>(clip.size());
  const int frameIndex = std::clamp(attackAnimationController_.frameIndex(), 0, frameCount - 1);
  return frameIndex;
}

void PlayerPresentation::transitionToAirPhase(AirPhase nextPhase)
{
  if (airPhase_ == nextPhase)
  {
    return;
  }
  airPhase_ = nextPhase;
  setVisualWindow(clipForAirPhase(airPhase_));
}

PlayerPresentation::ClipWindow PlayerPresentation::clipForAirPhase(AirPhase phase) const
{
  switch (phase)
  {
  case AirPhase::JumpStart:
    return ClipWindow{PlayerAnimationState::Jump, 1, 4, 0.33, false};
  case AirPhase::JumpLoop:
    return ClipWindow{PlayerAnimationState::Jump, 3, 4, 0.33, true};
  case AirPhase::JumpEnd:
    return ClipWindow{PlayerAnimationState::Jump, 4, 8, 0.33, false};
  case AirPhase::FallStart:
    return ClipWindow{PlayerAnimationState::Fall, 1, 4, 0.55, false};
  case AirPhase::FallLoop:
    return ClipWindow{PlayerAnimationState::Fall, 3, 4, 0.50, true};
  case AirPhase::FallEnd:
    return ClipWindow{PlayerAnimationState::Fall, 4, 8, 0.55, false};
  case AirPhase::None:
  default:
    return ClipWindow{PlayerAnimationState::Idle, 1, 6, 0.067, true};
  }
}

void PlayerPresentation::updateAirPhase(const Engine& engine, int frameCount)
{
  constexpr float apexEpsilon = 0.05f;
  constexpr float fallEndLeadPx = 8.0f;
  const float vy = engine.playerVelocityY();
  const bool grounded = engine.playerIsGrounded();
  const bool oneShotFinished =
      !activeLoop_ && frameCount > 0 && animationController_.isOneShotFinished(frameCount);

  if (grounded && !wasGrounded_ && airPhase_ != AirPhase::FallEnd)
  {
    transitionToAirPhase(AirPhase::FallEnd);
    const int fallEndFrameCount = std::max(0, activeFrameTo_ - activeFrameFrom_ + 1);
    if (fallEndFrameCount > 0)
    {
      animationController_.forceLastFrame(fallEndFrameCount);
    }
    airPhase_ = AirPhase::None;
    return;
  }

  if (!grounded &&
      (airPhase_ == AirPhase::FallStart || airPhase_ == AirPhase::FallLoop) &&
      engine.willPlayerTouchGroundSoon(fallEndLeadPx))
  {
    transitionToAirPhase(AirPhase::FallEnd);
  }

  switch (airPhase_)
  {
  case AirPhase::JumpStart:
    if (oneShotFinished)
    {
      transitionToAirPhase(vy < -apexEpsilon ? AirPhase::JumpLoop : AirPhase::JumpEnd);
    }
    break;

  case AirPhase::JumpLoop:
    if (vy >= -apexEpsilon)
    {
      transitionToAirPhase(AirPhase::JumpEnd);
    }
    break;

  case AirPhase::JumpEnd:
    if (oneShotFinished)
    {
      if (vy > apexEpsilon)
      {
        transitionToAirPhase(
            engine.willPlayerTouchGroundSoon(fallEndLeadPx) ? AirPhase::FallEnd : AirPhase::FallStart);
      }
      else
      {
        transitionToAirPhase(AirPhase::JumpEnd);
      }
    }
    break;

  case AirPhase::FallStart:
    if (oneShotFinished)
    {
      transitionToAirPhase(grounded ? AirPhase::FallEnd : AirPhase::FallLoop);
    }
    break;

  case AirPhase::FallLoop:
    if (grounded)
    {
      transitionToAirPhase(AirPhase::FallEnd);
    }
    break;

  case AirPhase::FallEnd:
    if (oneShotFinished)
    {
      airPhase_ = AirPhase::None;
    }
    break;

  case AirPhase::None:
  default:
    break;
  }
}
