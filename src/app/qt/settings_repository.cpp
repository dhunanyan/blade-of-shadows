#include "game/app/qt/settings_repository.h"

#include <algorithm>
#include <QSettings>

GameSettings SettingsRepository::load() const
{
  QSettings storage;
  GameSettings settings;
  settings.musicEnabled = storage.value(QString::fromUtf8("audio/musicEnabled"), true).toBool();
  settings.soundEnabled = storage.value(QString::fromUtf8("audio/soundEnabled"), true).toBool();
  settings.musicVolume = std::clamp(storage.value(QString::fromUtf8("audio/musicVolume"), 60).toInt(), 0, 100);
  settings.soundVolume = std::clamp(storage.value(QString::fromUtf8("audio/soundVolume"), 80).toInt(), 0, 100);
  settings.language =
      storage.value(QString::fromUtf8("general/language"), QString::fromUtf8("en")).toString() ==
              QString::fromUtf8("pl")
          ? GameLanguage::Polish
          : GameLanguage::English;
  return settings;
}

void SettingsRepository::save(const GameSettings& settings) const
{
  QSettings storage;
  storage.setValue(QString::fromUtf8("audio/musicEnabled"), settings.musicEnabled);
  storage.setValue(QString::fromUtf8("audio/soundEnabled"), settings.soundEnabled);
  storage.setValue(QString::fromUtf8("audio/musicVolume"), std::clamp(settings.musicVolume, 0, 100));
  storage.setValue(QString::fromUtf8("audio/soundVolume"), std::clamp(settings.soundVolume, 0, 100));
  storage.setValue(
      QString::fromUtf8("general/language"),
      settings.language == GameLanguage::Polish ? QString::fromUtf8("pl") : QString::fromUtf8("en"));
  storage.sync();
}
