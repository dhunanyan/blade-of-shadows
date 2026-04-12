#ifndef GAME_RENDERER_ENEMY_PRESENTATION_H
#define GAME_RENDERER_ENEMY_PRESENTATION_H

#include <utility>
#include <QPoint>
#include "game/core/position.h"

class Enemy;

class EnemyPresentation
{
public:
  static QPoint positionToQPoint(Position position, int tileSizePx);
  static std::pair<QPoint, QPoint> positionToRectPoints(Position position, int tileSizePx);
  static int lifeBarLengthPx(int enemyWidthPx, int lifePercent);
};

#endif // GAME_RENDERER_ENEMY_PRESENTATION_H
