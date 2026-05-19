#include "game/app/qt/tilemapper.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPainter>
#include <QRect>

bool TileMapper::loadFromJsonResource(const QString& levelResourcePath)
{
  tiles_.clear();
  solidGrid_.clear();
  loaded_ = false;
  maxOccupiedColumn_ = 0;
  hasPlayerStart_ = false;
  playerStartX_ = 0;
  playerStartY_ = 0;

  QFile file(levelResourcePath);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

  const QByteArray data = file.readAll();
  const QJsonDocument doc = QJsonDocument::fromJson(data);
  if (!doc.isObject()) return false;

  const QJsonObject root = doc.object();
  levelWidth_ = root.value("width").toInt();
  levelHeight_ = root.value("height").toInt();
  tileSizePx_ = root.value("tileSize").toInt(24);
  const QString tilesetPath = root.value("tileset").toString();
  const QJsonObject playerStartObj = root.value("playerStart").toObject();

  if (levelWidth_ <= 0 || levelHeight_ <= 0 || tileSizePx_ <= 0 || tilesetPath.isEmpty()) return false;

  if (!playerStartObj.isEmpty())
  {
    const int startX = playerStartObj.value("x").toInt(-1);
    const int startY = playerStartObj.value("y").toInt(-1);
    if (startX >= 0 && startY >= 0 && startX < levelWidth_ && startY < levelHeight_)
    {
      hasPlayerStart_ = true;
      playerStartX_ = startX;
      playerStartY_ = startY;
    }
  }
  solidGrid_.assign(static_cast<std::size_t>(levelHeight_),
                    std::vector<bool>(static_cast<std::size_t>(levelWidth_), false));

  const QPixmap tileset(tilesetPath);
  if (tileset.isNull()) return false;

  const int tilesetCols = tileset.width() / tileSizePx_;
  const int tilesetRows = tileset.height() / tileSizePx_;
  if (tilesetCols <= 0 || tilesetRows <= 0) return false;

  const QJsonArray tilesArray = root.value("tiles").toArray();
  tiles_.reserve(static_cast<std::size_t>(tilesArray.size()));

  for (const QJsonValue& value : tilesArray)
  {
    if (!value.isObject()) continue;

    const QJsonObject obj = value.toObject();
    const int x = obj.value("x").toInt(-1);
    const int y = obj.value("y").toInt(-1);
    const int srcX = obj.value("srcX").toInt(-1);
    const int srcY = obj.value("srcY").toInt(-1);
    const bool solid = obj.value("solid").toBool(false);

    if (x < 0 || y < 0 || srcX < 0 || srcY < 0) continue;
    if (x >= levelWidth_ || y >= levelHeight_) continue;
    if (srcX >= tilesetCols || srcY >= tilesetRows) continue;

    const QRect sourceRect(srcX * tileSizePx_, srcY * tileSizePx_, tileSizePx_, tileSizePx_);
    const QPixmap tilePixmap = tileset.copy(sourceRect);
    if (tilePixmap.isNull()) continue;

    Tile tile;
    tile.x = x;
    tile.y = y;
    tile.solid = solid;
    solidGrid_[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)] = solid;
    tile.pixmap = tilePixmap;
    tiles_.push_back(tile);
    if (x > maxOccupiedColumn_)
    {
      maxOccupiedColumn_ = x;
    }
  }

  loaded_ = true;
  return true;
}

bool TileMapper::isSolidAt(int gridX, int gridY) const
{
  if (!loaded_ || gridX < 0 || gridY < 0) return false;
  if (gridX >= levelWidth_ || gridY >= levelHeight_) return false;

  return solidGrid_[static_cast<std::size_t>(gridY)][static_cast<std::size_t>(gridX)];
}

void TileMapper::render(QPainter& painter, int tileSizePx, int cameraOffsetXPx) const
{
  if (!loaded_ || tileSizePx <= 0) return;

  for (const Tile& tile : tiles_)
  {
    const QRect targetRect(
        tile.x * tileSizePx - cameraOffsetXPx,
        tile.y * tileSizePx,
        tileSizePx,
        tileSizePx);
    painter.drawPixmap(targetRect, tile.pixmap);
  }
}
