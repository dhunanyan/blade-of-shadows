#include "game/app/game_controller.h"
#include "game/app/input_mapper.h"

GameController::GameController(std::size_t stageWidth, std::size_t stageHeight)
  : engine_(stageWidth, stageHeight)
{
}

void GameController::onKeyEvent(int key, bool isPressed, bool isAutoRepeat)
{
  InputMapper::applyQtKey(input_, key, isPressed, isAutoRepeat);
}

void GameController::applyInput()
{
  engine_.setPlayerMoveIntentX(input_.moveIntentX());
  engine_.setPlayerAttackHeld(input_.attackPressed);
  if (input_.attackJustPressed)
  {
    engine_.requestPlayerAttack();
  }
}

void GameController::tick()
{
  applyInput();
  engine_.update();
  input_.consumeOneShotSignals();
}
