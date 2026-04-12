#ifndef GAME_RENDERER_PLAYER_PRESENTATION_H
#define GAME_RENDERER_PLAYER_PRESENTATION_H

#include <QPixmap>
#include "game/app/input_state.h"
#include "game/core/direction.h"
#include "game/renderer/animation_controller.h"
#include "game/renderer/asset_repository.h"
#include "game/renderer/player_animation_state.h"

class PlayerPresentation
{
public:
  PlayerPresentation();

  void update(const InputState& input, const AssetRepository& assets);
  QPixmap currentFrame(const AssetRepository& assets) const;
  bool isFacingLeft(Direction direction) const;
  PlayerAnimationState state() const;

private:
  PlayerAnimationState resolveState(const InputState& input) const;
  void setState(PlayerAnimationState nextState);

private:
  AnimationController animationController_;
  PlayerAnimationState currentState_ = PlayerAnimationState::Idle;
  bool attackRequested_ = false;
  bool attackInProgress_ = false;
};

#endif // GAME_RENDERER_PLAYER_PRESENTATION_H
