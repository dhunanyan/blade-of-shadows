#include "game/renderer/scene_renderer.h"
#include <algorithm>
#include <cmath>
#include <QFont>
#include <QColor>
#include <QPainter>
#include <QPen>
#include <QTransform>
#include "game/app/menu_system.h"
#include "game/app/qt/tilemapper.h"
#include "game/core/enemy.h"
#include "game/core/engine.h"
#include "game/renderer/asset_repository.h"
#include "game/renderer/enemy_presentation.h"
#include "game/renderer/player_presentation.h"

QPixmap SceneRenderer::renderFrame(
    const Engine& engine,
    const TileMapper& tileMapper,
    const AssetRepository& assets,
    const PlayerPresentation& playerPresentation,
    const MenuView& menuView,
    int tileSizePx,
    int playerScale,
    bool editorMode,
    GameLanguage language) const
{
  const int stageWidthPx = static_cast<int>(engine.stageWidthCells()) * tileSizePx;
  const int stageHeightPx = static_cast<int>(engine.stageHeightCells()) * tileSizePx;
  const int viewportWidthPx = std::min(stageWidthPx, viewportWidthCells_ * tileSizePx);
  const int viewportHeightPx = std::min(stageHeightPx, viewportHeightCells_ * tileSizePx);
  const int contentWidthPx = std::max(viewportWidthPx, (tileMapper.maxOccupiedColumn() + 1) * tileSizePx);
  const float maxCameraX = std::max(0.0f, static_cast<float>(contentWidthPx - viewportWidthPx));
  const float maxCameraY =
      std::max(0.0f, static_cast<float>(stageHeightPx - viewportHeightPx));
  const QSize targetSize(viewportWidthPx, viewportHeightPx);

  QPixmap composed = assets.backgroundLayer1().scaled(
      targetSize,
      Qt::IgnoreAspectRatio,
      Qt::SmoothTransformation);

  QPainter painter(&composed);
  drawBackgroundLayers(painter, assets, targetSize);

  if (!engine.isPlayerAlive())
  {
    painter.drawRect(composed.rect());
  }

  const float cameraX = editorMode
                            ? std::clamp(editorCameraX_, 0.0f, maxCameraX)
                            : updateCameraX(engine, targetSize, maxCameraX);
  const float cameraY = editorMode
                            ? std::clamp(editorCameraY_, 0.0f, maxCameraY)
                            : 0.0f;
  if (editorMode)
  {
    editorCameraX_ = cameraX;
    editorCameraY_ = cameraY;
  }
  lastCameraX_ = cameraX;
  lastCameraY_ = cameraY;
  const int cameraOffsetXPx = static_cast<int>(std::lround(cameraX));
  const int cameraOffsetYPx = static_cast<int>(std::lround(cameraY));

  painter.save();
  painter.translate(0, -cameraOffsetYPx);
  tileMapper.render(painter, tileSizePx, cameraOffsetXPx);
  if (editorMode)
  {
    tileMapper.renderEditorMarkers(painter, tileSizePx, cameraOffsetXPx);
  }
  if (!editorMode)
  {
    drawWorldItems(painter, engine, assets, cameraOffsetXPx, tileSizePx);
    drawPlayer(painter, engine, playerPresentation, assets, cameraOffsetXPx, tileSizePx, playerScale);
    drawDoubleJumpFx(painter, engine, cameraOffsetXPx, tileSizePx, playerScale);
    drawEnemies(painter, engine, assets, cameraOffsetXPx, tileSizePx);
  }
  painter.restore();

  if (!editorMode)
  {
    drawHud(painter, engine, assets, targetSize, language);
  }
  if (editorMode)
  {
    drawEditorOverlay(
        painter,
        targetSize,
        tileSizePx,
        cameraOffsetXPx,
        cameraOffsetYPx,
        language);
  }
  drawMenuOverlay(painter, targetSize, menuView);

  return composed;
}

float SceneRenderer::updateCameraX(const Engine& engine, const QSize& viewportSize, float maxCameraX) const
{
  const float viewportWidthPx = static_cast<float>(viewportSize.width());

  if (maxCameraX <= 0.0f)
  {
    cameraX_ = 0.0f;
    cameraInitialized_ = true;
    return cameraX_;
  }

  const float leftTrigger = viewportWidthPx * 0.25f;

  if (!cameraInitialized_)
  {
    // Start with player in the left part of the screen (around chunk1/chunk2 split).
    cameraX_ = std::clamp(
        engine.playerBounds().origin.x +
            engine.playerBounds().size.width * 0.5f -
            leftTrigger,
        0.0f,
        maxCameraX);
    cameraInitialized_ = true;
  }

  const float playerScreenX =
      engine.playerBounds().origin.x +
      engine.playerBounds().size.width * 0.5f -
      cameraX_;
  const int moveIntentX = engine.playerMoveIntentX();

  if (moveIntentX < 0 && playerScreenX < leftTrigger)
  {
    cameraX_ -= (leftTrigger - playerScreenX);
  }
  else if (moveIntentX > 0 && playerScreenX > leftTrigger)
  {
    cameraX_ += (playerScreenX - leftTrigger);
  }

  cameraX_ = std::clamp(cameraX_, 0.0f, maxCameraX);
  return cameraX_;
}

int SceneRenderer::menuItemAtPoint(const MenuView& menuView, const QSize& targetSize, const QPoint& point) const
{
  if (!menuView.visible)
  {
    return -1;
  }

  const int panelWidth = targetSize.width() * 2 / 5;
  const int panelHeight = std::min(
      targetSize.height() - 40,
      std::max(
          targetSize.height() * 3 / 5,
          110 + static_cast<int>(menuView.items.size()) * 40));
  const int panelX = (targetSize.width() - panelWidth) / 2;
  const int panelY = (targetSize.height() - panelHeight) / 2;

  const int itemStartY = panelY + 80;
  const int itemStep = 40;
  for (int index = 0; index < static_cast<int>(menuView.items.size()); ++index)
  {
    const QRect itemRect(panelX + 24, itemStartY + index * itemStep, panelWidth - 48, 30);
    if (itemRect.adjusted(-8, -2, 8, 2).contains(point))
    {
      return menuView.firstVisibleIndex + index;
    }
  }

  return -1;
}

void SceneRenderer::drawBackgroundLayers(QPainter& painter, const AssetRepository& assets, const QSize& targetSize) const
{
  const QPixmap layer2 = assets.backgroundLayer2().scaled(
      targetSize,
      Qt::IgnoreAspectRatio,
      Qt::SmoothTransformation);
  const QPixmap layer3 = assets.backgroundLayer3().scaled(
      targetSize,
      Qt::IgnoreAspectRatio,
      Qt::SmoothTransformation);

  painter.drawPixmap(0, 0, layer2);
  painter.drawPixmap(0, 0, layer3);
}

void SceneRenderer::drawPlayer(
    QPainter& painter,
    const Engine& engine,
    const PlayerPresentation& playerPresentation,
    const AssetRepository& assets,
    int cameraOffsetXPx,
    int tileSizePx,
    int playerScale) const
{
  if (!engine.isPlayerAlive())
  {
    return;
  }

  QPixmap frame = playerPresentation.currentFrame(assets);
  if (frame.isNull())
  {
    return;
  }

  const bool facingLeft = playerPresentation.isFacingLeft(engine.playerDirection());
  if (facingLeft)
  {
    frame = frame.transformed(QTransform().scale(-1, 1));
  }

  const WorldRect body = engine.playerBounds();
  const QSize targetSize(tileSizePx * playerScale, tileSizePx * playerScale);
  const int centerX =
      static_cast<int>(std::lround(body.origin.x + body.size.width * 0.5f)) -
      cameraOffsetXPx;
  const int feetY = static_cast<int>(std::lround(body.bottom()));
  const int drawX = centerX - targetSize.width() / 2;
  const int drawY = feetY - targetSize.height();
  const QRect targetRect(QPoint(drawX, drawY), targetSize);
  painter.drawPixmap(targetRect, frame);
}

void SceneRenderer::drawEnemies(
    QPainter& painter,
    const Engine& engine,
    const AssetRepository& assets,
    int cameraOffsetXPx,
    int tileSizePx) const
{
  for (const auto& enemy : engine.enemies())
  {
    const auto [topLeft, bottomRight] = EnemyPresentation::positionToRectPoints(enemy->position(), tileSizePx);
    const QRect enemyRect(
        topLeft.x() - cameraOffsetXPx,
        topLeft.y(),
        bottomRight.x() - topLeft.x() + 1,
        bottomRight.y() - topLeft.y() + 1);
    painter.drawPixmap(enemyRect, assets.enemyTexture());
    painter.save();
    painter.translate(-cameraOffsetXPx, 0);
    drawLifeBarAboveEnemy(painter, *enemy, tileSizePx);
    painter.restore();
  }
}

void SceneRenderer::drawWorldItems(
    QPainter& painter,
    const Engine& engine,
    const AssetRepository& assets,
    int cameraOffsetXPx,
    int tileSizePx) const
{
  painter.save();
  painter.setRenderHint(QPainter::Antialiasing, false);

  for (const Position& coin : engine.coins())
  {
    const int centerX = static_cast<int>(coin.x()) * tileSizePx - cameraOffsetXPx + tileSizePx / 2;
    const int centerY = static_cast<int>(coin.y()) * tileSizePx + tileSizePx / 2;
    const QRect coinRect(centerX - 10, centerY - 10, 20, 20);
    painter.drawPixmap(coinRect, assets.coinIcon());
  }

  if (engine.levelExit())
  {
    const Position exit = *engine.levelExit();
    const int x = static_cast<int>(exit.x()) * tileSizePx - cameraOffsetXPx;
    const int y = static_cast<int>(exit.y()) * tileSizePx;
    const QRect portalRect(x + 3, y - tileSizePx, tileSizePx - 6, tileSizePx * 2);
    painter.setPen(QPen(engine.coins().empty() ? QColor(120, 245, 255) : QColor(120, 120, 130), 3));
    painter.setBrush(engine.coins().empty() ? QColor(35, 160, 210, 120) : QColor(30, 30, 40, 150));
    painter.drawRoundedRect(portalRect, 8, 8);
  }

  painter.restore();
}

void SceneRenderer::drawHud(
    QPainter& painter,
    const Engine& engine,
    const AssetRepository& assets,
    const QSize& targetSize,
    GameLanguage language) const
{
  painter.save();
  painter.setRenderHint(QPainter::Antialiasing, true);

  const QRect panel(14, 12, 260, 62);
  painter.setPen(QPen(QColor(225, 225, 225, 180), 1));
  painter.setBrush(QColor(8, 12, 20, 190));
  painter.drawRoundedRect(panel, 8, 8);

  for (int index = 0; index < engine.playerMaxHealth(); ++index)
  {
    const int x = 28 + index * 25;
    const int y = 27;
    if (index < engine.playerHealth())
    {
      painter.drawPixmap(QRect(x, y, 20, 20), assets.heartIcon());
    }
    else
    {
      painter.setOpacity(0.25);
      painter.drawPixmap(QRect(x, y, 20, 20), assets.heartIcon());
      painter.setOpacity(1.0);
    }
  }

  QFont hudFont = painter.font();
  hudFont.setPointSize(11);
  hudFont.setBold(true);
  painter.setFont(hudFont);
  painter.setPen(QColor(255, 224, 95));
  painter.drawText(QRect(28, 50, 110, 18), Qt::AlignLeft | Qt::AlignVCenter,
                   QString::fromUtf8(language == GameLanguage::Polish ? "Monety: %1" : "Coins: %1")
                       .arg(engine.playerCoins()));
  painter.setPen(Qt::white);
  painter.drawText(QRect(140, 50, 120, 18), Qt::AlignLeft | Qt::AlignVCenter,
                   QString::fromUtf8(language == GameLanguage::Polish ? "Wynik: %1" : "Score: %1")
                       .arg(engine.playerScore()));

  if (!engine.coins().empty())
  {
    painter.setPen(QColor(220, 225, 235));
    painter.drawText(
        QRect(targetSize.width() - 280, 18, 260, 24),
        Qt::AlignRight | Qt::AlignVCenter,
        QString::fromUtf8(
            language == GameLanguage::Polish
                ? "Zbierz wszystkie monety, aby otworzyc brame"
                : "Collect all coins to unlock the gate"));
  }

  painter.restore();
}

void SceneRenderer::drawEditorOverlay(
    QPainter& painter,
    const QSize& targetSize,
    int tileSizePx,
    int cameraOffsetXPx,
    int cameraOffsetYPx,
    GameLanguage language) const
{
  painter.save();
  painter.setPen(QPen(QColor(255, 255, 255, 35), 1));
  const int gridStartX = -(cameraOffsetXPx % tileSizePx);
  const int gridStartY = -(cameraOffsetYPx % tileSizePx);
  for (int x = gridStartX; x <= targetSize.width(); x += tileSizePx)
  {
    painter.drawLine(x, 0, x, targetSize.height());
  }
  for (int y = gridStartY; y <= targetSize.height(); y += tileSizePx)
  {
    painter.drawLine(0, y, targetSize.width(), y);
  }

  const QRect helpRect(14, targetSize.height() - 62, targetSize.width() - 28, 46);
  painter.setPen(QPen(QColor(220, 230, 245), 1));
  painter.setBrush(QColor(8, 12, 20, 210));
  painter.drawRoundedRect(helpRect, 6, 6);
  painter.setPen(Qt::white);
  painter.drawText(
      helpRect.adjusted(12, 4, -12, -4),
      Qt::AlignLeft | Qt::AlignVCenter,
      QString::fromUtf8(
          language == GameLanguage::Polish
              ? "EDYTOR  |  WASD/strzalki: kamera  |  Przeciagnij lewy: rysuj  |  Prawy: usun"
              : "EDITOR  |  WASD/arrows: camera  |  Drag left: paint  |  Drag right: erase"));
  painter.restore();
}

void SceneRenderer::drawDoubleJumpFx(
    QPainter& painter,
    const Engine& engine,
    int cameraOffsetXPx,
    int tileSizePx,
    int playerScale) const
{
  constexpr int kFxDuration = 16;
  const int ticksLeft = engine.playerDoubleJumpFxTicks();
  if (ticksLeft <= 0)
  {
    return;
  }

  const double t = 1.0 - (static_cast<double>(ticksLeft) / static_cast<double>(kFxDuration));
  const WorldRect body = engine.playerBounds();
  const QSize playerSize(tileSizePx * playerScale, tileSizePx * playerScale);
  const int playerCenterX =
      static_cast<int>(std::lround(body.origin.x + body.size.width * 0.5f)) -
      cameraOffsetXPx;
  const int feetY = static_cast<int>(std::lround(body.bottom()));
  const int drawX = playerCenterX - playerSize.width() / 2;
  const int drawY = feetY - playerSize.height();
  const int centerX = drawX + playerSize.width() / 2;
  const int baseY = drawY + playerSize.height() - 6;

  painter.save();
  painter.setPen(Qt::NoPen);

  const double spread = 10.0 + 30.0 * t;
  const double drop = 2.0 + 16.0 * t;
  const int alpha = static_cast<int>(180.0 * (1.0 - t));

  const QColor glowColor(190, 235, 255, std::max(0, alpha));
  const QColor coreColor(255, 255, 255, std::max(0, alpha + 20));

  const QPointF offsets[] = {
      QPointF(-1.0, 0.0), QPointF(-0.6, -0.35), QPointF(-0.2, -0.5), QPointF(0.2, -0.5),
      QPointF(0.6, -0.35), QPointF(1.0, 0.0), QPointF(-0.35, 0.15), QPointF(0.35, 0.15)};

  for (std::size_t i = 0; i < std::size(offsets); ++i)
  {
    const QPointF o = offsets[i];
    const double px = centerX + o.x() * spread;
    const double py = baseY + drop + o.y() * 10.0;
    const double rOuter = 2.5 + 2.0 * (1.0 - t);
    const double rInner = 1.4 + 1.2 * (1.0 - t);

    painter.setBrush(glowColor);
    painter.drawEllipse(QPointF(px, py), rOuter, rOuter);
    painter.setBrush(coreColor);
    painter.drawEllipse(QPointF(px, py), rInner, rInner);
  }

  painter.restore();
}

void SceneRenderer::drawLifeBarAboveEnemy(QPainter& painter, const Enemy& enemy, int tileSizePx) const
{
  const auto [enemyTopLeft, enemyBottomRight] = EnemyPresentation::positionToRectPoints(enemy.position(), tileSizePx);
  const QPoint cellSize = enemyBottomRight - enemyTopLeft;
  const int lifeBarLength = EnemyPresentation::lifeBarLengthPx(cellSize.x(), static_cast<int>(enemy.lifePercent()));
  const QPoint lifeBarBottomRight(enemyTopLeft.x() + lifeBarLength, enemyTopLeft.y() + cellSize.y() / 10);

  painter.setBrush(Qt::red);
  painter.drawRect(QRect{enemyTopLeft, lifeBarBottomRight});
}

void SceneRenderer::drawMenuOverlay(QPainter& painter, const QSize& targetSize, const MenuView& menuView) const
{
  if (!menuView.visible)
  {
    return;
  }

  painter.save();

  painter.fillRect(QRect(QPoint(0, 0), targetSize), QColor(0, 0, 0, 150));

  const int panelWidth = targetSize.width() * 2 / 5;
  const int panelHeight = std::min(
      targetSize.height() - 40,
      std::max(
          targetSize.height() * 3 / 5,
          110 + static_cast<int>(menuView.items.size()) * 40));
  const int panelX = (targetSize.width() - panelWidth) / 2;
  const int panelY = (targetSize.height() - panelHeight) / 2;
  const QRect panelRect(panelX, panelY, panelWidth, panelHeight);

  painter.setPen(QPen(QColor(220, 220, 220), 2));
  painter.setBrush(QColor(24, 24, 24, 220));
  painter.drawRoundedRect(panelRect, 12, 12);

  QFont titleFont = painter.font();
  titleFont.setPointSize(20);
  titleFont.setBold(true);
  painter.setFont(titleFont);
  painter.setPen(Qt::white);
  painter.drawText(panelRect.adjusted(0, 16, 0, 0), Qt::AlignTop | Qt::AlignHCenter, QString::fromStdString(menuView.title));

  QFont itemFont = painter.font();
  itemFont.setPointSize(14);
  itemFont.setBold(false);
  painter.setFont(itemFont);

  const int itemStartY = panelY + 80;
  const int itemStep = 40;
  for (int index = 0; index < static_cast<int>(menuView.items.size()); ++index)
  {
    const QRect itemRect(panelX + 24, itemStartY + index * itemStep, panelWidth - 48, 30);
    const int absoluteIndex = menuView.firstVisibleIndex + index;
    if (absoluteIndex == menuView.selectedIndex)
    {
      painter.setPen(Qt::NoPen);
      painter.setBrush(QColor(90, 90, 90, 190));
      painter.drawRoundedRect(itemRect.adjusted(-8, -2, 8, 2), 8, 8);
      painter.setPen(QColor(255, 225, 120));
    }
    else
    {
      painter.setPen(Qt::white);
    }

    const QString label = painter.fontMetrics().elidedText(
        QString::fromStdString(menuView.items[static_cast<std::size_t>(index)]),
        Qt::ElideRight,
        itemRect.width());
    painter.drawText(itemRect, Qt::AlignVCenter | Qt::AlignLeft, label);
  }

  if (menuView.firstVisibleIndex > 0)
  {
    painter.setPen(QColor(200, 200, 200));
    painter.drawText(
        QRect(panelX, panelY + 55, panelWidth, 20),
        Qt::AlignCenter,
        QString::fromUtf8("▲"));
  }
  if (menuView.firstVisibleIndex + static_cast<int>(menuView.items.size()) <
      menuView.totalItemCount)
  {
    painter.setPen(QColor(200, 200, 200));
    painter.drawText(
        QRect(panelX, panelY + panelHeight - 24, panelWidth, 20),
        Qt::AlignCenter,
        QString::fromUtf8("▼"));
  }

  painter.restore();
}
