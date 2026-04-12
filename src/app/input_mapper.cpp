#include "game/app/input_mapper.h"
#include <Qt>

InputAction InputMapper::mapQtKey(int key)
{
  switch (key)
  {
  case Qt::Key_Up:
    return InputAction::Up;
  case Qt::Key_Down:
    return InputAction::Down;
  case Qt::Key_Left:
    return InputAction::Left;
  case Qt::Key_Right:
    return InputAction::Right;
  case Qt::Key_Space:
    return InputAction::Attack;
  default:
    return InputAction::Unknown;
  }
}

void InputMapper::applyQtKey(InputState& state, int key, bool isPressed, bool isAutoRepeat)
{
  state.setPressed(mapQtKey(key), isPressed, isAutoRepeat);
}
