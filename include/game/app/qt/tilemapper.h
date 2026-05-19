#ifndef GAME_APP_QT_TILEMAPPER_H
#define GAME_APP_QT_TILEMAPPER_H

#include <vector>
#include <QString>
#include <QPixmap>

class QPainter;

class TileMapper
{
public:
    struct Tile
    {
        bool solid;
        int x = 0;
        int y = 0;
        QPixmap pixmap;
    };

    bool loadFromJsonResource(const QString& levelResourcePath);
    void render(QPainter& painter, int tileSizePx, int cameraOffsetXPx = 0) const;

    int levelWidth() const { return levelWidth_; }
    int levelHeight() const { return levelHeight_; }
    int tileSizePx() const { return tileSizePx_; }
    int maxOccupiedColumn() const { return maxOccupiedColumn_; }
    bool hasPlayerStart() const { return hasPlayerStart_; }
    int playerStartX() const { return playerStartX_; }
    int playerStartY() const { return playerStartY_; }
    bool isLoaded() const { return loaded_; }
    bool isSolidAt(int gridX, int gridY) const;

private:
    bool loaded_ = false;
    int levelWidth_ = 0;
    int levelHeight_ = 0;
    int tileSizePx_ = 24;
    int maxOccupiedColumn_ = 0;
    bool hasPlayerStart_ = false;
    int playerStartX_ = 0;
    int playerStartY_ = 0;
    std::vector<Tile> tiles_;
    std::vector<std::vector<bool>> solidGrid_;
};

#endif // GAME_APP_QT_TILEMAPPER_H
