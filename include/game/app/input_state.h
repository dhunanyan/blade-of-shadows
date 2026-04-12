#ifndef GAME_APP_INPUT_STATE_H
#define GAME_APP_INPUT_STATE_H

enum class InputAction
{
  Up,
  Down,
  Left,
  Right,
  Attack,
  Unknown
};

struct InputState
{
  bool jumpPressed = false;
  bool jumpJustPressed = false;
  bool downPressed = false;
  bool leftPressed = false;
  bool rightPressed = false;
  bool attackPressed = false;
  bool attackJustPressed = false;

  void setPressed(InputAction action, bool isPressed, bool isAutoRepeat)
  {
    switch (action)
    {
    case InputAction::Up:
      if (isPressed && !isAutoRepeat && !jumpPressed) {
        jumpJustPressed = true;
      }
      jumpPressed = isPressed;
      break;
    case InputAction::Down:
      downPressed = isPressed;
      break;
    case InputAction::Left:
      leftPressed = isPressed;
      break;
    case InputAction::Right:
      rightPressed = isPressed;
      break;
    case InputAction::Attack:
      if (isPressed && !isAutoRepeat && !attackPressed)
      {
        attackJustPressed = true;
      }
      attackPressed = isPressed;
      break;
    case InputAction::Unknown:
      break;
    }
  }

  void consumeOneShotSignals()
  {
    attackJustPressed = false;
    jumpJustPressed = false;
  }

  int moveIntentX() const
  {
    if (leftPressed == rightPressed)
    {
      return 0;
    }
    return leftPressed ? -1 : 1;
  }
};

#endif // GAME_APP_INPUT_STATE_H
