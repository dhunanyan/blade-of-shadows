#ifndef GAME_APP_QT_EDITOR_PALETTE_H
#define GAME_APP_QT_EDITOR_PALETTE_H

#include <QString>
#include <QRect>

class QPainter;
class TileMapper;

class EditorPalette
{
public:
  enum class Mode
  {
    Terrain,
    Decoration
  };

  void render(
      QPainter& painter,
      const QSize& viewportSize,
      const TileMapper& tileMapper) const;
  bool handleClick(
      const QPoint& point,
      const QSize& viewportSize,
      const TileMapper& tileMapper);
  bool contains(const QPoint& point, const QSize& viewportSize) const
  {
    return panelRect(viewportSize).contains(point);
  }

  Mode mode() const { return mode_; }
  int tileX() const { return tileX_; }
  int tileY() const { return tileY_; }
  const QString& decorationId() const { return decorationId_; }
  QString selectionLabel() const;

private:
  QRect panelRect(const QSize& viewportSize) const;
  QRect terrainTabRect(const QSize& viewportSize) const;
  QRect decorationTabRect(const QSize& viewportSize) const;
  QRect contentRect(const QSize& viewportSize) const;

  Mode mode_ = Mode::Terrain;
  int tileX_ = 5;
  int tileY_ = 13;
  QString decorationId_ = QString::fromUtf8("grass_1");
};

#endif // GAME_APP_QT_EDITOR_PALETTE_H
