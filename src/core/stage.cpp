#include <algorithm>
#include "game/core/stage.h"

Stage::Stage(std::size_t width, std::size_t height)
  : width_(width), height_(height), objects_(width * height, nullptr)
{
}

std::size_t Stage::position2Index(Position position) const
{
  return static_cast<std::size_t>(position.x_) +
         static_cast<std::size_t>(position.y_) * width_;
}

bool Stage::isInside(Position position) const
{
  return position.x() >= 0 &&
         position.y() >= 0 &&
         static_cast<std::size_t>(position.x()) < width() &&
         static_cast<std::size_t>(position.y()) < height();
}
