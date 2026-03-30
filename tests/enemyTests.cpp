#include <iostream>
#include <type_traits>
#include <gtest/gtest.h>

#include "../enemy.h"


using namespace std;
using namespace ::testing;


struct EnemyTester : public ::testing::Test
{
};

TEST_F(EnemyTester, enemyHasLimitedLife)
{
#ifdef UNIMPLEMENTED_enemy
    ADD_FAILURE() << "Not implemented UNIMPLEMENTED_enemy";
#else
    const Position position(5, 6);

    constexpr int enemyLife = 10;

    Enemy enemy(position, enemyLife);
    ASSERT_TRUE(enemy.isAlive());
    ASSERT_EQ(100.f, enemy.lifePercent());

    enemy.decreaseLife(enemyLife);
    ASSERT_FALSE(enemy.isAlive());
#endif
}
