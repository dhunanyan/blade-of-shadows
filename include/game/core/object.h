#ifndef GAME_CORE_OBJECT_H
#define GAME_CORE_OBJECT_H

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
  Object(ObjectType type);
  Object(ObjectType type, Position position);
  void moveLeft();
  void moveUp();
  void moveRight();
  void moveDown();
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

#endif // GAME_CORE_OBJECT_H
