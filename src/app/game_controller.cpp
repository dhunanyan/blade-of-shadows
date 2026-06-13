#include "game/app/game_controller.h"
#include <algorithm>
#include <utility>
#include <Qt>
#include "game/app/input_mapper.h"

GameController::GameController(std::size_t stageWidth, std::size_t stageHeight)
  : engine_(stageWidth, stageHeight)
{
  buildMenus();
  menuSystem_.openRoot("main");
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
    case Qt::Key_W:
      menuSystem_.moveSelection(-1);
      break;
    case Qt::Key_Down:
    case Qt::Key_S:
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
    menuSystem_.openRoot(modeBeforePause_ == GameMode::LevelEditor ? "editor" : "pause");
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
  if (input_.downJustPressed)
  {
    engine_.requestPlayerDodge();
  }
}

void GameController::tick()
{
  if (mode_ == GameMode::Menu)
  {
    input_.consumeOneShotSignals();
    return;
  }

  if (mode_ == GameMode::Playing)
  {
    applyInput();
    engine_.update();
  }
  input_.consumeOneShotSignals();

  if (mode_ == GameMode::Playing && !engine_.isPlayerAlive())
  {
    mode_ = GameMode::Menu;
    menuSystem_.openRoot("game_over");
    input_.resetAll();
  }
  else if (mode_ == GameMode::Playing && engine_.isLevelComplete())
  {
    mode_ = GameMode::Menu;
    menuSystem_.openRoot("victory");
    input_.resetAll();
  }
}

void GameController::buildMenus()
{
  menuSystem_.registerMenu(MenuDefinition{
      "main",
      text("Main Menu", "Menu glowne"),
      {
          MenuItem{text("New Game", "Nowa gra"), [this]() { startGame(true); }},
          MenuItem{text("Load Game", "Wczytaj gre"), [this]() { loadGame(); }},
          MenuItem{text("Level Editor", "Edytor poziomow"), [this]() { startLevelEditor(); }},
          MenuItem{text("Settings", "Ustawienia"), [this]() { menuSystem_.pushMenu("options"); }},
          MenuItem{text("Credits", "Autorzy"), [this]() { menuSystem_.pushMenu("credits"); }},
          MenuItem{text("Exit", "Wyjscie"), [this]() { shouldQuit_ = true; }},
      }});

  menuSystem_.registerMenu(MenuDefinition{
      "pause",
      text("Paused", "Pauza"),
      {
          MenuItem{text("Resume", "Wznow"), [this]() { resumeFromPause(); }},
          MenuItem{text("Save Game", "Zapisz gre"), [this]() { saveGame(); }},
          MenuItem{text("Load Game", "Wczytaj gre"), [this]() { loadGame(); }},
          MenuItem{text("Settings", "Ustawienia"), [this]() { menuSystem_.pushMenu("options"); }},
          MenuItem{text("Main Menu", "Menu glowne"), [this]() { returnToMainMenu(); }},
          MenuItem{text("Exit", "Wyjscie"), [this]() { shouldQuit_ = true; }},
      }});

  menuSystem_.registerMenu(MenuDefinition{
      "game_over",
      text("Defeated", "Przegrana"),
      {
          MenuItem{text("Retry", "Sprobuj ponownie"), [this]() { startGame(false); }},
          MenuItem{text("Load Game", "Wczytaj gre"), [this]() { loadGame(); }},
          MenuItem{text("Main Menu", "Menu glowne"), [this]() { returnToMainMenu(); }},
          MenuItem{text("Exit", "Wyjscie"), [this]() { shouldQuit_ = true; }},
      }});

  menuSystem_.registerMenu(MenuDefinition{
      "editor",
      text("Level Editor", "Edytor poziomow"),
      {
          MenuItem{text("Continue Editing", "Kontynuuj edycje"), [this]() { resumeFromPause(); }},
          MenuItem{text("Save Level", "Zapisz poziom"), [this]() { saveLevel(); }},
          MenuItem{text("Play Level", "Zagraj w poziom"), [this]() { playEditedLevel(); }},
          MenuItem{text("Settings", "Ustawienia"), [this]() { menuSystem_.pushMenu("options"); }},
          MenuItem{text("Main Menu", "Menu glowne"), [this]() { returnToMainMenu(); }},
          MenuItem{text("Exit", "Wyjscie"), [this]() { shouldQuit_ = true; }},
      }});

  menuSystem_.registerMenu(MenuDefinition{
      "victory",
      text("Level Complete", "Poziom ukonczony"),
      {
          MenuItem{text("Next Level", "Nastepny poziom"), [this]() { startNextLevel(); }},
          MenuItem{text("Replay Level", "Powtorz poziom"), [this]() { startGame(false); }},
          MenuItem{text("Save Game", "Zapisz gre"), [this]() { saveGame(); }},
          MenuItem{text("Main Menu", "Menu glowne"), [this]() { returnToMainMenu(); }},
          MenuItem{text("Exit", "Wyjscie"), [this]() { shouldQuit_ = true; }},
      }});

  menuSystem_.registerMenu(MenuDefinition{
      "campaign_complete",
      text("Campaign Complete", "Kampania ukonczona"),
      {
          MenuItem{text("New Campaign", "Nowa kampania"), [this]() { startGame(true); }},
          MenuItem{text("Main Menu", "Menu glowne"), [this]() { returnToMainMenu(); }},
          MenuItem{text("Credits", "Autorzy"), [this]() { menuSystem_.pushMenu("credits"); }},
          MenuItem{text("Exit", "Wyjscie"), [this]() { shouldQuit_ = true; }},
      }});

  menuSystem_.registerMenu(MenuDefinition{
      "credits",
      text("Credits", "Autorzy"),
      {
          MenuItem{"Blade of Shadows - Davit Hunanyan", []() {}},
          MenuItem{text("Back", "Wstecz"), [this]() { menuSystem_.popMenu(); }},
      }});

  rebuildOptionsMenu();
}

void GameController::rebuildOptionsMenu()
{
  const auto toggleMusic = [this]()
  {
    settings_.musicEnabled = !settings_.musicEnabled;
    notifySettingsChanged();
  };
  const auto toggleSound = [this]()
  {
    settings_.soundEnabled = !settings_.soundEnabled;
    notifySettingsChanged();
  };
  const auto cycleMusicVolume = [this]()
  {
    settings_.musicVolume = settings_.musicVolume >= 100 ? 0 : std::min(100, settings_.musicVolume + 10);
    notifySettingsChanged();
  };
  const auto cycleSoundVolume = [this]()
  {
    settings_.soundVolume = settings_.soundVolume >= 100 ? 0 : std::min(100, settings_.soundVolume + 10);
    notifySettingsChanged();
  };
  const auto toggleLanguage = [this]()
  {
    settings_.language =
        settings_.language == GameLanguage::English ? GameLanguage::Polish : GameLanguage::English;
    notifySettingsChanged();
    buildMenus();
  };
  const auto previousTrack = [this]()
  {
    if (previousMusicHandler_)
    {
      previousMusicHandler_();
    }
  };
  const auto nextTrack = [this]()
  {
    if (nextMusicHandler_)
    {
      nextMusicHandler_();
    }
  };

  menuSystem_.registerMenu(MenuDefinition{
      "options",
      text("Settings", "Ustawienia"),
      {
          MenuItem{
              text("Music: ", "Muzyka: ") +
                  text(settings_.musicEnabled ? "On" : "Off", settings_.musicEnabled ? "Wl." : "Wyl."),
              toggleMusic},
          MenuItem{
              text("Now Playing: ", "Teraz gra: ") + currentMusicTrack_,
              []() {}},
          MenuItem{text("Previous Track", "Poprzedni utwor"), previousTrack},
          MenuItem{text("Next Track", "Nastepny utwor"), nextTrack},
          MenuItem{
              text("Sound: ", "Dzwiek: ") +
                  text(settings_.soundEnabled ? "On" : "Off", settings_.soundEnabled ? "Wl." : "Wyl."),
              toggleSound},
          MenuItem{
              text("Music Volume: ", "Glosnosc muzyki: ") + std::to_string(settings_.musicVolume) + "%",
              cycleMusicVolume},
          MenuItem{
              text("Sound Volume: ", "Glosnosc dzwieku: ") + std::to_string(settings_.soundVolume) + "%",
              cycleSoundVolume},
          MenuItem{
              text("Language: English", "Jezyk: Polski"),
              toggleLanguage},
          MenuItem{text("Back", "Wstecz"), [this]() { menuSystem_.popMenu(); }},
      }});
}

void GameController::startGame(bool newCampaign)
{
  if (!newGameHandler_ || !newGameHandler_(newCampaign))
  {
    return;
  }
  mode_ = GameMode::Playing;
  menuSystem_.close();
  input_.resetAll();
}

void GameController::startNextLevel()
{
  if (nextLevelHandler_ && nextLevelHandler_())
  {
    mode_ = GameMode::Playing;
    menuSystem_.close();
    input_.resetAll();
    return;
  }
  mode_ = GameMode::Menu;
  menuSystem_.openRoot("campaign_complete");
  input_.resetAll();
}

void GameController::startLevelEditor()
{
  if (!levelEditorHandler_ || !levelEditorHandler_())
  {
    return;
  }
  mode_ = GameMode::LevelEditor;
  menuSystem_.close();
  input_.resetAll();
}

void GameController::resumeFromPause()
{
  mode_ = modeBeforePause_;
  menuSystem_.close();
  input_.resetAll();
}

void GameController::saveGame()
{
  if (saveGameHandler_)
  {
    saveGameHandler_();
  }
}

void GameController::saveLevel()
{
  if (saveLevelHandler_)
  {
    saveLevelHandler_();
  }
}

void GameController::playEditedLevel()
{
  if (!playEditedLevelHandler_ || !playEditedLevelHandler_())
  {
    return;
  }
  mode_ = GameMode::Playing;
  menuSystem_.close();
  input_.resetAll();
}

void GameController::loadGame()
{
  if (loadGameHandler_ && loadGameHandler_())
  {
    mode_ = GameMode::Playing;
    menuSystem_.close();
    input_.resetAll();
  }
}

void GameController::returnToMainMenu()
{
  mode_ = GameMode::Menu;
  menuSystem_.openRoot("main");
  input_.resetAll();
}

void GameController::setSettings(const GameSettings& settings)
{
  settings_ = settings;
  buildMenus();
}

void GameController::setCurrentMusicTrack(std::string trackName)
{
  currentMusicTrack_ = std::move(trackName);
  rebuildOptionsMenu();
}

void GameController::notifySettingsChanged()
{
  rebuildOptionsMenu();
  if (settingsChangedHandler_)
  {
    settingsChangedHandler_(settings_);
  }
}

std::string GameController::text(const char* english, const char* polish) const
{
  return settings_.language == GameLanguage::Polish ? polish : english;
}
