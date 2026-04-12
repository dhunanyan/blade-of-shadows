#include "game/renderer/asset_repository.h"

bool AssetRepository::loadAll()
{
  backgroundLayer1_.load(QString::fromUtf8(":/background_layer_1.png"));
  backgroundLayer2_.load(QString::fromUtf8(":/background_layer_2.png"));
  backgroundLayer3_.load(QString::fromUtf8(":/background_layer_3.png"));
  enemyTexture_.load(QString::fromUtf8(":/enemy.png"));

  playerClips_[PlayerAnimationState::Attack] = loadFrameRange(QString::fromUtf8(":/player/attack"), 1, 6);
  playerClips_[PlayerAnimationState::Damage] = loadFrameRange(QString::fromUtf8(":/player/damage"), 1, 8);
  playerClips_[PlayerAnimationState::Death] = loadFrameRange(QString::fromUtf8(":/player/death"), 1, 4);
  playerClips_[PlayerAnimationState::Dodge] = loadFrameRange(QString::fromUtf8(":/player/dodge"), 1, 1);
  playerClips_[PlayerAnimationState::DodgeMove] = loadFrameRange(QString::fromUtf8(":/player/dodge-move"), 1, 3);
  playerClips_[PlayerAnimationState::Fall] = loadFrameRange(QString::fromUtf8(":/player/fall"), 1, 8);
  playerClips_[PlayerAnimationState::Idle] = loadFrameRange(QString::fromUtf8(":/player/idle"), 1, 6);
  playerClips_[PlayerAnimationState::Jump] = loadFrameRange(QString::fromUtf8(":/player/jump"), 1, 8);
  playerClips_[PlayerAnimationState::Run] = loadFrameRange(QString::fromUtf8(":/player/run"), 1, 8);

  bool ok = !backgroundLayer1_.isNull() &&
            !backgroundLayer2_.isNull() &&
            !backgroundLayer3_.isNull() &&
            !enemyTexture_.isNull();

  for (const auto& [state, clip] : playerClips_)
  {
    (void)state;
    if (clip.empty())
    {
      ok = false;
      break;
    }
  }

  return ok;
}

const QPixmap& AssetRepository::backgroundLayer1() const
{
  return backgroundLayer1_;
}

const QPixmap& AssetRepository::backgroundLayer2() const
{
  return backgroundLayer2_;
}

const QPixmap& AssetRepository::backgroundLayer3() const
{
  return backgroundLayer3_;
}

const QPixmap& AssetRepository::enemyTexture() const
{
  return enemyTexture_;
}

const std::vector<QPixmap>& AssetRepository::playerClip(PlayerAnimationState state) const
{
  auto it = playerClips_.find(state);
  if (it == playerClips_.end())
  {
    return emptyClip();
  }
  return it->second;
}

std::vector<QPixmap> AssetRepository::loadFrames(const std::vector<QString>& resourcePaths) const
{
  std::vector<QPixmap> frames;
  frames.reserve(resourcePaths.size());

  for (const auto& path : resourcePaths)
  {
    QPixmap frame(path);
    if (!frame.isNull())
    {
      frames.push_back(frame);
    }
  }

  return frames;
}

std::vector<QPixmap> AssetRepository::loadFrameRange(const QString& baseDir, int from, int to) const
{
  std::vector<QString> paths;
  if (from > to)
  {
    return {};
  }

  paths.reserve(static_cast<std::size_t>(to - from + 1));
  for (int index = from; index <= to; ++index)
  {
    paths.push_back(QString::fromUtf8("%1/%2.png").arg(baseDir).arg(index, 2, 10, QChar::fromLatin1('0')));
  }

  return loadFrames(paths);
}

const std::vector<QPixmap>& AssetRepository::emptyClip() const
{
  return emptyClip_;
}
