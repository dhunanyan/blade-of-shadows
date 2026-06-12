#ifndef GAME_APP_QT_TILEMAPPER_H
#define GAME_APP_QT_TILEMAPPER_H

#include <vector>
#include <optional>
#include <QString>
#include <QPixmap>
#include "game/core/position.h"

class QPainter;

class TileMapper
{
public:
    struct Tile
    {
        bool solid;
        int x = 0;
        int y = 0;
        int srcX = 0;
        int srcY = 0;
        QPixmap pixmap;
    };

    bool loadFromJsonResource(const QString& levelResourcePath);
    bool loadFromJsonFile(const QString& levelFilePath);
    bool saveToJsonFile(const QString& levelFilePath) const;
    bool paintTile(int gridX, int gridY, int srcX, int srcY, bool solid);
    bool removeTile(int gridX, int gridY);
    bool setPlayerStart(int gridX, int gridY);
    bool toggleEnemySpawn(int gridX, int gridY);
    bool toggleCoinSpawn(int gridX, int gridY);
    bool setLevelExit(int gridX, int gridY);
    void removeEntitiesAt(int gridX, int gridY);
    void render(QPainter& painter, int tileSizePx, int cameraOffsetXPx = 0) const;
    void renderEditorMarkers(QPainter& painter, int tileSizePx, int cameraOffsetXPx = 0) const;

    int levelWidth() const { return levelWidth_; }
    int levelHeight() const { return levelHeight_; }
    int tileSizePx() const { return tileSizePx_; }
    int tilesetColumns() const { return tileset_.isNull() ? 0 : tileset_.width() / tileSizePx_; }
    int tilesetRows() const { return tileset_.isNull() ? 0 : tileset_.height() / tileSizePx_; }
    int maxOccupiedColumn() const { return maxOccupiedColumn_; }
    bool hasPlayerStart() const { return hasPlayerStart_; }
    int playerStartX() const { return playerStartX_; }
    int playerStartY() const { return playerStartY_; }
    Position safePlayerStart() const;
    const std::vector<Position>& enemySpawns() const { return enemySpawns_; }
    const std::vector<Position>& coinSpawns() const { return coinSpawns_; }
    const std::optional<Position>& levelExit() const { return levelExit_; }
    const QString& levelId() const { return levelId_; }
    const QString& levelName() const { return levelName_; }
    bool isLoaded() const { return loaded_; }
    bool isSolidAt(int gridX, int gridY) const;

private:
    bool loadFromDevice(class QIODevice& device);
    bool addTile(
        const QPixmap& tileset,
        int tilesetCols,
        int tilesetRows,
        int x,
        int y,
        int srcX,
        int srcY,
        bool solid);

    bool loaded_ = false;
    int levelWidth_ = 0;
    int levelHeight_ = 0;
    int tileSizePx_ = 24;
    int maxOccupiedColumn_ = 0;
    bool hasPlayerStart_ = false;
    int playerStartX_ = 0;
    int playerStartY_ = 0;
    QString levelId_;
    QString levelName_;
    std::vector<Tile> tiles_;
    std::vector<std::vector<bool>> solidGrid_;
    QPixmap tileset_;
    std::vector<Position> enemySpawns_;
    std::vector<Position> coinSpawns_;
    std::optional<Position> levelExit_;
};

#endif // GAME_APP_QT_TILEMAPPER_H
