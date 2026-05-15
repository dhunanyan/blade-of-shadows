#include "game/app/game_controller.h"
#include <Qt>
#include "game/app/input_mapper.h"

GameController::GameController(std::size_t stageWidth, std::size_t stageHeight)
  : engine_(stageWidth, stageHeight)
{
  buildMenus();
}

void GameController::onKeyEvent(int key, bool isPressed, bool isAutoRepeat)
{
  if (mode_ == GameMode::Menu)
  {
    if (!isPressed || isAutoRepeat)
    {
      return;
    }

    switch (key)
    {
    case Qt::Key_Up:
      menuSystem_.moveSelection(-1);
      break;
    case Qt::Key_Down:
      menuSystem_.moveSelection(1);
      break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Space:
      menuSystem_.activateSelected();
      break;
    case Qt::Key_Escape:
    case Qt::Key_Backspace:
      if (!menuSystem_.isRootMenuOpen())
      {
        menuSystem_.popMenu();
      }
      else if (menuSystem_.currentMenuId() == "pause")
      {
        resumeFromPause();
      }
      // On the main menu root, Esc does nothing by design.
      break;
    default:
      break;
    }
    return;
  }

  if (isPressed && !isAutoRepeat && key == Qt::Key_Escape)
  {
    modeBeforePause_ = mode_;
    mode_ = GameMode::Menu;
    menuSystem_.openRoot("pause");
    input_.resetAll();
    return;
  }

  InputMapper::applyQtKey(input_, key, isPressed, isAutoRepeat);
}

void GameController::onMenuHover(int hoveredIndex)
{
  if (mode_ != GameMode::Menu || hoveredIndex < 0)
  {
    return;
  }
  menuSystem_.setSelection(hoveredIndex);
}

void GameController::onMenuClick(int clickedIndex)
{
  if (mode_ != GameMode::Menu || clickedIndex < 0)
  {
    return;
  }
  menuSystem_.activateAt(clickedIndex);
}

void GameController::applyInput()
{
  engine_.setPlayerMoveIntentX(input_.moveIntentX());

  engine_.setPlayerAttackHeld(input_.attackPressed);
  if (input_.attackJustPressed)
  {
    engine_.requestPlayerAttack();
  }

  engine_.setPlayerJumpHeld(input_.jumpPressed);
  if (input_.jumpJustPressed) {
    engine_.requestPlayerJump();
  }
}

void GameController::tick()
{
  if (mode_ == GameMode::Menu)
  {
    input_.consumeOneShotSignals();
    return;
  }

  applyInput();
  engine_.update();
  input_.consumeOneShotSignals();
}

void GameController::buildMenus()
{
  menuSystem_.registerMenu(MenuDefinition{
      "main",
      "Main Menu",
      {
          MenuItem{"Start Game", [this]() { startGame(); }},
          MenuItem{"Level Editor", [this]() { startLevelEditor(); }},
          MenuItem{"Options", [this]() { menuSystem_.pushMenu("options"); }},
          MenuItem{"Exit", [this]() { shouldQuit_ = true; }},
      }});

  menuSystem_.registerMenu(MenuDefinition{
      "pause",
      "Paused",
      {
          MenuItem{"Resume", [this]() { resumeFromPause(); }},
          MenuItem{"Options", [this]() { menuSystem_.pushMenu("options"); }},
          MenuItem{"Main Menu", [this]() { mode_ = GameMode::Menu; menuSystem_.openRoot("main"); input_.resetAll(); }},
          MenuItem{"Exit", [this]() { shouldQuit_ = true; }},
      }});

  menuSystem_.registerMenu(MenuDefinition{
      "options",
      "Options",
      {
          MenuItem{"Back", [this]() { menuSystem_.popMenu(); }},
      }});

  menuSystem_.openRoot("main");
}

void GameController::startGame()
{
  mode_ = GameMode::Playing;
  menuSystem_.popMenu();
  input_.resetAll();
}

void GameController::startLevelEditor()
{
  mode_ = GameMode::LevelEditor;
  menuSystem_.popMenu();
  input_.resetAll();
}

void GameController::resumeFromPause()
{
  mode_ = modeBeforePause_;
  menuSystem_.popMenu();
  input_.resetAll();
}
