#ifndef GAME_APP_INPUT_MAPPER_H
#define GAME_APP_INPUT_MAPPER_H

#include "game/app/input_state.h"

class InputMapper
{
public:
  static InputAction mapQtKey(int key);
  static void applyQtKey(InputState& state, int key, bool isPressed, bool isAutoRepeat);
};

#endif // GAME_APP_INPUT_MAPPER_H
