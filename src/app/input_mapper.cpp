#include "game/app/input_mapper.h"
#include <Qt>

InputAction InputMapper::mapQtKey(int key)
{
  switch (key)
  {
  case Qt::Key_Up:
  case Qt::Key_W:
    return InputAction::Up;
  case Qt::Key_Down:
  case Qt::Key_S:
    return InputAction::Down;
  case Qt::Key_Left:
  case Qt::Key_A:
    return InputAction::Left;
  case Qt::Key_Right:
  case Qt::Key_D:
    return InputAction::Right;
  case Qt::Key_Space:
  case Qt::Key_J:
    return InputAction::Attack;
  default:
    return InputAction::Unknown;
  }
}

void InputMapper::applyQtKey(InputState& state, int key, bool isPressed, bool isAutoRepeat)
{
  state.setPressed(mapQtKey(key), isPressed, isAutoRepeat);
}
