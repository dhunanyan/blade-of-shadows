#ifndef GAME_APP_QT_GAME_SESSION_H
#define GAME_APP_QT_GAME_SESSION_H

#include <QString>
#include <QStringList>
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

  QString customLevelPath() const;
  QString saveGamePath() const { return saveRepository_.savePath(); }

private:
  bool loadResource(const QString& resourcePath);
  void installLoadedLevel();

  Engine& engine_;
  TileMapper level_;
  SaveGameRepository saveRepository_;
  QStringList campaignLevelIds_{
      QString::fromUtf8("level_01"),
      QString::fromUtf8("level_02")};
};

#endif // GAME_APP_QT_GAME_SESSION_H
