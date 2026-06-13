#ifndef GAME_APP_QT_MUSIC_PLAYER_H
#define GAME_APP_QT_MUSIC_PLAYER_H

#include <random>
#include <vector>
#include <QAudioOutput>
#include <QMediaPlayer>
#include <QObject>
#include <QString>
#include <QUrl>

class MusicPlayer : public QObject
{
  Q_OBJECT

public:
  explicit MusicPlayer(QObject* parent = nullptr);

  void setEnabled(bool enabled);
  void setVolume(int volume);
  void next();
  void previous();

  bool isEnabled() const { return enabled_; }
  int volume() const { return volume_; }
  int currentTrackIndex() const { return currentTrackIndex_; }
  QString currentTrackName() const;

signals:
  void currentTrackChanged(const QString& name, int index);

private:
  struct Track
  {
    QString name;
    QUrl source;
  };

  void playTrack(int index);
  void refillShuffleBag();
  int takeRandomTrack();

private:
  QMediaPlayer player_;
  QAudioOutput audioOutput_;
  std::vector<Track> tracks_;
  std::vector<int> shuffleBag_;
  std::vector<int> history_;
  std::mt19937 randomEngine_;
  int historyPosition_ = -1;
  int currentTrackIndex_ = -1;
  int volume_ = 60;
  bool enabled_ = false;
};

#endif // GAME_APP_QT_MUSIC_PLAYER_H
