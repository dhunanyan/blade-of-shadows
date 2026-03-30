#include <iostream>
#include <type_traits>
#include <gtest/gtest.h>

#include "../object.h"

using namespace std;
using namespace ::testing;


struct ObjectTester : public ::testing::Test
{
};

TEST_F(ObjectTester, constructorWithTwoArguments)
{
#ifdef UNIMPLEMENTED_objectWithConstructorAndGetters
    ADD_FAILURE() << "Not implemented: UNIMPLEMENTED_objectWithConstructorAndGetters";
#else
    const Position position(10, 13);
    const ObjectType objectType = ObjectType::OBJECT_PLAYER;

    Object object(objectType, position);

    ASSERT_EQ(objectType, object.type());
    ASSERT_EQ(position.x(), object.position().x());
    ASSERT_EQ(position.y(), object.position().y());
#endif
}

TEST_F(ObjectTester, setPosition)
{
#if defined(UNIMPLEMENTED_objectWithConstructorAndGetters) || defined(UNIMPLEMENTED_objectSetPosition)
    ADD_FAILURE() << "Not implemented: UNIMPLEMENTED_objectSetPosition";
#else
    const Position position1(10, 13), position2(3, 1);
    const ObjectType objectType = ObjectType::OBJECT_PLAYER;

    Object object(objectType);

    object.setPosition(position1);
    ASSERT_EQ(position1.x(), object.position().x());
    ASSERT_EQ(position1.y(), object.position().y());

    object.setPosition(position2);
    ASSERT_EQ(position2.x(), object.position().x());
    ASSERT_EQ(position2.y(), object.position().y());
#endif
}

TEST_F(ObjectTester, movingObject)
{
#if defined(UNIMPLEMENTED_objectWithConstructorAndGetters) || defined(UNIMPLEMENTED_movingObject)
    ADD_FAILURE() << "Not implemented: UNIMPLEMENTED_movingObject";
#else
    const Position position(10, 13);
    const Position positionAfterMovingUp(position.x(), position.y()+1);
    const Position positionAfterMovingLeft(position.x()-1, position.y());
    const ObjectType objectType = ObjectType::OBJECT_PLAYER;

    Object object(objectType, position);

    object.moveUp();
    ASSERT_EQ(positionAfterMovingUp.x(), object.position().x());
    ASSERT_EQ(positionAfterMovingUp.y(), object.position().y());

    object.moveDown();
    ASSERT_EQ(position.x(), object.position().x());
    ASSERT_EQ(position.y(), object.position().y());

    object.moveLeft();
    ASSERT_EQ(positionAfterMovingLeft.x(), object.position().x());
    ASSERT_EQ(positionAfterMovingLeft.y(), object.position().y());

    object.moveRight();
    ASSERT_EQ(position.x(), object.position().x());
    ASSERT_EQ(position.y(), object.position().y());
#endif
}
