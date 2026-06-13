#ifndef GAME_CORE_WORLD_GEOMETRY_H
#define GAME_CORE_WORLD_GEOMETRY_H

#include <algorithm>
#include <cmath>
#include "game/core/position.h"

struct WorldPoint
{
  float x = 0.0f;
  float y = 0.0f;

  bool operator==(const WorldPoint&) const = default;
};

struct WorldSize
{
  float width = 0.0f;
  float height = 0.0f;
};

struct WorldRect
{
  WorldPoint origin;
  WorldSize size;

  float left() const { return origin.x; }
  float top() const { return origin.y; }
  float right() const { return origin.x + size.width; }
  float bottom() const { return origin.y + size.height; }

  bool intersects(const WorldRect& other) const
  {
    return left() < other.right() &&
           right() > other.left() &&
           top() < other.bottom() &&
           bottom() > other.top();
  }
};

class PhysicsBody
{
public:
  PhysicsBody(WorldSize size = {16.0f, 32.0f})
      : size_(size)
  {
  }

  const WorldPoint& position() const { return position_; }
  const WorldPoint& velocity() const { return velocity_; }
  const WorldSize& size() const { return size_; }
  WorldRect bounds() const { return {position_, size_}; }

  void setPosition(WorldPoint position) { position_ = position; }
  void setVelocity(WorldPoint velocity) { velocity_ = velocity; }
  void setVelocityX(float velocity) { velocity_.x = velocity; }
  void setVelocityY(float velocity) { velocity_.y = velocity; }

  Position occupiedCell(int tileSize) const
  {
    const float centerX = position_.x + size_.width * 0.5f;
    const float feetY = position_.y + size_.height - 0.001f;
    return Position(
        static_cast<int>(std::floor(centerX / static_cast<float>(tileSize))),
        static_cast<int>(std::floor(feetY / static_cast<float>(tileSize))));
  }

private:
  WorldPoint position_;
  WorldPoint velocity_;
  WorldSize size_;
};

#endif // GAME_CORE_WORLD_GEOMETRY_H
