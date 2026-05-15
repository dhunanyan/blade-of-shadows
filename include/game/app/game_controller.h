#ifndef GAME_APP_GAME_CONTROLLER_H
#define GAME_APP_GAME_CONTROLLER_H

#include <cstddef>
#include "game/app/input_state.h"
#include "game/app/menu_system.h"
#include "game/core/engine.h"

enum class GameMode
{
  Menu,
  Playing,
  LevelEditor
};

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
  GameMode mode() const
  {
    return mode_;
  }
  MenuView menuView() const
  {
    return menuSystem_.view();
  }
  bool shouldQuit() const
  {
    return shouldQuit_;
  }

  void onKeyEvent(int key, bool isPressed, bool isAutoRepeat);
  void onMenuHover(int hoveredIndex);
  void onMenuClick(int clickedIndex);
  void applyInput();
  void tick();

private:
  void buildMenus();
  void startGame();
  void startLevelEditor();
  void resumeFromPause();

private:
  Engine engine_;
  InputState input_;
  MenuSystem menuSystem_;
  GameMode mode_ = GameMode::Menu;
  GameMode modeBeforePause_ = GameMode::Playing;
  bool shouldQuit_ = false;
};

#endif // GAME_APP_GAME_CONTROLLER_H
