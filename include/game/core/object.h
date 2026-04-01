#ifndef OBJECT_H
#define OBJECT_H

#include "game/core/position.h"

enum class ObjectType
{
  OBJECT_PLAYER,
  OBJECT_ENEMY,
  OBJECT_WALL,
  OBJECT_UNKNOWN
};

class Object
{
protected:
  ObjectType type_;
  Position position_;
public:
  Object(ObjectType type): Object(type, Position(0, 0)) {}
  Object(ObjectType type, Position position): type_(type), position_(position) {}
  void moveLeft();
  void moveUp();
  void moveRight();
  void moveDown();
  void moveUpLeft();
  void moveUpRight();
  void moveDownLeft();
  void moveDownRight();
  void setPosition(Position position)
  {
    position_ = position;
  }
  ObjectType type() const
  {
    return type_;
  }
  Position position() const
  {
    return position_;
  }
};

#endif // OBJECT_H
