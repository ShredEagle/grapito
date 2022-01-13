#include "LevelStacks.h"

namespace ad {
namespace grapito {

constexpr float gLevelHalfWidth = 40.f;
constexpr float gWallWidth = 4.f;
constexpr float gJumpHeight = 3.6f;
constexpr float gPlatformHeight = 0.8f;
constexpr float gStackHeight = gJumpHeight * 15.f;

aunteater::Entity makeWall(Position2 aPos)
{
    return makeAnchor(aPos, math::Size<2, float>{gWallWidth, gStackHeight});
}

void createStackOne(aunteater::EntityManager & aEntityManager, float baseHeight)
{
    aEntityManager.addEntity(makeWall({-gLevelHalfWidth - gWallWidth, 0.f + baseHeight}));
    aEntityManager.addEntity(makeWall({gLevelHalfWidth, 0.f + baseHeight}));

    aEntityManager.addEntity(makeAnchor({-15.f, gJumpHeight * 4 - gPlatformHeight + baseHeight}, {30.f, gPlatformHeight}));

    aEntityManager.addEntity(makeAnchor({-30.f, gJumpHeight * 8 - gPlatformHeight + baseHeight}, {10.f, gPlatformHeight}));
    aEntityManager.addEntity(makeAnchor({20.f, gJumpHeight * 8 - gPlatformHeight + baseHeight}, {10.f, gPlatformHeight}));

    aEntityManager.addEntity(makeAnchor(
        {-2.5f, gJumpHeight * 11 - gWallWidth}, 
        {
            {0.f, 0.f},
            {2.5f, 0.f},
            {5.f, 2.5f},
            {5.f, 4.f},
            {-2.5f, 4.f},
            {-2.5f, 2.5f}
        }
    ));

    aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth, gJumpHeight * 13 - gWallWidth + baseHeight}, {20.f, gWallWidth}));
    aEntityManager.addEntity(makeAnchor({gLevelHalfWidth - 20.f, gJumpHeight * 13 - gWallWidth + baseHeight}, {20.f, gWallWidth}));

    aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth + 7.f, gJumpHeight * 15 - gPlatformHeight + baseHeight}, {gLevelHalfWidth * 2 - 14.f, gPlatformHeight}));

}

void createStackTwo(aunteater::EntityManager & aEntityManager, float baseHeight)
{
    aEntityManager.addEntity(makeWall({-gLevelHalfWidth - gWallWidth, 0.f + baseHeight}));
    aEntityManager.addEntity(makeWall({gLevelHalfWidth, 0.f + baseHeight}));

    aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth + 10.f, gJumpHeight * 3 - gPlatformHeight + baseHeight}, {gLevelHalfWidth * 2 - 20.f, gPlatformHeight}));

    aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth + 14.f, gJumpHeight * 3 + baseHeight}, {gPlatformHeight, gJumpHeight * 9.f}));
    aEntityManager.addEntity(makeAnchor({gLevelHalfWidth - 14.f - gPlatformHeight, gJumpHeight * 3 + baseHeight}, {gPlatformHeight, gJumpHeight * 9.f}));

    aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth + 22.f, gJumpHeight * 6 + baseHeight}, {gPlatformHeight, gJumpHeight * 9.f}));
    aEntityManager.addEntity(makeAnchor({gLevelHalfWidth - 22.f - gPlatformHeight, gJumpHeight * 6 + baseHeight}, {gPlatformHeight, gJumpHeight * 9.f}));

    aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth, gJumpHeight * 15 - gPlatformHeight + baseHeight}, {22.f, gPlatformHeight}));
    aEntityManager.addEntity(makeAnchor({gLevelHalfWidth - 22.f - gPlatformHeight, gJumpHeight * 15 - gPlatformHeight + baseHeight}, {22.f + gPlatformHeight, gPlatformHeight}));
}

void createStackThree(aunteater::EntityManager & aEntityManager, float baseHeight)
{
    aEntityManager.addEntity(makeWall({-gLevelHalfWidth - gWallWidth, 0.f + baseHeight}));
    aEntityManager.addEntity(makeWall({gLevelHalfWidth, 0.f + baseHeight}));

    aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth + 10.f, gJumpHeight * 3 - gPlatformHeight + baseHeight}, {gLevelHalfWidth * 2 - 20.f, gPlatformHeight}));

    aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth + 14.f, gJumpHeight * 3 + baseHeight}, {gPlatformHeight, gJumpHeight * 9.f}));
    aEntityManager.addEntity(makeAnchor({gLevelHalfWidth - 14.f - gPlatformHeight, gJumpHeight * 3 + baseHeight}, {gPlatformHeight, gJumpHeight * 9.f}));

    aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth + 22.f, gJumpHeight * 6 + baseHeight}, {gPlatformHeight, gJumpHeight * 9.f}));
    aEntityManager.addEntity(makeAnchor({gLevelHalfWidth - 22.f - gPlatformHeight, gJumpHeight * 6 + baseHeight}, {gPlatformHeight, gJumpHeight * 9.f}));

    aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth, gJumpHeight * 15 - gPlatformHeight + baseHeight}, {22.f, gPlatformHeight}));
    aEntityManager.addEntity(makeAnchor({gLevelHalfWidth - 22.f - gPlatformHeight, gJumpHeight * 15 - gPlatformHeight + baseHeight}, {22.f + gPlatformHeight, gPlatformHeight}));
}

}
}
