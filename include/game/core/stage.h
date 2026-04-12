#ifndef GAME_CORE_STAGE_H
#define GAME_CORE_STAGE_H

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
  std::size_t position2Index(Position position) const;
public:
  Stage(std::size_t width, std::size_t height);
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
  bool isInside(Position position) const;
  std::size_t width() const
  {
      return width_;
  }
  std::size_t height() const
  {
      return height_;
  }
};

#endif // GAME_CORE_STAGE_H
