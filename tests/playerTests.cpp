#include <iostream>
#include <type_traits>
#include <gtest/gtest.h>

#include "../player.h"


using namespace std;
using namespace ::testing;


struct PlayerTester : public ::testing::Test
{
};

TEST_F(PlayerTester, playerIsObject)
{
#ifdef UNIMPLEMENTED_playerIsObject
    ADD_FAILURE() << "Not implemented UNIMPLEMENTED_playerIsObject";
#else
    constexpr bool objectIsBaseForPlayer = std::is_base_of_v<Object, Player>;
    ASSERT_TRUE(objectIsBaseForPlayer);

    const ObjectType objectType = ObjectType::OBJECT_PLAYER;
    Player object;

    ASSERT_EQ(objectType, object.type());
#endif
}

TEST_F(PlayerTester, playerCanMoveLikeObject)
{
#ifdef UNIMPLEMENTED_playerIsObject
    ADD_FAILURE() << "Not implemented UNIMPLEMENTED_playerIsObject";
#else
    const Position position(5, 6);
    const Position positionAfterMoving(6, 7);

    Player player;
    player.setPosition(position);
    player.moveUp();
    player.moveRight();

    ASSERT_EQ(positionAfterMoving.x(), player.position().x());
    ASSERT_EQ(positionAfterMoving.y(), player.position().y());
#endif
}

TEST_F(PlayerTester, playerHasDirection)
{
#ifdef UNIMPLEMENTED_playerHasDirection
    ADD_FAILURE() << "Not implemented UNIMPLEMENTED_playerHasDirection";
#else
    Player player;

    const Direction defaultDirection = Direction::RIGHT;

    const Direction direction = Direction::UP;
    ASSERT_EQ(defaultDirection, player.direction());

    player.setDirection(direction);
    ASSERT_EQ(direction, player.direction());
#endif
}

TEST_F(PlayerTester, playerCanDie)
{
#ifdef UNIMPLEMENTED_playerCanDie
    ADD_FAILURE() << "Not implemented UNIMPLEMENTED_playerCanDie";
#else
    Player player;
    ASSERT_TRUE(player.isAlive());
    
    player.setIsAlive(false);

    ASSERT_FALSE(player.isAlive());
#endif
}
