#ifndef GAME_RENDERER_SCENE_RENDERER_H
#define GAME_RENDERER_SCENE_RENDERER_H

#include <QPixmap>
#include <QPoint>
#include <QSize>

class QPainter;
class AssetRepository;
class TileMapper;
class Engine;
class PlayerPresentation;
class Enemy;
struct MenuView;

class SceneRenderer
{
public:
  QPixmap renderFrame(
      const Engine& engine,
      const TileMapper& tileMapper,
      const AssetRepository& assets,
      const PlayerPresentation& playerPresentation,
      const MenuView& menuView,
      int tileSizePx,
      int playerScale) const;
  int menuItemAtPoint(const MenuView& menuView, const QSize& targetSize, const QPoint& point) const;

private:
  float updateCameraX(const Engine& engine, const QSize& viewportSize, float maxCameraX) const;
  void drawBackgroundLayers(QPainter& painter, const AssetRepository& assets, const QSize& targetSize) const;
  void drawPlayer(
      QPainter& painter,
      const Engine& engine,
      const PlayerPresentation& playerPresentation,
      const AssetRepository& assets,
      int cameraOffsetXPx,
      int tileSizePx,
      int playerScale) const;
  void drawDoubleJumpFx(
      QPainter& painter,
      const Engine& engine,
      int cameraOffsetXPx,
      int tileSizePx,
      int playerScale) const;
  void drawEnemies(
      QPainter& painter,
      const Engine& engine,
      const AssetRepository& assets,
      int cameraOffsetXPx,
      int tileSizePx) const;
  void drawLifeBarAboveEnemy(QPainter& painter, const Enemy& enemy, int tileSizePx) const;
  void drawMenuOverlay(QPainter& painter, const QSize& targetSize, const MenuView& menuView) const;

private:
  static constexpr int viewportWidthCells_ = 48;
  mutable float cameraX_ = 0.0f;
  mutable bool cameraInitialized_ = false;
};

#endif // GAME_RENDERER_SCENE_RENDERER_H
