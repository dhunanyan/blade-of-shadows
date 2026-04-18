#include "game/renderer/scene_renderer.h"
#include <cmath>
#include <QFont>
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
    int playerScale) const
{
  const QSize targetSize(
      static_cast<int>(engine.stageWidthCells()) * tileSizePx,
      static_cast<int>(engine.stageHeightCells()) * tileSizePx);

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

  tileMapper.render(painter, tileSizePx);
  drawPlayer(painter, engine, playerPresentation, assets, tileSizePx, playerScale);
  drawEnemies(painter, engine, assets, tileSizePx);
  drawMenuOverlay(painter, targetSize, menuView);

  return composed;
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

  const auto [topLeft, bottomRight] = EnemyPresentation::positionToRectPoints(engine.playerPosition(), tileSizePx);
  const QPoint cellSize = bottomRight - topLeft + QPoint(1, 1);
  const QSize targetSize(cellSize.x() * playerScale, cellSize.y() * playerScale);

  const int playerX = static_cast<int>(std::lround(engine.playerPixelX()));
  const int playerY = static_cast<int>(std::lround(engine.playerPixelY()));
  const int drawX = playerX - (targetSize.width() - cellSize.x()) / 2;
  const int drawY = playerY + cellSize.y() - targetSize.height();
  const QRect targetRect(QPoint(drawX, drawY), targetSize);
  painter.drawPixmap(targetRect, frame);
}

void SceneRenderer::drawEnemies(
    QPainter& painter,
    const Engine& engine,
    const AssetRepository& assets,
    int tileSizePx) const
{
  for (const auto& enemy : engine.enemies())
  {
    const auto [topLeft, bottomRight] = EnemyPresentation::positionToRectPoints(enemy->position(), tileSizePx);
    const QRect enemyRect(topLeft, bottomRight);
    painter.drawPixmap(enemyRect, assets.enemyTexture());
    drawLifeBarAboveEnemy(painter, *enemy, tileSizePx);
  }
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
  const int panelHeight = targetSize.height() * 3 / 5;
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
    if (index == menuView.selectedIndex)
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

    painter.drawText(itemRect, Qt::AlignVCenter | Qt::AlignLeft, QString::fromStdString(menuView.items[static_cast<std::size_t>(index)]));
  }

  painter.restore();
}
