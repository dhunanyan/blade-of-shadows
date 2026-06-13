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
