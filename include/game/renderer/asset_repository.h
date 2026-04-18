#ifndef GAME_RENDERER_ASSET_REPOSITORY_H
#define GAME_RENDERER_ASSET_REPOSITORY_H

#include <unordered_map>
#include <vector>
#include <QPixmap>
#include <QString>
#include "game/renderer/player_animation_state.h"

class AssetRepository
{
private:
  QPixmap backgroundLayer1_;
  QPixmap backgroundLayer2_;
  QPixmap backgroundLayer3_;
  QPixmap enemyTexture_;

  std::unordered_map<PlayerAnimationState, std::vector<QPixmap>> playerClips_;
  std::unordered_map<PlayerAnimationState, std::vector<QPixmap>> playerAttackVariantClips_;
  std::vector<QPixmap> emptyClip_;

  std::vector<QPixmap> loadFrames(const std::vector<QString>& resourcePaths) const;
  std::vector<QPixmap> loadFrameRange(const QString& baseDir, int from, int to) const;
  const std::vector<QPixmap>& emptyClip() const;

public:
  bool loadAll();
  const QPixmap& backgroundLayer1() const
  {
    return backgroundLayer1_;
  }
  const QPixmap& backgroundLayer2() const
  {
    return backgroundLayer2_;
  }
  const QPixmap& backgroundLayer3() const
  {
    return backgroundLayer3_;
  }
  const QPixmap& enemyTexture() const
  {
    return enemyTexture_;
  }
  const std::vector<QPixmap>& playerClip(PlayerAnimationState state) const
  {
    const auto it = playerClips_.find(state);
    if (it == playerClips_.end())
    {
      return emptyClip();
    }
    return it->second;
  }
  const std::vector<QPixmap>& playerAttackVariantClip(PlayerAnimationState state) const
  {
    const auto it = playerAttackVariantClips_.find(state);
    if (it == playerAttackVariantClips_.end())
    {
      return emptyClip();
    }
    return it->second;
  }
};

#endif // GAME_RENDERER_ASSET_REPOSITORY_H
