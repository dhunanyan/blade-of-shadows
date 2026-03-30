#ifndef OBJECT_H
#define OBJECT_H

#include "game/core/position.h"

enum class ObjectType
{
  OBJECT_PLAYER,
  OBJECT_BULLET,
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
  Object(ObjectType type);
  Object(ObjectType type, Position position);
  void moveLeft();
  void moveUp();
  void moveRight();
  void moveDown();
  void moveUpLeft();
  void moveUpRight();
  void moveDownLeft();
  void moveDownRight();
  void setPosition(Position position);
  ObjectType type() const;
  Position position() const;
};

#endif // OBJECT_H
