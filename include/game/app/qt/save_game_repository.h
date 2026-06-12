#ifndef GAME_APP_QT_SAVE_GAME_REPOSITORY_H
#define GAME_APP_QT_SAVE_GAME_REPOSITORY_H

#include <optional>
#include <QString>
#include "game/core/engine.h"

struct SaveGameData
{
  QString levelId;
  Engine::Snapshot snapshot;
};

class SaveGameRepository
{
public:
  bool exists() const;
  bool save(const SaveGameData& data) const;
  std::optional<SaveGameData> load() const;
  QString savePath() const;
};

#endif // GAME_APP_QT_SAVE_GAME_REPOSITORY_H
