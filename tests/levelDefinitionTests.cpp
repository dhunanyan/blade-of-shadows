#include <gtest/gtest.h>

#include "game/core/level_definition.h"

TEST(LevelDefinitionTester, resolvesStandingSpawnUsingWholePlayerBody)
{
    LevelDefinition level(10, 10, 24);
    for (int x = 0; x < level.width(); ++x)
    {
        level.setSolid(Position(x, 6), true);
    }

    const Position spawn = level.resolveStandingSpawn(
        Position(2, 5),
        WorldSize{16.0f, 32.0f});
    const WorldPoint world = level.worldPositionForStandingCell(
        spawn,
        WorldSize{16.0f, 32.0f});

    EXPECT_EQ(Position(2, 5), spawn);
    EXPECT_FLOAT_EQ(52.0f, world.x);
    EXPECT_FLOAT_EQ(112.0f, world.y);
}

TEST(LevelDefinitionTester, relocatesSpawnWhenBodyOverlapsSolidTile)
{
    LevelDefinition level(10, 10, 24);
    for (int x = 0; x < level.width(); ++x)
    {
        level.setSolid(Position(x, 4), true);
        level.setSolid(Position(x, 6), true);
    }

    EXPECT_EQ(
        Position(2, 3),
        level.resolveStandingSpawn(
            Position(2, 5),
            WorldSize{16.0f, 32.0f}));
}

TEST(LevelDefinitionTester, outsideCellsAreSolidWorldBoundaries)
{
    const LevelDefinition level(10, 10, 24);

    EXPECT_TRUE(level.isSolid(-1, 0));
    EXPECT_TRUE(level.isSolid(10, 0));
    EXPECT_TRUE(level.isSolid(0, -1));
    EXPECT_TRUE(level.isSolid(0, 10));
}
