// author: Davit Hunanyan
#ifndef POSITION_H
#define POSITION_H

#include <cstddef>  // std::size_t

struct Position
{
  std::size_t x_=0, y_=0;
  Position(std::size_t x={}, std::size_t y={}): x_(x), y_(y)
  {}
  auto x() const
  {
    return x_;
  }
  auto y() const
  {
    return y_;
  }
  Position& moveUp()
  {
    y_++;
    return *this;
  }
  Position& moveDown()
  {
    y_--;
    return *this;
  }
  Position& moveLeft()
  {
    x_--;
    return *this;
  }
  Position& moveRight()
  {
    x_++;
    return *this;
  }
  Position& moveUpRight()
  {
    return moveUp().moveRight();
  }
  Position& moveUpLeft()
  {
    return moveUp().moveLeft();
  }
  Position& moveDownRight()
  {
    return moveDown().moveRight();
  }
  Position& moveDownLeft()
  {
    return moveDown().moveLeft();
  }
};

#endif // POSITION_H
