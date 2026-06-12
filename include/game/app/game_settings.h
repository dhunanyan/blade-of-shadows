#ifndef GAME_APP_GAME_SETTINGS_H
#define GAME_APP_GAME_SETTINGS_H

enum class GameLanguage
{
  English,
  Polish
};

struct GameSettings
{
  bool musicEnabled = true;
  bool soundEnabled = true;
  int musicVolume = 60;
  int soundVolume = 80;
  GameLanguage language = GameLanguage::English;
};

#endif // GAME_APP_GAME_SETTINGS_H
