#include "LevelStacks.h"

namespace ad {
namespace grapito {

constexpr float gLevelHalfWidth = 40.f;
constexpr float gWallWidth = 4.f;
constexpr float gJumpHeight = 3.6f;
constexpr float gPlatformHeight = 1.2f;
constexpr float gStackHeight = gJumpHeight * 15.f;
constexpr float gSmallAnchorSize = 1.5f;
constexpr float gBigAnchorSize = 2.f;

aunteater::Entity makeWall(Position2 aPos)
{
    return makeAnchor(aPos, math::Size<2, float>{gWallWidth, gStackHeight});
}

aunteater::Entity makeGrappleAnchor(Position2 aPos, float sideSize)
{
    return makeAnchor(aPos, {
        {0.f, 0.f},
        {1.f * sideSize, 0.f},
        {1.5f * sideSize, 0.875f * sideSize},
        {1.f * sideSize, 1.75f * sideSize},
        {0.f, 1.75f * sideSize},
        {-0.5f * sideSize, 0.875f * sideSize}
    });
}

void createStackOne(aunteater::EntityManager & aEntityManager, float baseHeight)
{
    aEntityManager.addEntity(makeWall({-gLevelHalfWidth - gWallWidth, 0.f + baseHeight}));
    aEntityManager.addEntity(makeWall({gLevelHalfWidth, 0.f + baseHeight}));

    aEntityManager.addEntity(makeAnchor({-15.f, gJumpHeight * 4 - gPlatformHeight + baseHeight}, {30.f, gPlatformHeight}));

    aEntityManager.addEntity(makeAnchor({-30.f, gJumpHeight * 8 - gPlatformHeight + baseHeight}, {10.f, gPlatformHeight}));
    aEntityManager.addEntity(makeAnchor({20.f, gJumpHeight * 8 - gPlatformHeight + baseHeight}, {10.f, gPlatformHeight}));

    aEntityManager.addEntity(makeAnchor(
        {-2.5f, gJumpHeight * 11 - gWallWidth + baseHeight}, 
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

    aEntityManager.addEntity(makeGrappleAnchor({0.f, gJumpHeight * 12.f + baseHeight}, 1.f));
}

void createStackThree(aunteater::EntityManager & aEntityManager, float baseHeight)
{
    aEntityManager.addEntity(makeWall({-gLevelHalfWidth - gWallWidth, 0.f + baseHeight}));
    aEntityManager.addEntity(makeWall({gLevelHalfWidth, 0.f + baseHeight}));

    aEntityManager.addEntity(makeGrappleAnchor({-9.f, gJumpHeight * 3 + baseHeight}, gSmallAnchorSize));
    aEntityManager.addEntity(makeGrappleAnchor({9.f, gJumpHeight * 3 + baseHeight}, gSmallAnchorSize));

    aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth, gJumpHeight * 5 - gPlatformHeight + baseHeight}, {10.f, gPlatformHeight}));
    aEntityManager.addEntity(makeAnchor({gLevelHalfWidth - 10.f, gJumpHeight * 5 - gPlatformHeight + baseHeight}, {10.f, gPlatformHeight}));

    aEntityManager.addEntity(makeGrappleAnchor({-9.f, gJumpHeight * 9 + baseHeight}, gSmallAnchorSize));
    aEntityManager.addEntity(makeGrappleAnchor({9.f, gJumpHeight * 9 + baseHeight}, gSmallAnchorSize));

    aEntityManager.addEntity(makeGrappleAnchor({-17.f, gJumpHeight * 13 + baseHeight}, gSmallAnchorSize));
    aEntityManager.addEntity(makeGrappleAnchor({17.f, gJumpHeight * 13 + baseHeight}, gSmallAnchorSize));

    aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth, gJumpHeight * 15 - gPlatformHeight + baseHeight}, {5.f, gPlatformHeight}));
    aEntityManager.addEntity(makeAnchor({gLevelHalfWidth - 5.f, gJumpHeight * 15 - gPlatformHeight + baseHeight}, {5.f, gPlatformHeight}));

    aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth + 10.f, gJumpHeight * 15 - gPlatformHeight + baseHeight}, {gLevelHalfWidth * 2 - 20.f, gPlatformHeight}));
}

void createStackFour(aunteater::EntityManager & aEntityManager, float baseHeight)
{
    aEntityManager.addEntity(makeWall({-gLevelHalfWidth - gWallWidth, 0.f + baseHeight}));
    aEntityManager.addEntity(makeWall({gLevelHalfWidth, 0.f + baseHeight}));


    aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth + 12.f, gJumpHeight * 3.f + baseHeight}, {gPlatformHeight, gJumpHeight * 7.f}));
    aEntityManager.addEntity(makeAnchor({gLevelHalfWidth - 12.f - gPlatformHeight, gJumpHeight * 3.f + baseHeight}, {gPlatformHeight, gJumpHeight * 7.f}));

    aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth + 12.f, gJumpHeight * 10.f + baseHeight}, {5.f, gPlatformHeight}));
    aEntityManager.addEntity(makeAnchor({gLevelHalfWidth - 12.f - 5.f, gJumpHeight * 10.f + baseHeight}, {5.f, gPlatformHeight}));

    aEntityManager.addEntity(makeGrappleAnchor({-9.f, gJumpHeight * 9 + baseHeight}, gSmallAnchorSize));
    aEntityManager.addEntity(makeGrappleAnchor({9.f, gJumpHeight * 9 + baseHeight}, gSmallAnchorSize));

    aEntityManager.addEntity(makeAnchor({-12.f, gJumpHeight * 13.f + baseHeight}, {gPlatformHeight, gJumpHeight * 2.f}));
    aEntityManager.addEntity(makeAnchor({12.f - gPlatformHeight, gJumpHeight * 13.f + baseHeight}, {gPlatformHeight, gJumpHeight * 2.f}));

    aEntityManager.addEntity(makeAnchor({-12.f + gPlatformHeight, gJumpHeight * 13.f + baseHeight}, {5.f, gPlatformHeight}));
    aEntityManager.addEntity(makeAnchor({12.f - 5.f, gJumpHeight * 13.f + baseHeight}, {5.f, gPlatformHeight}));

    aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth, gJumpHeight * 15 - gPlatformHeight + baseHeight}, {gLevelHalfWidth - 12.f, gPlatformHeight}));
    aEntityManager.addEntity(makeAnchor({12.f, gJumpHeight * 15 - gPlatformHeight + baseHeight}, {gLevelHalfWidth - 12.f, gPlatformHeight}));
}

void createStackFive(aunteater::EntityManager & aEntityManager, float baseHeight)
{
    aEntityManager.addEntity(makeWall({-gLevelHalfWidth - gWallWidth, 0.f + baseHeight}));
    aEntityManager.addEntity(makeWall({gLevelHalfWidth, 0.f + baseHeight}));

    aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth, gJumpHeight * 3.f - gPlatformHeight + baseHeight}, {28.f, gPlatformHeight}));
    aEntityManager.addEntity(makeAnchor({gLevelHalfWidth - 28.f, gJumpHeight * 3.f - gPlatformHeight + baseHeight}, {28.f, gPlatformHeight}));

    aEntityManager.addEntity(makeGrappleAnchor({0.f, gJumpHeight * 3.f + baseHeight}, gSmallAnchorSize));

    aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth + 28.f - gPlatformHeight, gJumpHeight * 3.f + baseHeight}, {gPlatformHeight, gJumpHeight * 9.f}));
    aEntityManager.addEntity(makeAnchor({gLevelHalfWidth - 28.f, gJumpHeight * 3.f + baseHeight}, {gPlatformHeight, gJumpHeight * 9.f}));

    aEntityManager.addEntity(makeAnchor({-3.f - gPlatformHeight / 2, gJumpHeight * 7.f - gPlatformHeight + baseHeight}, {6.f + gPlatformHeight, gPlatformHeight}));

    aEntityManager.addEntity(makeAnchor({0.f - gPlatformHeight / 2, gJumpHeight * 7.f + baseHeight}, {gPlatformHeight, gJumpHeight * 8.f}));

    aEntityManager.addEntity(makeGrappleAnchor({-gLevelHalfWidth + 14.f - gSmallAnchorSize / 2, gJumpHeight * 10.f + baseHeight}, gSmallAnchorSize));
    aEntityManager.addEntity(makeGrappleAnchor({gLevelHalfWidth - 14 - gSmallAnchorSize / 2, gJumpHeight * 10.f + baseHeight}, gSmallAnchorSize));

    aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth + 10.f, gJumpHeight * 15 - gPlatformHeight + baseHeight}, {gLevelHalfWidth * 2 - 20.f, gPlatformHeight}));
    aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth, gJumpHeight * 15 - gPlatformHeight + baseHeight}, {5.f, gPlatformHeight}));
    aEntityManager.addEntity(makeAnchor({gLevelHalfWidth - 5.f, gJumpHeight * 15 - gPlatformHeight + baseHeight}, {5.f, gPlatformHeight}));
}

}
}
