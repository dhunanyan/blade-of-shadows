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
    inline static int startingX = 0;
    inline static int startingY = 0;
    inline static unsigned generatedEnemies = 0;
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

TEST_F(EngineTester, sessionLoadsDeterministicEntitiesAndCollectsCoins)
{
    EngineUnderTest engine(width, height);
    engine.setSolidQuery([&](int, int y) {
        return y >= 6;
    });

    const Position spawn(2, 5);
    engine.resetSession(
        spawn,
        {Position(8, 5)},
        {spawn, Position(4, 5)},
        Position(9, 5));

    ASSERT_EQ(1, engine.enemies().size());
    ASSERT_EQ(2, engine.coins().size());

    engine.update();

    EXPECT_EQ(1, engine.playerCoins());
    EXPECT_EQ(10, engine.playerScore());
    EXPECT_EQ(1, engine.coins().size());
    EXPECT_FALSE(engine.isLevelComplete());
}

TEST_F(EngineTester, attackDamagesEnemyOnlyOncePerSwing)
{
    EngineUnderTest engine(width, height);
    engine.setSolidQuery([&](int, int y) {
        return y >= 6;
    });
    engine.resetSession(
        Position(2, 5),
        {Position(3, 5)},
        {},
        std::nullopt);
    engine.setPlayerDirection(Direction::RIGHT);
    engine.requestPlayerAttack();

    for (int tick = 0; tick < 12; ++tick)
    {
        engine.update();
    }

    ASSERT_EQ(1, engine.enemies().size());
    EXPECT_FLOAT_EQ(50.0f, engine.enemies().front()->life());
}

TEST_F(EngineTester, enemyContactDamagesPlayerAndAppliesCooldown)
{
    EngineUnderTest engine(width, height);
    engine.setSolidQuery([&](int, int y) {
        return y >= 6;
    });
    engine.resetSession(
        Position(2, 5),
        {Position(2, 5)},
        {},
        std::nullopt);

    engine.update();
    EXPECT_EQ(4, engine.playerHealth());

    engine.update();
    EXPECT_EQ(4, engine.playerHealth());
}

TEST_F(EngineTester, snapshotRestoresProgressAndWorldState)
{
    EngineUnderTest engine(width, height);
    engine.setSolidQuery([&](int, int y) {
        return y >= 6;
    });
    engine.resetSession(
        Position(2, 5),
        {Position(8, 5)},
        {Position(4, 5)},
        Position(9, 5));

    Engine::Snapshot state = engine.snapshot();
    state.playerHealth = 3;
    state.coins = 7;
    state.score = 420;
    engine.restoreSnapshot(state);

    EXPECT_EQ(3, engine.playerHealth());
    EXPECT_EQ(7, engine.playerCoins());
    EXPECT_EQ(420, engine.playerScore());
    EXPECT_EQ(1, engine.coins().size());
    EXPECT_EQ(1, engine.enemies().size());
}

TEST_F(EngineTester, invalidPlayerPositionIsRejected)
{
    EngineUnderTest engine(width, height);
    const Position original = engine.playerPosition();

    engine.setPlayerPosition(Position(-1, 2));
    EXPECT_EQ(original, engine.playerPosition());
}

TEST_F(EngineTester, dodgeMovesPlayerAndPreventsContactDamage)
{
    EngineUnderTest engine(width, height);
    engine.setSolidQuery([&](int, int y) {
        return y >= 6;
    });
    engine.resetSession(
        Position(2, 5),
        {Position(2, 5)},
        {},
        std::nullopt);

    const float originalX = engine.playerPixelX();
    engine.requestPlayerDodge();
    engine.update();

    EXPECT_TRUE(engine.isPlayerDodging());
    EXPECT_GT(engine.playerPixelX(), originalX);
    EXPECT_EQ(engine.playerMaxHealth(), engine.playerHealth());
}

TEST_F(EngineTester, jumpStopsAtSolidCeiling)
{
    EngineUnderTest engine(width, height);
    engine.setSolidQuery([&](int, int y) {
        return y == 4 || y >= 6;
    });
    engine.resetSession(Position(2, 5), {}, {}, std::nullopt);

    const float originalY = engine.playerPixelY();
    engine.requestPlayerJump();
    engine.update();

    EXPECT_FLOAT_EQ(originalY, engine.playerPixelY());
    EXPECT_FLOAT_EQ(0.0f, engine.playerVelocityY());
}
