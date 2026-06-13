#include "game/app/qt/tilemapper.h"

#include <algorithm>
#include <utility>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPainter>
#include <QRect>
#include <QSaveFile>

bool TileMapper::loadFromJsonResource(const QString& levelResourcePath)
{
  QFile file(levelResourcePath);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
  return loadFromDevice(file);
}

bool TileMapper::loadFromJsonFile(const QString& levelFilePath)
{
  QFile file(levelFilePath);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
  return loadFromDevice(file);
}

bool TileMapper::loadFromDevice(QIODevice& device)
{
  tiles_.clear();
  solidGrid_.clear();
  enemySpawns_.clear();
  coinSpawns_.clear();
  decorations_.clear();
  levelExit_.reset();
  loaded_ = false;
  maxOccupiedColumn_ = 0;
  hasPlayerStart_ = false;
  playerStartX_ = 0;
  playerStartY_ = 0;
  levelId_.clear();
  levelName_.clear();

  const QByteArray data = device.readAll();
  const QJsonDocument doc = QJsonDocument::fromJson(data);
  if (!doc.isObject()) return false;

  const QJsonObject root = doc.object();
  levelId_ = root.value("id").toString(root.value("name").toString(QString::fromUtf8("level")));
  levelName_ = root.value("displayName").toString(root.value("name").toString(levelId_));
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

  tileset_.load(tilesetPath);
  if (tileset_.isNull()) return false;
  loadDecorationCatalog();

  const int tilesetCols = tileset_.width() / tileSizePx_;
  const int tilesetRows = tileset_.height() / tileSizePx_;
  if (tilesetCols <= 0 || tilesetRows <= 0) return false;

  const QJsonArray tileRectsArray = root.value("tileRects").toArray();
  for (const QJsonValue& value : tileRectsArray)
  {
    if (!value.isObject()) continue;
    const QJsonObject obj = value.toObject();
    const int startX = obj.value("x").toInt(-1);
    const int startY = obj.value("y").toInt(-1);
    const int width = obj.value("width").toInt(0);
    const int height = obj.value("height").toInt(0);
    const int srcX = obj.value("srcX").toInt(-1);
    const int srcY = obj.value("srcY").toInt(-1);
    const int topSrcX = obj.value("topSrcX").toInt(srcX);
    const int topSrcY = obj.value("topSrcY").toInt(srcY);
    const bool solid = obj.value("solid").toBool(false);
    for (int row = 0; row < height; ++row)
    {
      for (int column = 0; column < width; ++column)
      {
        addTile(
            tileset_,
            tilesetCols,
            tilesetRows,
            startX + column,
            startY + row,
            row == 0 ? topSrcX : srcX,
            row == 0 ? topSrcY : srcY,
            solid);
      }
    }
  }

  const QJsonArray tilesArray = root.value("tiles").toArray();
  for (const QJsonValue& value : tilesArray)
  {
    if (!value.isObject()) continue;

    const QJsonObject obj = value.toObject();
    const int x = obj.value("x").toInt(-1);
    const int y = obj.value("y").toInt(-1);
    const int srcX = obj.value("srcX").toInt(-1);
    const int srcY = obj.value("srcY").toInt(-1);
    const bool solid = obj.value("solid").toBool(false);

    addTile(tileset_, tilesetCols, tilesetRows, x, y, srcX, srcY, solid);
  }

  const auto parsePositions = [&](const char* key, std::vector<Position>& output)
  {
    const QJsonArray array = root.value(QString::fromUtf8(key)).toArray();
    for (const QJsonValue& value : array)
    {
      const QJsonObject object = value.toObject();
      const int x = object.value("x").toInt(-1);
      const int y = object.value("y").toInt(-1);
      if (x >= 0 && y >= 0 && x < levelWidth_ && y < levelHeight_)
      {
        output.emplace_back(x, y);
      }
    }
  };
  parsePositions("enemies", enemySpawns_);
  parsePositions("coins", coinSpawns_);

  for (const QJsonValue& value : root.value("decorations").toArray())
  {
    const QJsonObject object = value.toObject();
    const QString type = object.value("type").toString();
    const int x = object.value("x").toInt(-1);
    const int y = object.value("y").toInt(-1);
    if (decorationAsset(type) != nullptr &&
        x >= 0 && y >= 0 && x < levelWidth_ && y < levelHeight_)
    {
      decorations_.push_back(Decoration{type, x, y});
    }
  }

  const QJsonObject exitObject = root.value("exit").toObject();
  const int exitX = exitObject.value("x").toInt(-1);
  const int exitY = exitObject.value("y").toInt(-1);
  if (exitX >= 0 && exitY >= 0 && exitX < levelWidth_ && exitY < levelHeight_)
  {
    levelExit_ = Position(exitX, exitY);
  }

  if (!hasPlayerStart_)
  {
    return false;
  }

  loaded_ = true;
  return true;
}

bool TileMapper::addTile(
    const QPixmap& tileset,
    int tilesetCols,
    int tilesetRows,
    int x,
    int y,
    int srcX,
    int srcY,
    bool solid)
{
  if (x < 0 || y < 0 || srcX < 0 || srcY < 0) return false;
  if (x >= levelWidth_ || y >= levelHeight_) return false;
  if (srcX >= tilesetCols || srcY >= tilesetRows) return false;

  const QRect sourceRect(srcX * tileSizePx_, srcY * tileSizePx_, tileSizePx_, tileSizePx_);
  const QPixmap tilePixmap = tileset.copy(sourceRect);
  if (tilePixmap.isNull()) return false;

  Tile tile;
  tile.x = x;
  tile.y = y;
  tile.solid = solid;
  tile.srcX = srcX;
  tile.srcY = srcY;
  tile.pixmap = tilePixmap;
  tiles_.push_back(std::move(tile));
  solidGrid_[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)] =
      solidGrid_[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)] || solid;
  maxOccupiedColumn_ = std::max(maxOccupiedColumn_, x);
  return true;
}

bool TileMapper::paintTile(int gridX, int gridY, int srcX, int srcY, bool solid)
{
  if (!loaded_ || tileset_.isNull())
  {
    return false;
  }
  if (solid &&
      hasPlayerStart_ &&
      playerStartX_ == gridX &&
      playerStartY_ == gridY)
  {
    return false;
  }
  if (solid)
  {
    removeEntitiesAt(gridX, gridY);
  }
  removeTile(gridX, gridY);
  return addTile(
      tileset_,
      tileset_.width() / tileSizePx_,
      tileset_.height() / tileSizePx_,
      gridX,
      gridY,
      srcX,
      srcY,
      solid);
}

bool TileMapper::removeTile(int gridX, int gridY)
{
  if (!loaded_ || gridX < 0 || gridY < 0 || gridX >= levelWidth_ || gridY >= levelHeight_)
  {
    return false;
  }
  const auto oldSize = tiles_.size();
  tiles_.erase(
      std::remove_if(
          tiles_.begin(),
          tiles_.end(),
          [&](const Tile& tile)
          {
            return tile.x == gridX && tile.y == gridY;
          }),
      tiles_.end());
  solidGrid_[static_cast<std::size_t>(gridY)][static_cast<std::size_t>(gridX)] = false;
  maxOccupiedColumn_ = 0;
  for (const Tile& tile : tiles_)
  {
    maxOccupiedColumn_ = std::max(maxOccupiedColumn_, tile.x);
  }
  return oldSize != tiles_.size();
}

bool TileMapper::setPlayerStart(int gridX, int gridY)
{
  if (gridX < 0 || gridY < 0 || gridX >= levelWidth_ || gridY >= levelHeight_)
  {
    return false;
  }
  if (isSolidAt(gridX, gridY))
  {
    return false;
  }
  hasPlayerStart_ = true;
  playerStartX_ = gridX;
  playerStartY_ = gridY;
  return true;
}

bool TileMapper::toggleEnemySpawn(int gridX, int gridY)
{
  if (gridX < 0 || gridY < 0 || gridX >= levelWidth_ || gridY >= levelHeight_)
  {
    return false;
  }
  if (isSolidAt(gridX, gridY))
  {
    return false;
  }
  const Position target(gridX, gridY);
  const auto it = std::find(enemySpawns_.begin(), enemySpawns_.end(), target);
  if (it != enemySpawns_.end())
  {
    enemySpawns_.erase(it);
  }
  else
  {
    enemySpawns_.push_back(target);
  }
  return true;
}

bool TileMapper::toggleCoinSpawn(int gridX, int gridY)
{
  if (gridX < 0 || gridY < 0 || gridX >= levelWidth_ || gridY >= levelHeight_)
  {
    return false;
  }
  if (isSolidAt(gridX, gridY))
  {
    return false;
  }
  const Position target(gridX, gridY);
  const auto it = std::find(coinSpawns_.begin(), coinSpawns_.end(), target);
  if (it != coinSpawns_.end())
  {
    coinSpawns_.erase(it);
  }
  else
  {
    coinSpawns_.push_back(target);
  }
  return true;
}

bool TileMapper::setLevelExit(int gridX, int gridY)
{
  if (gridX < 0 || gridY < 0 || gridX >= levelWidth_ || gridY >= levelHeight_)
  {
    return false;
  }
  if (isSolidAt(gridX, gridY))
  {
    return false;
  }
  levelExit_ = Position(gridX, gridY);
  return true;
}

bool TileMapper::placeDecoration(
    const QString& decorationId,
    int gridX,
    int gridY)
{
  if (!loaded_ ||
      gridX < 0 || gridY < 0 ||
      gridX >= levelWidth_ || gridY >= levelHeight_ ||
      decorationAsset(decorationId) == nullptr)
  {
    return false;
  }
  decorations_.erase(
      std::remove_if(
          decorations_.begin(),
          decorations_.end(),
          [&](const Decoration& decoration)
          {
            return decoration.x == gridX && decoration.y == gridY;
          }),
      decorations_.end());
  decorations_.push_back(Decoration{decorationId, gridX, gridY});
  return true;
}

void TileMapper::removeEntitiesAt(int gridX, int gridY)
{
  const Position target(gridX, gridY);
  enemySpawns_.erase(
      std::remove(enemySpawns_.begin(), enemySpawns_.end(), target),
      enemySpawns_.end());
  coinSpawns_.erase(
      std::remove(coinSpawns_.begin(), coinSpawns_.end(), target),
      coinSpawns_.end());
  if (levelExit_ && *levelExit_ == target)
  {
    levelExit_.reset();
  }
  decorations_.erase(
      std::remove_if(
          decorations_.begin(),
          decorations_.end(),
          [&](const Decoration& decoration)
          {
            return decoration.x == gridX && decoration.y == gridY;
          }),
      decorations_.end());
}

bool TileMapper::saveToJsonFile(const QString& levelFilePath) const
{
  if (!loaded_ || !hasPlayerStart_)
  {
    return false;
  }

  QJsonArray tileArray;
  for (const Tile& tile : tiles_)
  {
    tileArray.append(QJsonObject{
        {QString::fromUtf8("x"), tile.x},
        {QString::fromUtf8("y"), tile.y},
        {QString::fromUtf8("srcX"), tile.srcX},
        {QString::fromUtf8("srcY"), tile.srcY},
        {QString::fromUtf8("solid"), tile.solid}});
  }

  const auto positionsToJson = [](const std::vector<Position>& positions)
  {
    QJsonArray array;
    for (const Position& position : positions)
    {
      array.append(QJsonObject{
          {QString::fromUtf8("x"), static_cast<int>(position.x())},
          {QString::fromUtf8("y"), static_cast<int>(position.y())}});
    }
    return array;
  };

  QJsonObject root{
      {QString::fromUtf8("id"), levelId_},
      {QString::fromUtf8("name"), levelName_},
      {QString::fromUtf8("displayName"), levelName_},
      {QString::fromUtf8("width"), levelWidth_},
      {QString::fromUtf8("height"), levelHeight_},
      {QString::fromUtf8("tileSize"), tileSizePx_},
      {QString::fromUtf8("tileset"), QString::fromUtf8(":/tiles/tileset.png")},
      {QString::fromUtf8("playerStart"), QJsonObject{
          {QString::fromUtf8("x"), playerStartX_},
          {QString::fromUtf8("y"), playerStartY_}}},
      {QString::fromUtf8("tiles"), tileArray},
      {QString::fromUtf8("enemies"), positionsToJson(enemySpawns_)},
      {QString::fromUtf8("coins"), positionsToJson(coinSpawns_)}};

  QJsonArray decorationArray;
  for (const Decoration& decoration : decorations_)
  {
    decorationArray.append(QJsonObject{
        {QString::fromUtf8("type"), decoration.id},
        {QString::fromUtf8("x"), decoration.x},
        {QString::fromUtf8("y"), decoration.y}});
  }
  root.insert(QString::fromUtf8("decorations"), decorationArray);

  if (levelExit_)
  {
    root.insert(QString::fromUtf8("exit"), QJsonObject{
        {QString::fromUtf8("x"), static_cast<int>(levelExit_->x())},
        {QString::fromUtf8("y"), static_cast<int>(levelExit_->y())}});
  }

  QDir().mkpath(QFileInfo(levelFilePath).absolutePath());
  QSaveFile file(levelFilePath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    return false;
  }
  file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
  return file.commit();
}

Position TileMapper::safePlayerStart() const
{
  return levelDefinition().resolveStandingSpawn(
      Position(playerStartX_, playerStartY_));
}

LevelDefinition TileMapper::levelDefinition() const
{
  LevelDefinition definition(
      std::max(1, levelWidth_),
      std::max(1, levelHeight_),
      std::max(1, tileSizePx_));
  definition.setId(levelId_.toStdString());
  definition.setDisplayName(levelName_.toStdString());
  definition.setPlayerSpawn(Position(playerStartX_, playerStartY_));
  definition.setEnemySpawns(enemySpawns_);
  definition.setCoinSpawns(coinSpawns_);
  definition.setExit(levelExit_);

  for (int y = 0; y < levelHeight_; ++y)
  {
    for (int x = 0; x < levelWidth_; ++x)
    {
      if (isSolidAt(x, y))
      {
        definition.setSolid(Position(x, y), true);
      }
    }
  }
  return definition;
}

bool TileMapper::isSolidAt(int gridX, int gridY) const
{
  if (!loaded_ || gridX < 0 || gridY < 0) return false;
  if (gridX >= levelWidth_ || gridY >= levelHeight_) return false;

  return solidGrid_[static_cast<std::size_t>(gridY)][static_cast<std::size_t>(gridX)];
}

bool TileMapper::tileHasVisiblePixels(int srcX, int srcY) const
{
  return std::find(
             terrainCatalog_.begin(),
             terrainCatalog_.end(),
             Position(srcX, srcY)) != terrainCatalog_.end();
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

  for (const Decoration& decoration : decorations_)
  {
    const DecorationAsset* asset = decorationAsset(decoration.id);
    if (asset == nullptr || asset->pixmap.isNull())
    {
      continue;
    }
    const QSize size = asset->pixmap.size();
    const int anchorX =
        decoration.x * tileSizePx - cameraOffsetXPx + tileSizePx / 2;
    const int anchorY = (decoration.y + 1) * tileSizePx;
    painter.drawPixmap(
        anchorX - size.width() / 2,
        anchorY - size.height(),
        asset->pixmap);
  }
}

const TileMapper::DecorationAsset* TileMapper::decorationAsset(
    const QString& id) const
{
  const auto it = std::find_if(
      decorationCatalog_.begin(),
      decorationCatalog_.end(),
      [&](const DecorationAsset& asset)
      {
        return asset.id == id;
      });
  return it == decorationCatalog_.end() ? nullptr : &*it;
}

void TileMapper::loadDecorationCatalog()
{
  if (!decorationCatalog_.empty() && !terrainCatalog_.empty())
  {
    return;
  }
  QFile terrainCatalogFile(QString::fromUtf8(":/tiles/catalog.json"));
  if (terrainCatalogFile.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    const QJsonArray terrain =
        QJsonDocument::fromJson(terrainCatalogFile.readAll()).array();
    for (const QJsonValue& value : terrain)
    {
      const QJsonObject object = value.toObject();
      terrainCatalog_.emplace_back(
          object.value(QString::fromUtf8("srcX")).toInt(),
          object.value(QString::fromUtf8("srcY")).toInt());
    }
  }

  if (!decorationCatalog_.empty())
  {
    return;
  }
  const auto add = [&](const char* id, const char* label, int frameWidth = 0)
  {
    QPixmap pixmap(
        QString::fromUtf8(":/decorations/%1.png").arg(QString::fromUtf8(id)));
    if (frameWidth > 0 && pixmap.width() >= frameWidth)
    {
      pixmap = pixmap.copy(0, 0, frameWidth, pixmap.height());
    }
    decorationCatalog_.push_back(DecorationAsset{
        QString::fromUtf8(id),
        QString::fromUtf8(label),
        std::move(pixmap)});
  };
  add("shop", "Shop");
  add("shop_anim", "Shop Alt", 118);
  add("fence_1", "Fence A");
  add("fence_2", "Fence B");
  add("sign", "Sign");
  add("rock_1", "Rock A");
  add("rock_2", "Rock B");
  add("rock_3", "Rock C");
  add("grass_1", "Grass A");
  add("grass_2", "Grass B");
  add("grass_3", "Grass C");
  add("lamp", "Lamp");
}

void TileMapper::renderEditorMarkers(QPainter& painter, int tileSizePx, int cameraOffsetXPx) const
{
  if (!loaded_ || tileSizePx <= 0)
  {
    return;
  }

  const auto markerRect = [&](const Position& position)
  {
    return QRect(
        position.x() * tileSizePx - cameraOffsetXPx + 3,
        position.y() * tileSizePx + 3,
        tileSizePx - 6,
        tileSizePx - 6);
  };

  painter.save();
  painter.setRenderHint(QPainter::Antialiasing, true);

  if (hasPlayerStart_)
  {
    painter.setPen(QPen(QColor(230, 245, 255), 2));
    painter.setBrush(QColor(30, 150, 230, 180));
    const QRect rect = markerRect(Position(playerStartX_, playerStartY_));
    painter.drawEllipse(rect);
    painter.drawText(rect, Qt::AlignCenter, QString::fromUtf8("P"));
  }

  for (const Position& enemy : enemySpawns_)
  {
    painter.setPen(QPen(QColor(255, 210, 210), 2));
    painter.setBrush(QColor(180, 35, 55, 190));
    const QRect rect = markerRect(enemy);
    painter.drawRect(rect);
    painter.drawText(rect, Qt::AlignCenter, QString::fromUtf8("E"));
  }

  for (const Position& coin : coinSpawns_)
  {
    painter.setPen(QPen(QColor(255, 245, 170), 2));
    painter.setBrush(QColor(230, 165, 20, 190));
    const QRect rect = markerRect(coin);
    painter.drawEllipse(rect);
    painter.drawText(rect, Qt::AlignCenter, QString::fromUtf8("C"));
  }

  if (levelExit_)
  {
    painter.setPen(QPen(QColor(180, 255, 255), 2));
    painter.setBrush(QColor(20, 170, 180, 180));
    const QRect rect = markerRect(*levelExit_);
    painter.drawRoundedRect(rect, 4, 4);
    painter.drawText(rect, Qt::AlignCenter, QString::fromUtf8("X"));
  }

  painter.restore();
}
