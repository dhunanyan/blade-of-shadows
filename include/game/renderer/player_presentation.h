#ifndef GAME_RENDERER_PLAYER_PRESENTATION_H
#define GAME_RENDERER_PLAYER_PRESENTATION_H

#include <QPixmap>
#include "game/app/input_state.h"
#include "game/core/direction.h"
#include "game/core/engine.h"
#include "game/renderer/animation_controller.h"
#include "game/renderer/asset_repository.h"
#include "game/renderer/player_animation_state.h"

class PlayerPresentation
{
public:
  PlayerPresentation();

  void update(const InputState& input, const Engine& engine, const AssetRepository& assets);
  QPixmap currentFrame(const AssetRepository& assets) const;
  bool isFacingLeft(Direction direction) const;
  PlayerAnimationState state() const
  {
    return currentState_;
  }

private:
  enum class AirPhase
  {
    None,
    JumpStart,
    JumpLoop,
    JumpEnd,
    FallStart,
    FallLoop,
    FallEnd
  };

  struct ClipWindow
  {
    PlayerAnimationState state = PlayerAnimationState::Idle;
    int from = 1;
    int to = 1;
    double framesPerTick = 0.1;
    bool loop = true;
  };

  PlayerAnimationState resolveGroundedState(const InputState& input) const;
  void setVisualWindow(const ClipWindow& window);
  int windowFrameCount(const AssetRepository& assets) const;
  void transitionToAirPhase(AirPhase nextPhase);
  ClipWindow clipForAirPhase(AirPhase phase) const;
  void updateAirPhase(const Engine& engine, int frameCount);
  int currentWindowFrameIndex(const std::vector<QPixmap>& clip) const;
  int currentAttackFrameIndex(const std::vector<QPixmap>& clip) const;

private:
  AnimationController animationController_;
  PlayerAnimationState currentState_ = PlayerAnimationState::Idle;
  AirPhase airPhase_ = AirPhase::None;
  int activeFrameFrom_ = 1;
  int activeFrameTo_ = 1;
  double activeFramesPerTick_ = 0.1;
  bool activeLoop_ = true;
  bool wasGrounded_ = true;
  AnimationController attackAnimationController_;
  bool attackVisualActive_ = false;
  int previousAttackTicksLeft_ = 0;
};

#endif // GAME_RENDERER_PLAYER_PRESENTATION_H
