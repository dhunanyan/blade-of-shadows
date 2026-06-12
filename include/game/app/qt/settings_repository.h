#ifndef GAME_APP_QT_SETTINGS_REPOSITORY_H
#define GAME_APP_QT_SETTINGS_REPOSITORY_H

#include "game/app/game_settings.h"

class SettingsRepository
{
public:
  GameSettings load() const;
  void save(const GameSettings& settings) const;
};

#endif // GAME_APP_QT_SETTINGS_REPOSITORY_H
