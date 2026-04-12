#ifndef GAME_RENDERER_SCENE_RENDERER_H
#define GAME_RENDERER_SCENE_RENDERER_H

#include <QPixmap>
#include <QSize>

class QPainter;
class AssetRepository;
class TileMapper;
class Engine;
class PlayerPresentation;
class Enemy;

class SceneRenderer
{
public:
  QPixmap renderFrame(
      const Engine& engine,
      const TileMapper& tileMapper,
      const AssetRepository& assets,
      const PlayerPresentation& playerPresentation,
      int tileSizePx,
      int playerScale) const;

private:
  void drawBackgroundLayers(QPainter& painter, const AssetRepository& assets, const QSize& targetSize) const;
  void drawPlayer(
      QPainter& painter,
      const Engine& engine,
      const PlayerPresentation& playerPresentation,
      const AssetRepository& assets,
      int tileSizePx,
      int playerScale) const;
  void drawEnemies(QPainter& painter, const Engine& engine, const AssetRepository& assets, int tileSizePx) const;
  void drawLifeBarAboveEnemy(QPainter& painter, const Enemy& enemy, int tileSizePx) const;
};

#endif // GAME_RENDERER_SCENE_RENDERER_H
