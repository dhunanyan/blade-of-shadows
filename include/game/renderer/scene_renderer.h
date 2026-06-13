#ifndef GAME_RENDERER_SCENE_RENDERER_H
#define GAME_RENDERER_SCENE_RENDERER_H

#include <QPixmap>
#include <QPoint>
#include <QSize>
#include "game/app/game_settings.h"

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
      int playerScale,
      bool editorMode = false,
      GameLanguage language = GameLanguage::English) const;
  int menuItemAtPoint(const MenuView& menuView, const QSize& targetSize, const QPoint& point) const;
  float cameraOffsetX() const { return cameraX_; }
  void resetCamera()
  {
    cameraX_ = 0.0f;
    cameraInitialized_ = false;
  }

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
  void drawWorldItems(
      QPainter& painter,
      const Engine& engine,
      const AssetRepository& assets,
      int cameraOffsetXPx,
      int tileSizePx) const;
  void drawHud(
      QPainter& painter,
      const Engine& engine,
      const AssetRepository& assets,
      const QSize& targetSize,
      GameLanguage language) const;
  void drawEditorOverlay(
      QPainter& painter,
      const QSize& targetSize,
      int tileSizePx,
      GameLanguage language) const;
  void drawLifeBarAboveEnemy(QPainter& painter, const Enemy& enemy, int tileSizePx) const;
  void drawMenuOverlay(QPainter& painter, const QSize& targetSize, const MenuView& menuView) const;

private:
  static constexpr int viewportWidthCells_ = 48;
  mutable float cameraX_ = 0.0f;
  mutable bool cameraInitialized_ = false;
};

#endif // GAME_RENDERER_SCENE_RENDERER_H
