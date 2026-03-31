#ifndef TILEMAPPER_H
#define TILEMAPPER_H

#include <vector>
#include <QString>
#include <QPixmap>

class QPainter;

class TileMapper
{
public:
    struct Tile
    {
        int x = 0;
        int y = 0;
        QPixmap pixmap;
    };

    bool loadFromJsonResource(const QString& levelResourcePath);
    void render(QPainter& painter, int tileSizePx) const;

    int levelWidth() const { return levelWidth_; }
    int levelHeight() const { return levelHeight_; }
    int tileSizePx() const { return tileSizePx_; }
    bool isLoaded() const { return loaded_; }

private:
    bool loaded_ = false;
    int levelWidth_ = 0;
    int levelHeight_ = 0;
    int tileSizePx_ = 24;
    std::vector<Tile> tiles_;
};

#endif // TILEMAPPER_H
