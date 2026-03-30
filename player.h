// author: Davit Hunanyan
#ifndef PLAYER_H
#define PLAYER_H

#include "object.h"
#include "direction.h"
 
class Player : public Object
{
private:
  bool isAlive_;
  Direction direction_;
public:
  Player(): Object(ObjectType::OBJECT_PLAYER), isAlive_(true), direction_(Direction::RIGHT) {}
  Direction direction() const
  {
    return direction_;
  }
  void setDirection(Direction direction)
  {
    direction_ = direction;
  }
  bool isAlive() const
  {
    return isAlive_;
  }
  void setIsAlive(bool isAlive)
  {
    isAlive_ = isAlive;
  }
};

#endif // PLAYER_H
