#ifndef STAGE_H
#define STAGE_H

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>
#include "game/core/position.h"

class Object;

class Stage
{
private:
  std::size_t width_, height_;
  std::vector<std::shared_ptr<Object>> objects_;
  std::size_t position2Index(Position position) const
  {
    return position.x_ + position.y_ * width_;
  }
public:
  Stage(std::size_t width, std::size_t height)
      : width_(width), height_(height), objects_(width * height, nullptr)
  {}
  std::shared_ptr<Object> get(Position position)
  {
    return objects_[position2Index(position)];
  }
  std::shared_ptr<Object> get(Position position) const
  {
    return objects_[position2Index(position)];
  }
  void set(std::shared_ptr<Object> object, Position position)
  {
    objects_[position2Index(position)] = std::move(object);
  }
  bool isInside(Position position) const
  {
    auto [x, y] = position;
    return x == std::clamp(x, static_cast<std::size_t>(0), width() - 1)
        && y == std::clamp(y, static_cast<std::size_t>(0), height() - 1);
  }
  std::size_t width() const
  {
      return width_;
  }
  std::size_t height() const
  {
      return height_;
  }
};

#endif // STAGE_H
