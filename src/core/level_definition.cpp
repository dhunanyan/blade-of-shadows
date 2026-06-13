#include "game/core/level_definition.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

LevelDefinition::LevelDefinition(int width, int height, int tileSize)
    : width_(std::max(1, width)),
      height_(std::max(1, height)),
      tileSize_(std::max(1, tileSize)),
      solidCells_(static_cast<std::size_t>(width_ * height_), false)
{
}

bool LevelDefinition::isInside(Position cell) const
{
  return cell.x() >= 0 &&
         cell.y() >= 0 &&
         cell.x() < width_ &&
         cell.y() < height_;
}

bool LevelDefinition::isSolid(Position cell) const
{
  if (!isInside(cell))
  {
    return true;
  }
  return solidCells_[index(cell)];
}

void LevelDefinition::setSolid(Position cell, bool solid)
{
  if (!isInside(cell))
  {
    throw std::out_of_range("Level cell is outside the level");
  }
  solidCells_[index(cell)] = solid;
}

Position LevelDefinition::resolveStandingSpawn(
    Position requested,
    WorldSize bodySize) const
{
  const int x = std::clamp(requested.x(), 0, width_ - 1);
  const int requestedY = std::clamp(requested.y(), 0, height_ - 1);
  const auto canStandAt = [this, x, bodySize](int y)
  {
    if (y < 0 ||
        y >= height_ ||
        isSolid(x, y) ||
        (y + 1 < height_ && !isSolid(x, y + 1)))
    {
      return false;
    }
    if (bodySize.width <= 0.0f || bodySize.height <= 0.0f)
    {
      return true;
    }

    const float tile = static_cast<float>(tileSize_);
    const WorldPoint origin{
        x * tile + (tile - bodySize.width) * 0.5f,
        (y + 1) * tile - bodySize.height};
    if (origin.x < 0.0f || origin.y < 0.0f)
    {
      return false;
    }

    constexpr float epsilon = 0.001f;
    const int left = static_cast<int>(std::floor(origin.x / tile));
    const int right = static_cast<int>(
        std::floor((origin.x + bodySize.width - epsilon) / tile));
    const int top = static_cast<int>(std::floor(origin.y / tile));
    const int bottom = static_cast<int>(
        std::floor((origin.y + bodySize.height - epsilon) / tile));
    for (int bodyY = top; bodyY <= bottom; ++bodyY)
    {
      for (int bodyX = left; bodyX <= right; ++bodyX)
      {
        if (isSolid(bodyX, bodyY))
        {
          return false;
        }
      }
    }
    return true;
  };

  if (canStandAt(requestedY))
  {
    return Position(x, requestedY);
  }

  for (int distance = 1; distance < height_; ++distance)
  {
    if (canStandAt(requestedY - distance))
    {
      return Position(x, requestedY - distance);
    }
    if (canStandAt(requestedY + distance))
    {
      return Position(x, requestedY + distance);
    }
  }

  return Position(x, 0);
}

WorldPoint LevelDefinition::worldPositionForStandingCell(
    Position cell,
    WorldSize bodySize) const
{
  const Position safeCell = resolveStandingSpawn(cell, bodySize);
  const float tile = static_cast<float>(tileSize_);
  return WorldPoint{
      safeCell.x() * tile + (tile - bodySize.width) * 0.5f,
      (safeCell.y() + 1) * tile - bodySize.height};
}

WorldRect LevelDefinition::cellBounds(Position cell) const
{
  const float tile = static_cast<float>(tileSize_);
  return WorldRect{
      WorldPoint{cell.x() * tile, cell.y() * tile},
      WorldSize{tile, tile}};
}

std::size_t LevelDefinition::index(Position cell) const
{
  return static_cast<std::size_t>(cell.y() * width_ + cell.x());
}
