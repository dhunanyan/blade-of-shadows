#include <iostream>
#include <type_traits>
#include <gtest/gtest.h>

#include "game/core/engine.h"
#include "game/core/position.h"
#include "game/core/enemy.h"

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
    engine.setSolidQuery([&](int, int y) {
        return y >= static_cast<int>(height / 2) + 1;
    });

    const float defaultX = engine.playerPixelX();
    const float defaultY = engine.playerPixelY();

    engine.setPlayerMoveIntentX(1);
    for (int i = 0; i < 6; ++i)
    {
        engine.update();
    }
    ASSERT_GT(engine.playerPixelX(), defaultX);
    ASSERT_EQ(Direction::RIGHT, engine.playerDirection());

    engine.setPlayerMoveIntentX(-1);
    const float movedRightX = engine.playerPixelX();
    for (int i = 0; i < 6; ++i)
    {
        engine.update();
    }
    ASSERT_LT(engine.playerPixelX(), movedRightX);
    ASSERT_EQ(Direction::LEFT, engine.playerDirection());
    ASSERT_EQ(defaultY, engine.playerPixelY());
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
