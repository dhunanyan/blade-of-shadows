#include <algorithm>
#include "game/core/stage.h"
#include "game/core/position.h"

std::size_t Stage::position2Index(Position position) const
{
  return position.x_ + position.y_ * width_;
}

std::shared_ptr<Object> Stage::get(Position position)
{
  auto index = position2Index(position);
  return objects_[index];
}

std::shared_ptr<Object> Stage::get(Position position) const
{
  auto index = position2Index(position);
  return objects_[index];
}

void Stage::set(std::shared_ptr<Object> object, Position position)
{
  auto index = position2Index(position);
  objects_[index] = object;
}

bool Stage::isInside(Position position) const
{
  auto [x, y] = position;
  return x == std::clamp(x, static_cast<size_t>(0), width() - 1)
        && y == std::clamp(y, static_cast<size_t>(0), height() - 1);
}
