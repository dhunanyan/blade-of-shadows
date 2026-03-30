#include <iostream>
#include <type_traits>
#include <gtest/gtest.h>

#include "../bullet.h"


using namespace std;
using namespace ::testing;


struct BulletTester : public ::testing::Test
{
};

TEST_F(BulletTester, shootIsObject)
{
#ifdef UNIMPLEMENTED_bulletIsObject
    ADD_FAILURE() << "Not implemented UNIMPLEMENTED_bulletIsObject";
#else
    constexpr bool objectIsBaseForPlayer = std::is_base_of_v<Object, Bullet>;
    ASSERT_TRUE(objectIsBaseForPlayer);
    
    const Bullet shoot(Direction::RIGHT, Position{});
    
    ASSERT_EQ(ObjectType::OBJECT_BULLET, shoot.type());
#endif
}

TEST_F(BulletTester, shootHasDirectionAndPosition)
{
#ifdef UNIMPLEMENTED_bulletHasDirectionAndPosition
    ADD_FAILURE() << "Not implemented UNIMPLEMENTED_bulletHasDirectionAndPosition";
#else
    const Position position(5, 6), positionToTheLeftAndDown(4, 5);
    const Direction direction = Direction::UP;
    
    Bullet shoot(direction, position);
    ASSERT_EQ(position.x(), shoot.position().x());
    ASSERT_EQ(position.y(), shoot.position().y());
    ASSERT_EQ(direction, shoot.direction());

    shoot.setPosition(positionToTheLeftAndDown);
    ASSERT_EQ(positionToTheLeftAndDown.x(), shoot.position().x());
    ASSERT_EQ(positionToTheLeftAndDown.y(), shoot.position().y());

    shoot.moveUp();
    shoot.moveRight();
    ASSERT_EQ(position.x(), shoot.position().x());
    ASSERT_EQ(position.y(), shoot.position().y());
#endif
}
