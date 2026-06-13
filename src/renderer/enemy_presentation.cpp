#include "game/renderer/enemy_presentation.h"
#include <cstdlib>

QPoint EnemyPresentation::positionToQPoint(Position position, int tileSizePx)
{
  const int x = static_cast<int>(position.x_) * tileSizePx;
  const int y = static_cast<int>(position.y_) * tileSizePx;
  return QPoint(x, y);
}

std::pair<QPoint, QPoint> EnemyPresentation::positionToRectPoints(Position position, int tileSizePx)
{
  const QPoint from = positionToQPoint(position, tileSizePx);
  const QPoint to = from + QPoint(tileSizePx - 1, tileSizePx - 1);
  return {from, to};
}

int EnemyPresentation::lifeBarLengthPx(int enemyWidthPx, int lifePercent)
{
  return std::abs(enemyWidthPx * lifePercent / 100);
}
