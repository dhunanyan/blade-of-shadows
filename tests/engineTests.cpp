#include <iostream>
#include <type_traits>
#include <gtest/gtest.h>

#include "../engine.h"
#include "../position.h"
#include "../bullet.h"
#include "../enemy.h"

using namespace std;
using namespace ::testing;


struct EngineTester : public ::testing::Test
{
    static constexpr size_t width = 20;
    static constexpr size_t height = 10;
};

struct EngineUnderTest: public Engine
{
    using Engine::Engine;
    using Engine::randEnemies;
};

TEST_F(EngineTester, engineConstructedWithWidthAndHeight)
{
#ifdef UNIMPLEMENTED_engineConstructedWithWidthAndHeight
    ADD_FAILURE() << "Not implemented: UNIMPLEMENTED_engineConstructedWithWidthAndHeight";
#else
    EngineUnderTest engine(width, height);

    ASSERT_EQ(width, engine.stageWidthCells());
    ASSERT_EQ(height, engine.stageHeightCells());
#endif
}

TEST_F(EngineTester, engineHasGettersToGetInformationAboutPlayer)
{
#ifdef UNIMPLEMENTED_engineHasGettersToGetInformationAboutPlayer
    ADD_FAILURE() << "Not implemented: UNIMPLEMENTED_engineHasGettersToGetInformationAboutPlayer";
#else
    EngineUnderTest engine(width, height);

    const Position expectedPlayerPosition(width/2, height/2);
    const Direction expectedPlayerDirection = Direction::RIGHT;

    ASSERT_EQ(expectedPlayerPosition.x(), engine.playerPosition().x());
    ASSERT_EQ(expectedPlayerPosition.y(), engine.playerPosition().y());
    ASSERT_EQ(expectedPlayerDirection, engine.playerDirection());
    ASSERT_TRUE(engine.isPlayerAlive());
#endif
}

TEST_F(EngineTester, engineCanMovePlayer)
{
#ifdef UNIMPLEMENTED_engineCanMovePlayer
    ADD_FAILURE() << "Not implemented: UNIMPLEMENTED_engineCanMovePlayer";
#else
    EngineUnderTest engine(width, height);

    const Position playerDefaultPosition(width/2, height/2);
    const Direction playerDefaultDirection = Direction::RIGHT;

    const Position playerAfterMovingRight(playerDefaultPosition.x()+1, playerDefaultPosition.y());
    engine.movePlayerRight();
    ASSERT_EQ(playerAfterMovingRight.x(), engine.playerPosition().x());
    ASSERT_EQ(playerAfterMovingRight.y(), engine.playerPosition().y());
    ASSERT_EQ(playerDefaultDirection, engine.playerDirection());

    const Position playerAfterMovingUp(playerAfterMovingRight.x(), playerAfterMovingRight.y()+1);
    engine.movePlayerUp();
    ASSERT_EQ(playerAfterMovingUp.x(), engine.playerPosition().x());
    ASSERT_EQ(playerAfterMovingUp.y(), engine.playerPosition().y());
#endif
}

TEST_F(EngineTester, engineHandlePlayerShooting)
{
#ifdef UNIMPLEMENTED_engineHandlePlayerShooting
    ADD_FAILURE() << "Not implemented: UNIMPLEMENTED_engineHandlePlayerShooting";
#else
    EngineUnderTest engine(width, height);

    const Direction playerDefaultDirection = Direction::RIGHT;
    const Position playerDefaultPosition(width/2, height/2);
    const Position shootPosition(playerDefaultPosition.x()+1, playerDefaultPosition.y());

    ASSERT_EQ(0, engine.bullets().size());

    engine.playerShoots();

    const auto& shoots = engine.bullets();
    ASSERT_EQ(1, shoots.size());
    ASSERT_EQ(playerDefaultDirection, shoots[0].direction());
    ASSERT_EQ(shootPosition.x(), shoots[0].position().x());
    ASSERT_EQ(shootPosition.y(), shoots[0].position().y());
#endif
}

struct EnemyRandomPositionGenerator
{
    inline static unsigned startingX=0, startingY=0, generatedEnemies={};
    static Position getNextPosition(int, int)
    {
        ++generatedEnemies;
        return Position{startingX++, startingY++};
    }
};

TEST_F(EngineTester, engineHandlesEnemies)
{
#ifdef UNIMPLEMENTED_engineHandlesEnemies
    ADD_FAILURE() << "Not implemented: UNIMPLEMENTED_engineHandlesEnemies";
#else
    EngineUnderTest engine(width, height);

    ASSERT_EQ(0, engine.enemies().size());

    engine.randEnemies(&EnemyRandomPositionGenerator::getNextPosition);

    const auto enemies = engine.enemies();
    ASSERT_EQ(EnemyRandomPositionGenerator::generatedEnemies, enemies.size());
    for (unsigned i=0; i < enemies.size(); ++i)
    {
        ASSERT_TRUE(enemies[i]->isAlive());
        ASSERT_EQ(i, enemies[i]->position().x()) << "i=" << i;
        ASSERT_EQ(i, enemies[i]->position().y()) << "i=" << i;
    }
#endif
}
