#ifndef BULLET_H
#define BULLET_H

#include "game/core/object.h"
#include "game/core/direction.h"

class Bullet : public Object
{
private:
  Direction direction_;
public:
  static float speed_;
  static float damage_;
  Bullet(Direction direction, Position position)
    : Object(ObjectType::OBJECT_BULLET, position), direction_(direction)
  {}
  Direction direction() const
  {
    return direction_;
  }
  void setDirection(Direction direction)
  {
    direction_ = direction;
  }
  Position position() const
  {
    return position_;
  }
  void setPosition(Position position)
  {
    position_ = position;
  }
  static float speed()
  {
    return speed_;
  }
  static float damage()
  {
    return damage_;
  }
};

#endif // BULLET_H
