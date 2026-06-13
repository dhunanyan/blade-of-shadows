#include "game/app/qt/game_session.h"

#include <algorithm>
#include <optional>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QStandardPaths>

namespace
{
struct CustomLevelMetadata
{
  QString id;
  QString name;
};

std::optional<CustomLevelMetadata> readCustomLevelMetadata(const QString& path)
{
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    return std::nullopt;
  }
  const QJsonDocument document = QJsonDocument::fromJson(file.readAll());
  if (!document.isObject())
  {
    return std::nullopt;
  }
  const QJsonObject root = document.object();
  const QString id = root.value(QString::fromUtf8("id")).toString();
  const QString name = root.value(QString::fromUtf8("displayName"))
                           .toString(root.value(QString::fromUtf8("name")).toString(id));
  if (id.isEmpty() || name.isEmpty())
  {
    return std::nullopt;
  }
  return CustomLevelMetadata{id, name};
}
} // namespace

GameSession::GameSession(Engine& engine)
    : engine_(engine)
{
}

bool GameSession::loadInitialLevel()
{
  return loadLevelById(campaignLevels_.front().id);
}

bool GameSession::startNewCampaign()
{
  return loadLevelById(campaignLevels_.front().id);
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
  int currentIndex = -1;
  for (int index = 0; index < static_cast<int>(campaignLevels_.size()); ++index)
  {
    if (campaignLevels_[static_cast<std::size_t>(index)].id == level_.levelId())
    {
      currentIndex = index;
      break;
    }
  }
  if (currentIndex < 0 ||
      currentIndex + 1 >= static_cast<int>(campaignLevels_.size()))
  {
    return false;
  }
  setHighestUnlockedCampaignIndex(currentIndex + 1);
  return loadLevelById(
      campaignLevels_[static_cast<std::size_t>(currentIndex + 1)].id);
}

bool GameSession::loadLevelById(const QString& levelId)
{
  for (const CampaignLevel& campaignLevel : campaignLevels_)
  {
    if (campaignLevel.id == levelId)
    {
      return loadResource(
          QString::fromUtf8(":/levels/%1.json").arg(campaignLevel.id));
    }
  }

  const QDir directory(customLevelsDirectory());
  for (const QFileInfo& file :
       directory.entryInfoList(
           {QString::fromUtf8("*.json")},
           QDir::Files,
           QDir::Name))
  {
    const auto metadata = readCustomLevelMetadata(file.absoluteFilePath());
    if (metadata && metadata->id == levelId)
    {
      return loadCustomLevel(file.absoluteFilePath());
    }
  }
  return false;
}

bool GameSession::enterEditor()
{
  return level_.isLoaded();
}

bool GameSession::playEditedLevel()
{
  return restartLevel();
}

bool GameSession::saveCustomLevel()
{
  QString path = currentCustomLevelPath_;
  if (path.isEmpty())
  {
    path = nextCustomLevelPath();
    const QString baseName = QFileInfo(path).completeBaseName();
    const QString suffix = baseName.mid(QString::fromUtf8("custom_level_").size());
    level_.setLevelIdentity(
        baseName,
        QString::fromUtf8("Custom Level %1").arg(suffix.toInt()));
  }

  if (!level_.saveToJsonFile(path))
  {
    return false;
  }
  currentCustomLevelPath_ = path;
  installLoadedLevel();
  return true;
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
  for (int index = 0; index < static_cast<int>(campaignLevels_.size()); ++index)
  {
    if (campaignLevels_[static_cast<std::size_t>(index)].id == saved->levelId)
    {
      setHighestUnlockedCampaignIndex(index);
      break;
    }
  }
  engine_.restoreSnapshot(saved->snapshot);
  return true;
}

bool GameSession::loadSelectableLevel(const std::string& levelId)
{
  const QString requested = QString::fromStdString(levelId);
  for (const LevelSelectionEntry& entry : levelSelections())
  {
    if (entry.id == levelId)
    {
      return entry.unlocked && loadLevelById(requested);
    }
  }
  return false;
}

void GameSession::recordCurrentLevelCompleted()
{
  for (int index = 0; index < static_cast<int>(campaignLevels_.size()); ++index)
  {
    if (campaignLevels_[static_cast<std::size_t>(index)].id == level_.levelId())
    {
      setHighestUnlockedCampaignIndex(
          std::min(index + 1, static_cast<int>(campaignLevels_.size()) - 1));
      return;
    }
  }
}

std::vector<LevelSelectionEntry> GameSession::levelSelections() const
{
  std::vector<LevelSelectionEntry> result;
  const int highestUnlocked = highestUnlockedCampaignIndex();
  result.reserve(campaignLevels_.size());
  for (int index = 0; index < static_cast<int>(campaignLevels_.size()); ++index)
  {
    const CampaignLevel& level =
        campaignLevels_[static_cast<std::size_t>(index)];
    result.push_back(LevelSelectionEntry{
        level.id.toStdString(),
        QString::fromUtf8("Level %1 - %2")
            .arg(index + 1)
            .arg(level.name)
            .toStdString(),
        index <= highestUnlocked,
        false});
  }

  const QDir directory(customLevelsDirectory());
  for (const QFileInfo& file :
       directory.entryInfoList(
           {QString::fromUtf8("*.json")},
           QDir::Files,
           QDir::Name))
  {
    const auto metadata = readCustomLevelMetadata(file.absoluteFilePath());
    if (!metadata)
    {
      continue;
    }
    result.push_back(LevelSelectionEntry{
        metadata->id.toStdString(),
        QString::fromUtf8("Custom - %1").arg(metadata->name).toStdString(),
        true,
        true});
  }
  return result;
}

QString GameSession::customLevelPath() const
{
  return currentCustomLevelPath_.isEmpty()
             ? nextCustomLevelPath()
             : currentCustomLevelPath_;
}

QString GameSession::customLevelsDirectory() const
{
  const QString root =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  return QDir(root).filePath(QString::fromUtf8("levels"));
}

bool GameSession::loadResource(const QString& resourcePath)
{
  if (!level_.loadFromJsonResource(resourcePath))
  {
    return false;
  }
  currentCustomLevelPath_.clear();
  installLoadedLevel();
  return true;
}

bool GameSession::loadCustomLevel(const QString& filePath)
{
  if (!level_.loadFromJsonFile(filePath))
  {
    return false;
  }
  currentCustomLevelPath_ = filePath;
  installLoadedLevel();
  return true;
}

int GameSession::highestUnlockedCampaignIndex() const
{
  QSettings settings;
  return std::clamp(
      settings.value(QString::fromUtf8("progress/highestUnlockedLevel"), 0).toInt(),
      0,
      std::max(0, static_cast<int>(campaignLevels_.size()) - 1));
}

void GameSession::setHighestUnlockedCampaignIndex(int index) const
{
  QSettings settings;
  settings.setValue(
      QString::fromUtf8("progress/highestUnlockedLevel"),
      std::max(highestUnlockedCampaignIndex(), index));
  settings.sync();
}

QString GameSession::nextCustomLevelPath() const
{
  const QDir directory(customLevelsDirectory());
  for (int index = 1; index < 10000; ++index)
  {
    const QString path = directory.filePath(
        QString::fromUtf8("custom_level_%1.json")
            .arg(index, 3, 10, QLatin1Char('0')));
    if (!QFile::exists(path))
    {
      return path;
    }
  }
  return directory.filePath(QString::fromUtf8("custom_level.json"));
}

void GameSession::installLoadedLevel()
{
  engine_.loadLevel(level_.levelDefinition());
}
