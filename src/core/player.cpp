#include "game/core/player.h"

Player::Player()
  : Object(ObjectType::OBJECT_PLAYER), isAlive_(true), direction_(Direction::RIGHT)
{
}
