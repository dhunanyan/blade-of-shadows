#include "game/renderer/asset_repository.h"

bool AssetRepository::loadAll()
{
  backgroundLayer1_.load(QString::fromUtf8(":/background_layer_1.png"));
  backgroundLayer2_.load(QString::fromUtf8(":/background_layer_2.png"));
  backgroundLayer3_.load(QString::fromUtf8(":/background_layer_3.png"));
  enemyTexture_.load(QString::fromUtf8(":/enemy.png"));
  heartIcon_.load(QString::fromUtf8(":/ui/heart.png"));
  coinIcon_.load(QString::fromUtf8(":/ui/coin.png"));

  playerClips_[PlayerAnimationState::Damage] = loadFrameRange(QString::fromUtf8(":/player/damage"), 1, 8);
  playerClips_[PlayerAnimationState::Death] = loadFrameRange(QString::fromUtf8(":/player/death"), 1, 4);
  playerClips_[PlayerAnimationState::Dodge] = loadFrameRange(QString::fromUtf8(":/player/dodge"), 1, 3);
  playerClips_[PlayerAnimationState::DodgeMove] = loadFrameRange(QString::fromUtf8(":/player/dodge-move"), 1, 3);
  playerClips_[PlayerAnimationState::Fall] = loadFrameRange(QString::fromUtf8(":/player/fall"), 1, 8);
  playerClips_[PlayerAnimationState::Idle] = loadFrameRange(QString::fromUtf8(":/player/idle"), 1, 6);
  playerClips_[PlayerAnimationState::Jump] = loadFrameRange(QString::fromUtf8(":/player/jump"), 1, 8);
  playerClips_[PlayerAnimationState::Run] = loadFrameRange(QString::fromUtf8(":/player/run"), 1, 8);

  playerAttackVariantClips_[PlayerAnimationState::Dodge] =
      loadFrameRange(QString::fromUtf8(":/player/dodge-attack"), 1, 3);
  playerAttackVariantClips_[PlayerAnimationState::DodgeMove] =
      loadFrameRange(QString::fromUtf8(":/player/dodge-move-attack"), 1, 3);
  playerAttackVariantClips_[PlayerAnimationState::Fall] =
      loadFrameRange(QString::fromUtf8(":/player/fall-attack"), 1, 8);
  playerAttackVariantClips_[PlayerAnimationState::Idle] =
      loadFrameRange(QString::fromUtf8(":/player/idle-attack"), 1, 6);
  playerAttackVariantClips_[PlayerAnimationState::Jump] =
      loadFrameRange(QString::fromUtf8(":/player/jump-attack"), 1, 8);
  playerAttackVariantClips_[PlayerAnimationState::Run] =
      loadFrameRange(QString::fromUtf8(":/player/run-attack"), 1, 8);

  bool ok = !backgroundLayer1_.isNull() &&
            !backgroundLayer2_.isNull() &&
            !backgroundLayer3_.isNull() &&
            !enemyTexture_.isNull() &&
            !heartIcon_.isNull() &&
            !coinIcon_.isNull();

  for (const auto& [state, clip] : playerClips_)
  {
    (void)state;
    if (clip.empty())
    {
      ok = false;
      break;
    }
  }
  for (const auto& [state, clip] : playerAttackVariantClips_)
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
