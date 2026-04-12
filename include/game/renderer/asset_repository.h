#ifndef ASSET_REPOSITORY_H
#define ASSET_REPOSITORY_H

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
  std::vector<QPixmap> emptyClip_;

  std::vector<QPixmap> loadFrames(const std::vector<QString>& resourcePaths) const;
  std::vector<QPixmap> loadFrameRange(const QString& baseDir, int from, int to) const;
  const std::vector<QPixmap>& emptyClip() const;

public:
  bool loadAll();
  const QPixmap& backgroundLayer1() const;
  const QPixmap& backgroundLayer2() const;
  const QPixmap& backgroundLayer3() const;
  const QPixmap& enemyTexture() const;
  const std::vector<QPixmap>& playerClip(PlayerAnimationState state) const;
};

#endif // ASSET_REPOSITORY_H
