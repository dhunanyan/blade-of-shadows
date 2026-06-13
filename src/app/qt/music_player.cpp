#include "game/app/qt/music_player.h"

#include <algorithm>
#include <array>
#include <numeric>
#include <QDebug>

namespace
{
constexpr std::array<const char*, 46> trackNames{
    "Moonlit Oath",
    "Ashen Footsteps",
    "Lanterns in the Fog",
    "Silent Katana",
    "Echoes Beneath Stone",
    "The Long Night March",
    "Shuriken Rain",
    "Crimson Canopy",
    "Hidden Path",
    "Shadowborne",
    "Temple of Broken Bells",
    "Frost on the Blade",
    "Dusk Runner",
    "Hollow Sanctum",
    "Ember Veil",
    "Whispering Steel",
    "Ronin's Resolve",
    "Cavern Pulse",
    "Blood Moon Crossing",
    "Bamboo After Dark",
    "Spectral Pursuit",
    "Shrine of Cinders",
    "Nocturne of the Lost",
    "Warden of Mist",
    "Black Feather Waltz",
    "Thunder Without Sky",
    "Veins of the Mountain",
    "Last Light at the Gate",
    "Phantom Province",
    "Blades in Autumn",
    "The Unseen Road",
    "Iron Lotus",
    "Nightfall Rebellion",
    "Beneath Violet Clouds",
    "Wolf at the Torii",
    "Siege of Silence",
    "Darkwater Reflection",
    "Oathkeeper's Descent",
    "Bells Before Battle",
    "Crown of Shadows",
    "A Thousand Quiet Steps",
    "The Final Lantern",
    "Kingdom Under Eclipse",
    "Dawn Through Smoke",
    "Home of the Wandering Blade",
    "Blade of Shadows"};
} // namespace

MusicPlayer::MusicPlayer(QObject* parent)
    : QObject(parent),
      randomEngine_(std::random_device{}())
{
  tracks_.reserve(trackNames.size());
  for (std::size_t index = 0; index < trackNames.size(); ++index)
  {
    tracks_.push_back(Track{
        QString::fromUtf8(trackNames[index]),
        QUrl(QString::fromUtf8("qrc:/audio/music/%1.m4a")
                 .arg(static_cast<int>(index + 1), 2, 10, QChar('0')))});
  }

  player_.setAudioOutput(&audioOutput_);
  connect(
      &player_,
      &QMediaPlayer::mediaStatusChanged,
      this,
      [this](QMediaPlayer::MediaStatus status)
      {
        if (status == QMediaPlayer::EndOfMedia)
        {
          next();
        }
      });
  connect(
      &player_,
      &QMediaPlayer::errorOccurred,
      this,
      [](QMediaPlayer::Error, const QString& message)
      {
        qWarning() << "Music playback error:" << message;
      });

  const int firstTrack = takeRandomTrack();
  if (firstTrack >= 0)
  {
    history_.push_back(firstTrack);
    historyPosition_ = 0;
    playTrack(firstTrack);
  }
}

void MusicPlayer::setEnabled(bool enabled)
{
  enabled_ = enabled;
  audioOutput_.setMuted(!enabled_);
  if (enabled_)
  {
    player_.play();
  }
  else
  {
    player_.stop();
  }
}

void MusicPlayer::setVolume(int volume)
{
  volume_ = std::clamp(volume, 0, 100);
  audioOutput_.setVolume(static_cast<float>(volume_) / 100.0f);
}

void MusicPlayer::next()
{
  if (historyPosition_ + 1 < static_cast<int>(history_.size()))
  {
    ++historyPosition_;
    playTrack(history_[static_cast<std::size_t>(historyPosition_)]);
    return;
  }

  const int nextTrack = takeRandomTrack();
  if (nextTrack < 0)
  {
    return;
  }

  history_.push_back(nextTrack);
  historyPosition_ = static_cast<int>(history_.size()) - 1;
  playTrack(nextTrack);
}

void MusicPlayer::previous()
{
  if (historyPosition_ <= 0)
  {
    const int previousTrack = takeRandomTrack();
    if (previousTrack < 0)
    {
      return;
    }
    history_.insert(history_.begin(), previousTrack);
    historyPosition_ = 0;
    playTrack(previousTrack);
    return;
  }

  --historyPosition_;
  playTrack(history_[static_cast<std::size_t>(historyPosition_)]);
}

QString MusicPlayer::currentTrackName() const
{
  if (currentTrackIndex_ < 0 || currentTrackIndex_ >= static_cast<int>(tracks_.size()))
  {
    return QString::fromUtf8("No Track");
  }
  return tracks_[static_cast<std::size_t>(currentTrackIndex_)].name;
}

void MusicPlayer::playTrack(int index)
{
  if (index < 0 || index >= static_cast<int>(tracks_.size()))
  {
    return;
  }

  currentTrackIndex_ = index;
  const Track& track = tracks_[static_cast<std::size_t>(index)];
  player_.setSource(track.source);
  if (enabled_)
  {
    player_.play();
  }
  qDebug() << "Now playing:" << track.name;
  emit currentTrackChanged(track.name, index);
}

void MusicPlayer::refillShuffleBag()
{
  shuffleBag_.resize(tracks_.size());
  std::iota(shuffleBag_.begin(), shuffleBag_.end(), 0);
  if (currentTrackIndex_ >= 0 && tracks_.size() > 1)
  {
    std::erase(shuffleBag_, currentTrackIndex_);
  }
  std::shuffle(shuffleBag_.begin(), shuffleBag_.end(), randomEngine_);
}

int MusicPlayer::takeRandomTrack()
{
  if (tracks_.empty())
  {
    return -1;
  }
  if (shuffleBag_.empty())
  {
    refillShuffleBag();
  }

  const int index = shuffleBag_.back();
  shuffleBag_.pop_back();
  return index;
}
