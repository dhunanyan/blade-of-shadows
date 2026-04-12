#ifndef GAME_APP_GAME_CONTROLLER_H
#define GAME_APP_GAME_CONTROLLER_H

#include <cstddef>
#include "game/app/input_state.h"
#include "game/core/engine.h"

class GameController
{
public:
  GameController(std::size_t stageWidth, std::size_t stageHeight);

  Engine& engine()
  {
    return engine_;
  }

  const Engine& engine() const
  {
    return engine_;
  }

  InputState& input()
  {
    return input_;
  }

  const InputState& input() const
  {
    return input_;
  }

  void onKeyEvent(int key, bool isPressed, bool isAutoRepeat);
  void applyInput();
  void tick();

private:
  Engine engine_;
  InputState input_;
};

#endif // GAME_APP_GAME_CONTROLLER_H
