#ifndef GAME_APP_GAME_CONTROLLER_H
#define GAME_APP_GAME_CONTROLLER_H

#include <cstddef>
#include <functional>
#include <string>
#include <vector>
#include "game/app/level_selection.h"
#include "game/app/input_state.h"
#include "game/app/menu_system.h"
#include "game/app/game_settings.h"
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
  const GameSettings& settings() const { return settings_; }
  void setSettings(const GameSettings& settings);
  void setNewGameHandler(std::function<bool(bool)> handler) { newGameHandler_ = std::move(handler); }
  void setNextLevelHandler(std::function<bool()> handler) { nextLevelHandler_ = std::move(handler); }
  void setSaveGameHandler(std::function<bool()> handler) { saveGameHandler_ = std::move(handler); }
  void setLoadGameHandler(std::function<bool()> handler) { loadGameHandler_ = std::move(handler); }
  void setLevelEditorHandler(std::function<bool()> handler) { levelEditorHandler_ = std::move(handler); }
  void setSaveLevelHandler(std::function<bool()> handler) { saveLevelHandler_ = std::move(handler); }
  void setPlayEditedLevelHandler(std::function<bool()> handler) { playEditedLevelHandler_ = std::move(handler); }
  void setLevelSelectionHandlers(
      std::function<std::vector<LevelSelectionEntry>()> catalogHandler,
      std::function<bool(const std::string&)> loadHandler)
  {
    levelCatalogHandler_ = std::move(catalogHandler);
    loadSelectedLevelHandler_ = std::move(loadHandler);
  }
  void setSettingsChangedHandler(std::function<void(const GameSettings&)> handler)
  {
    settingsChangedHandler_ = std::move(handler);
  }
  void setMusicNavigationHandlers(
      std::function<void()> previousHandler,
      std::function<void()> nextHandler)
  {
    previousMusicHandler_ = std::move(previousHandler);
    nextMusicHandler_ = std::move(nextHandler);
  }
  void setCurrentMusicTrack(std::string trackName);

  void onKeyEvent(int key, bool isPressed, bool isAutoRepeat);
  void onMenuHover(int hoveredIndex);
  void onMenuClick(int clickedIndex);
  void onMenuScroll(int delta);
  void applyInput();
  void tick();

private:
  void buildMenus();
  void startGame(bool newCampaign = false);
  void startNextLevel();
  void startLevelEditor();
  void openLevelSelection();
  void loadSelectedLevel(const std::string& levelId);
  void resumeFromPause();
  void saveGame();
  void saveLevel();
  void playEditedLevel();
  void loadGame();
  void returnToMainMenu();
  void rebuildOptionsMenu();
  void notifySettingsChanged();
  std::string text(const char* english, const char* polish) const;

private:
  Engine engine_;
  InputState input_;
  MenuSystem menuSystem_;
  GameMode mode_ = GameMode::Menu;
  GameMode modeBeforePause_ = GameMode::Playing;
  bool shouldQuit_ = false;
  GameSettings settings_;
  std::function<bool(bool)> newGameHandler_;
  std::function<bool()> nextLevelHandler_;
  std::function<bool()> saveGameHandler_;
  std::function<bool()> loadGameHandler_;
  std::function<bool()> levelEditorHandler_;
  std::function<bool()> saveLevelHandler_;
  std::function<bool()> playEditedLevelHandler_;
  std::function<std::vector<LevelSelectionEntry>()> levelCatalogHandler_;
  std::function<bool(const std::string&)> loadSelectedLevelHandler_;
  std::function<void(const GameSettings&)> settingsChangedHandler_;
  std::function<void()> previousMusicHandler_;
  std::function<void()> nextMusicHandler_;
  std::string currentMusicTrack_ = "No Track";
};

#endif // GAME_APP_GAME_CONTROLLER_H
