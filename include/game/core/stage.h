#ifndef STAGE_H
#define STAGE_H

#include <vector>
#include <memory>

class Object;
struct Position;

class Stage
{
private:
  std::size_t width_, height_;
  std::vector<std::shared_ptr<Object>> objects_;
  std::size_t position2Index(Position position) const;
public:
  Stage(std::size_t width, std::size_t height)
      : width_(width), height_(height), objects_(width * height, nullptr)
  {}
  std::shared_ptr<Object> get(Position position);
  std::shared_ptr<Object> get(Position position) const;
  void set(std::shared_ptr<Object> object, Position position);
  bool isInside(Position position) const;
  auto width() const
  {
      return width_;
  }
  auto height() const
  {
      return height_;
  }
};

#endif // STAGE_H
