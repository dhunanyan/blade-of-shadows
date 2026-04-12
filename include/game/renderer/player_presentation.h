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
  PlayerAnimationState resolveState(const InputState& input) const;
  void setState(PlayerAnimationState nextState, bool isAttackActive);

private:
  AnimationController animationController_;
  PlayerAnimationState currentState_ = PlayerAnimationState::Idle;
};

#endif // GAME_RENDERER_PLAYER_PRESENTATION_H
