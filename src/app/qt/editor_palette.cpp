#include "game/app/qt/editor_palette.h"

#include <algorithm>
#include <QFont>
#include <QPainter>
#include <QPen>
#include "game/app/qt/tilemapper.h"

namespace
{
constexpr int panelWidth = 300;
constexpr int panelHeight = 270;
constexpr int tabHeight = 34;
constexpr int padding = 12;
} // namespace

QRect EditorPalette::panelRect(const QSize& viewportSize) const
{
  return QRect(
      viewportSize.width() - panelWidth - 14,
      14,
      panelWidth,
      panelHeight);
}

QRect EditorPalette::terrainTabRect(const QSize& viewportSize) const
{
  const QRect panel = panelRect(viewportSize);
  return QRect(panel.x() + padding, panel.y() + padding, 128, tabHeight);
}

QRect EditorPalette::decorationTabRect(const QSize& viewportSize) const
{
  const QRect terrain = terrainTabRect(viewportSize);
  return QRect(terrain.right() + 8, terrain.y(), 128, tabHeight);
}

QRect EditorPalette::contentRect(const QSize& viewportSize) const
{
  const QRect panel = panelRect(viewportSize);
  return QRect(
      panel.x() + padding,
      panel.y() + padding + tabHeight + 10,
      panel.width() - padding * 2,
      panel.height() - padding * 2 - tabHeight - 10);
}

void EditorPalette::render(
    QPainter& painter,
    const QSize& viewportSize,
    const TileMapper& tileMapper) const
{
  painter.save();
  painter.setRenderHint(QPainter::Antialiasing, true);

  const QRect panel = panelRect(viewportSize);
  painter.setPen(QPen(QColor(218, 224, 235), 2));
  painter.setBrush(QColor(8, 12, 20, 232));
  painter.drawRoundedRect(panel, 10, 10);

  const auto drawTab = [&](const QRect& rect, const QString& label, bool active)
  {
    painter.setPen(active ? QColor(255, 225, 120) : QColor(185, 195, 210));
    painter.setBrush(active ? QColor(72, 82, 98) : QColor(28, 35, 46));
    painter.drawRoundedRect(rect, 6, 6);
    painter.drawText(rect, Qt::AlignCenter, label);
  };
  drawTab(
      terrainTabRect(viewportSize),
      QString::fromUtf8("Terrain"),
      mode_ == Mode::Terrain);
  drawTab(
      decorationTabRect(viewportSize),
      QString::fromUtf8("Decorations"),
      mode_ == Mode::Decoration);

  const QRect content = contentRect(viewportSize);
  if (mode_ == Mode::Terrain)
  {
    const int columns = std::max(1, tileMapper.tilesetColumns());
    const int rows = std::max(1, tileMapper.tilesetRows());
    const int cellSize = std::max(
        1,
        std::min(content.width() / columns, content.height() / rows));
    const QRect atlasRect(
        content.x(),
        content.y(),
        columns * cellSize,
        rows * cellSize);
    painter.drawPixmap(atlasRect, tileMapper.tilesetPixmap());
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 150));
    for (int row = 0; row < rows; ++row)
    {
      for (int column = 0; column < columns; ++column)
      {
        if (!tileMapper.tileHasVisiblePixels(column, row))
        {
          painter.drawRect(QRect(
              atlasRect.x() + column * cellSize,
              atlasRect.y() + row * cellSize,
              cellSize,
              cellSize));
        }
      }
    }
    painter.setPen(QPen(QColor(255, 225, 80), 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRect(
        atlasRect.x() + tileX_ * cellSize,
        atlasRect.y() + tileY_ * cellSize,
        cellSize,
        cellSize));
  }
  else
  {
    const auto& decorations = tileMapper.decorationCatalog();
    constexpr int columns = 4;
    const int cellWidth = content.width() / columns;
    constexpr int cellHeight = 64;
    for (int index = 0; index < static_cast<int>(decorations.size()); ++index)
    {
      const auto& decoration = decorations[static_cast<std::size_t>(index)];
      const QRect cell(
          content.x() + (index % columns) * cellWidth,
          content.y() + (index / columns) * cellHeight,
          cellWidth - 4,
          cellHeight - 4);
      painter.setPen(
          decoration.id == decorationId_
              ? QPen(QColor(255, 225, 80), 2)
              : QPen(QColor(100, 112, 130), 1));
      painter.setBrush(QColor(22, 29, 39));
      painter.drawRoundedRect(cell, 5, 5);
      const QRect preview = cell.adjusted(5, 4, -5, -19);
      painter.drawPixmap(
          preview,
          decoration.pixmap.scaled(
              preview.size(),
              Qt::KeepAspectRatio,
              Qt::FastTransformation));
      QFont font = painter.font();
      font.setPointSize(7);
      painter.setFont(font);
      painter.setPen(Qt::white);
      painter.drawText(
          cell.adjusted(3, 0, -3, -2),
          Qt::AlignBottom | Qt::AlignHCenter,
          decoration.label);
    }
  }

  painter.restore();
}

bool EditorPalette::handleClick(
    const QPoint& point,
    const QSize& viewportSize,
    const TileMapper& tileMapper)
{
  if (!panelRect(viewportSize).contains(point))
  {
    return false;
  }
  if (terrainTabRect(viewportSize).contains(point))
  {
    mode_ = Mode::Terrain;
    return true;
  }
  if (decorationTabRect(viewportSize).contains(point))
  {
    mode_ = Mode::Decoration;
    return true;
  }

  const QRect content = contentRect(viewportSize);
  if (!content.contains(point))
  {
    return true;
  }
  if (mode_ == Mode::Terrain)
  {
    const int columns = std::max(1, tileMapper.tilesetColumns());
    const int rows = std::max(1, tileMapper.tilesetRows());
    const int cellSize = std::max(
        1,
        std::min(content.width() / columns, content.height() / rows));
    const int column = (point.x() - content.x()) / cellSize;
    const int row = (point.y() - content.y()) / cellSize;
    if (column >= 0 && column < columns &&
        row >= 0 && row < rows &&
        tileMapper.tileHasVisiblePixels(column, row))
    {
      tileX_ = column;
      tileY_ = row;
    }
  }
  else
  {
    constexpr int columns = 4;
    const int cellWidth = content.width() / columns;
    constexpr int cellHeight = 64;
    const int column = (point.x() - content.x()) / cellWidth;
    const int row = (point.y() - content.y()) / cellHeight;
    const int index = row * columns + column;
    const auto& decorations = tileMapper.decorationCatalog();
    if (index >= 0 && index < static_cast<int>(decorations.size()))
    {
      decorationId_ = decorations[static_cast<std::size_t>(index)].id;
    }
  }
  return true;
}

QString EditorPalette::selectionLabel() const
{
  if (mode_ == Mode::Terrain)
  {
    return QString::fromUtf8("Terrain (%1, %2)").arg(tileX_).arg(tileY_);
  }
  return QString::fromUtf8("Decoration: %1").arg(decorationId_);
}
