#ifndef GAME_APP_QT_GAME_SESSION_H
#define GAME_APP_QT_GAME_SESSION_H

#include <QString>
#include <QStringList>
#include <vector>
#include "game/app/level_selection.h"
#include "game/app/qt/save_game_repository.h"
#include "game/app/qt/tilemapper.h"
#include "game/core/engine.h"

class GameSession
{
public:
  explicit GameSession(Engine& engine);

  TileMapper& level() { return level_; }
  const TileMapper& level() const { return level_; }

  bool loadInitialLevel();
  bool startNewCampaign();
  bool restartLevel();
  bool loadNextLevel();
  bool loadLevelById(const QString& levelId);
  bool enterEditor();
  bool playEditedLevel();
  bool saveCustomLevel();
  bool saveGame();
  bool loadGame();
  bool loadSelectableLevel(const std::string& levelId);
  void recordCurrentLevelCompleted();
  std::vector<LevelSelectionEntry> levelSelections() const;

  QString customLevelPath() const;
  QString customLevelsDirectory() const;
  QString saveGamePath() const { return saveRepository_.savePath(); }

private:
  struct CampaignLevel
  {
    QString id;
    QString name;
  };

  bool loadResource(const QString& resourcePath);
  bool loadCustomLevel(const QString& filePath);
  int highestUnlockedCampaignIndex() const;
  void setHighestUnlockedCampaignIndex(int index) const;
  QString nextCustomLevelPath() const;
  void installLoadedLevel();

  Engine& engine_;
  TileMapper level_;
  SaveGameRepository saveRepository_;
  std::vector<CampaignLevel> campaignLevels_{
      {QString::fromUtf8("level_01"), QString::fromUtf8("Moonlit Approach")},
      {QString::fromUtf8("level_02"), QString::fromUtf8("Cavern of Echoes")},
      {QString::fromUtf8("level_03"), QString::fromUtf8("Lanternwood Crossing")},
      {QString::fromUtf8("level_04"), QString::fromUtf8("Ruins Beneath Rain")},
      {QString::fromUtf8("level_05"), QString::fromUtf8("The Ember Stair")},
      {QString::fromUtf8("level_06"), QString::fromUtf8("Moss-Crowned Ramparts")},
      {QString::fromUtf8("level_07"), QString::fromUtf8("Hollow Market")},
      {QString::fromUtf8("level_08"), QString::fromUtf8("Bridges of Blackwater")},
      {QString::fromUtf8("level_09"), QString::fromUtf8("The Broken Aqueduct")},
      {QString::fromUtf8("level_10"), QString::fromUtf8("Warden's Garden")},
      {QString::fromUtf8("level_11"), QString::fromUtf8("Ashen Bell District")},
      {QString::fromUtf8("level_12"), QString::fromUtf8("Thornwatch Passage")},
      {QString::fromUtf8("level_13"), QString::fromUtf8("The Sunken Archive")},
      {QString::fromUtf8("level_14"), QString::fromUtf8("Gloamstone Foundry")},
      {QString::fromUtf8("level_15"), QString::fromUtf8("Pilgrim's Last Road")},
      {QString::fromUtf8("level_16"), QString::fromUtf8("Citadel of Quiet Knives")},
      {QString::fromUtf8("level_17"), QString::fromUtf8("The Ironroot Depths")},
      {QString::fromUtf8("level_18"), QString::fromUtf8("Palace of Falling Stars")},
      {QString::fromUtf8("level_19"), QString::fromUtf8("Shadowgate Ascent")},
      {QString::fromUtf8("level_20"), QString::fromUtf8("The Last Moon Shrine")}};
  QString currentCustomLevelPath_;
};

#endif // GAME_APP_QT_GAME_SESSION_H
