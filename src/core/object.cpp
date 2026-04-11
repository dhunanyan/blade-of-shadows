#include "game/core/object.h"

Object::Object(ObjectType type)
  : Object(type, Position(0, 0))
{
}

Object::Object(ObjectType type, Position position)
  : type_(type), position_(position)
{
}

void Object::moveLeft()
{
  position_.moveLeft();
}
void Object::moveUp()
{
  position_.moveUp();
}
void Object::moveRight()
{
  position_.moveRight();
}
void Object::moveDown()
{
  position_.moveDown();
}
void Object::moveUpLeft()
{
  position_.moveUpLeft();
}
void Object::moveUpRight()
{
  position_.moveUpRight();
}
void Object::moveDownLeft()
{
  position_.moveDownLeft();
}
void Object::moveDownRight()
{
  position_.moveDownRight();
}
