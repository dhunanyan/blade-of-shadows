#include <algorithm>
#include "game/core/stage.h"

Stage::Stage(std::size_t width, std::size_t height)
  : width_(width), height_(height), objects_(width * height, nullptr)
{
}

std::size_t Stage::position2Index(Position position) const
{
  return position.x_ + position.y_ * width_;
}

bool Stage::isInside(Position position) const
{
  auto [x, y] = position;
  return x == std::clamp(x, static_cast<std::size_t>(0), width() - 1)
      && y == std::clamp(y, static_cast<std::size_t>(0), height() - 1);
}
