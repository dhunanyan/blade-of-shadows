#include "game/app/qt/game_session.h"

#include <QDir>
#include <QFile>
#include <QStandardPaths>

GameSession::GameSession(Engine& engine)
    : engine_(engine)
{
}

bool GameSession::loadInitialLevel()
{
  return loadLevelById(campaignLevelIds_.front());
}

bool GameSession::startNewCampaign()
{
  return loadLevelById(campaignLevelIds_.front());
}

bool GameSession::restartLevel()
{
  if (!level_.isLoaded())
  {
    return false;
  }
  installLoadedLevel();
  return true;
}

bool GameSession::loadNextLevel()
{
  const int currentIndex = campaignLevelIds_.indexOf(level_.levelId());
  if (currentIndex < 0 || currentIndex + 1 >= campaignLevelIds_.size())
  {
    return false;
  }
  return loadLevelById(campaignLevelIds_.at(currentIndex + 1));
}

bool GameSession::loadLevelById(const QString& levelId)
{
  if (levelId == QString::fromUtf8("custom_level"))
  {
    if (!level_.loadFromJsonFile(customLevelPath()))
    {
      return false;
    }
    installLoadedLevel();
    return true;
  }
  return loadResource(QString::fromUtf8(":/levels/%1.json").arg(levelId));
}

bool GameSession::enterEditor()
{
  if (QFile::exists(customLevelPath()) &&
      level_.loadFromJsonFile(customLevelPath()))
  {
    installLoadedLevel();
    return true;
  }
  return level_.isLoaded();
}

bool GameSession::playEditedLevel()
{
  return restartLevel();
}

bool GameSession::saveCustomLevel()
{
  return level_.saveToJsonFile(customLevelPath());
}

bool GameSession::saveGame()
{
  if (!level_.isLoaded())
  {
    return false;
  }
  return saveRepository_.save(SaveGameData{
      level_.levelId(),
      engine_.snapshot()});
}

bool GameSession::loadGame()
{
  const auto saved = saveRepository_.load();
  if (!saved || !loadLevelById(saved->levelId))
  {
    return false;
  }
  engine_.restoreSnapshot(saved->snapshot);
  return true;
}

QString GameSession::customLevelPath() const
{
  const QString root =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  return QDir(root).filePath(QString::fromUtf8("levels/custom_level.json"));
}

bool GameSession::loadResource(const QString& resourcePath)
{
  if (!level_.loadFromJsonResource(resourcePath))
  {
    return false;
  }
  installLoadedLevel();
  return true;
}

void GameSession::installLoadedLevel()
{
  engine_.loadLevel(level_.levelDefinition());
}
