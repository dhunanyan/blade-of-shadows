#ifndef GAME_CORE_POSITION_H
#define GAME_CORE_POSITION_H

struct Position
{
  int x_ = 0;
  int y_ = 0;

  Position(int x = 0, int y = 0) : x_(x), y_(y) {}

  int x() const { return x_; }
  int y() const { return y_; }

  Position& moveUp()
  {
    --y_;
    return *this;
  }
  Position& moveDown()
  {
    ++y_;
    return *this;
  }
  Position& moveLeft()
  {
    --x_;
    return *this;
  }
  Position& moveRight()
  {
    x_++;
    return *this;
  }
  bool operator==(const Position&) const = default;
};

#endif // GAME_CORE_POSITION_H
