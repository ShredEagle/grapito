#include "LevelStacks.h"

namespace ad {
namespace grapito {

const float gLevelHalfWidth = 40.f;
const float gWallWidth = 4.f;
const float gJumpHeight = 3.6f;
const float gPlatformHeight = 1.2f;
const float gStackHeight = gJumpHeight * 15.f;
const float gSmallAnchorSize = 1.5f;
const float gBigAnchorSize = 2.f;

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

std::vector<aunteater::weak_entity> 
createStackOne(aunteater::EntityManager & aEntityManager, float baseHeight)
{
    std::vector<aunteater::weak_entity> segment;

    segment.push_back(aEntityManager.addEntity(makeWall({-gLevelHalfWidth - gWallWidth, 0.f + baseHeight})));
    segment.push_back(aEntityManager.addEntity(makeWall({gLevelHalfWidth, 0.f + baseHeight})));

    segment.push_back(aEntityManager.addEntity(makeAnchor({-15.f, gJumpHeight * 4 - gPlatformHeight + baseHeight}, {30.f, gPlatformHeight})));

    segment.push_back(aEntityManager.addEntity(makeAnchor({-30.f, gJumpHeight * 8 - gPlatformHeight + baseHeight}, {10.f, gPlatformHeight})));
    segment.push_back(aEntityManager.addEntity(makeAnchor({20.f, gJumpHeight * 8 - gPlatformHeight + baseHeight}, {10.f, gPlatformHeight})));

    segment.push_back(aEntityManager.addEntity(makeAnchor(
        {-2.5f, gJumpHeight * 11 - gWallWidth + baseHeight}, 
        {
            {0.f, 0.f},
            {2.5f, 0.f},
            {5.f, 2.5f},
            {5.f, 4.f},
            {-2.5f, 4.f},
            {-2.5f, 2.5f}
        }
    )));

    segment.push_back(aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth, gJumpHeight * 13 - gWallWidth + baseHeight}, {20.f, gWallWidth})));
    segment.push_back(aEntityManager.addEntity(makeAnchor({gLevelHalfWidth - 20.f, gJumpHeight * 13 - gWallWidth + baseHeight}, {20.f, gWallWidth})));

    segment.push_back(aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth + 7.f, gJumpHeight * 15 - gPlatformHeight + baseHeight}, {gLevelHalfWidth * 2 - 14.f, gPlatformHeight})));

    return segment;
}

std::vector<aunteater::weak_entity> 
createStackTwo(aunteater::EntityManager & aEntityManager, float baseHeight)
{
    std::vector<aunteater::weak_entity> segment;

    segment.push_back(aEntityManager.addEntity(makeWall({-gLevelHalfWidth - gWallWidth, 0.f + baseHeight})));
    segment.push_back(aEntityManager.addEntity(makeWall({gLevelHalfWidth, 0.f + baseHeight})));

    segment.push_back(aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth + 10.f, gJumpHeight * 3 - gPlatformHeight + baseHeight}, {gLevelHalfWidth * 2 - 20.f, gPlatformHeight})));

    segment.push_back(aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth + 14.f, gJumpHeight * 3 + baseHeight}, {gPlatformHeight, gJumpHeight * 9.f})));
    segment.push_back(aEntityManager.addEntity(makeAnchor({gLevelHalfWidth - 14.f - gPlatformHeight, gJumpHeight * 3 + baseHeight}, {gPlatformHeight, gJumpHeight * 9.f})));

    segment.push_back(aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth + 22.f, gJumpHeight * 6 + baseHeight}, {gPlatformHeight, gJumpHeight * 9.f})));
    segment.push_back(aEntityManager.addEntity(makeAnchor({gLevelHalfWidth - 22.f - gPlatformHeight, gJumpHeight * 6 + baseHeight}, {gPlatformHeight, gJumpHeight * 9.f})));

    segment.push_back(aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth, gJumpHeight * 15 - gPlatformHeight + baseHeight}, {22.f, gPlatformHeight})));
    segment.push_back(aEntityManager.addEntity(makeAnchor({gLevelHalfWidth - 22.f - gPlatformHeight, gJumpHeight * 15 - gPlatformHeight + baseHeight}, {22.f + gPlatformHeight, gPlatformHeight})));

    segment.push_back(aEntityManager.addEntity(makeGrappleAnchor({0.f, gJumpHeight * 12.f + baseHeight}, gBigAnchorSize)));

    return segment;
}

std::vector<aunteater::weak_entity> 
createStackThree(aunteater::EntityManager & aEntityManager, float baseHeight)
{
    std::vector<aunteater::weak_entity> segment;

    segment.push_back(aEntityManager.addEntity(makeWall({-gLevelHalfWidth - gWallWidth, 0.f + baseHeight})));
    segment.push_back(aEntityManager.addEntity(makeWall({gLevelHalfWidth, 0.f + baseHeight})));

    segment.push_back(aEntityManager.addEntity(makeGrappleAnchor({-9.f, gJumpHeight * 3 + baseHeight}, gSmallAnchorSize)));
    segment.push_back(aEntityManager.addEntity(makeGrappleAnchor({9.f, gJumpHeight * 3 + baseHeight}, gSmallAnchorSize)));

    segment.push_back(aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth, gJumpHeight * 5 - gPlatformHeight + baseHeight}, {10.f, gPlatformHeight})));
    segment.push_back(aEntityManager.addEntity(makeAnchor({gLevelHalfWidth - 10.f, gJumpHeight * 5 - gPlatformHeight + baseHeight}, {10.f, gPlatformHeight})));

    segment.push_back(aEntityManager.addEntity(makeGrappleAnchor({-9.f, gJumpHeight * 9 + baseHeight}, gSmallAnchorSize)));
    segment.push_back(aEntityManager.addEntity(makeGrappleAnchor({9.f, gJumpHeight * 9 + baseHeight}, gSmallAnchorSize)));

    segment.push_back(aEntityManager.addEntity(makeGrappleAnchor({-17.f, gJumpHeight * 13 + baseHeight}, gSmallAnchorSize)));
    segment.push_back(aEntityManager.addEntity(makeGrappleAnchor({17.f, gJumpHeight * 13 + baseHeight}, gSmallAnchorSize)));

    segment.push_back(aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth + 10.f, gJumpHeight * 15 - gPlatformHeight + baseHeight}, {gLevelHalfWidth * 2 - 20.f, gPlatformHeight})));

    return segment;
}

std::vector<aunteater::weak_entity> 
createStackFour(aunteater::EntityManager & aEntityManager, float baseHeight)
{
    std::vector<aunteater::weak_entity> segment;

    segment.push_back(aEntityManager.addEntity(makeWall({-gLevelHalfWidth - gWallWidth, 0.f + baseHeight})));
    segment.push_back(aEntityManager.addEntity(makeWall({gLevelHalfWidth, 0.f + baseHeight})));


    segment.push_back(aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth + 12.f, gJumpHeight * 3.f + baseHeight}, {gPlatformHeight, gJumpHeight * 7.f})));
    segment.push_back(aEntityManager.addEntity(makeAnchor({gLevelHalfWidth - 12.f - gPlatformHeight, gJumpHeight * 3.f + baseHeight}, {gPlatformHeight, gJumpHeight * 7.f})));

    segment.push_back(aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth + 12.f, gJumpHeight * 10.f + baseHeight}, {5.f, gPlatformHeight})));
    segment.push_back(aEntityManager.addEntity(makeAnchor({gLevelHalfWidth - 12.f - 5.f, gJumpHeight * 10.f + baseHeight}, {5.f, gPlatformHeight})));

    segment.push_back(aEntityManager.addEntity(makeGrappleAnchor({-9.f, gJumpHeight * 9 + baseHeight}, gSmallAnchorSize)));
    segment.push_back(aEntityManager.addEntity(makeGrappleAnchor({9.f, gJumpHeight * 9 + baseHeight}, gSmallAnchorSize)));

    segment.push_back(aEntityManager.addEntity(makeAnchor({-12.f, gJumpHeight * 13.f + baseHeight}, {gPlatformHeight, gJumpHeight * 2.f})));
    segment.push_back(aEntityManager.addEntity(makeAnchor({12.f - gPlatformHeight, gJumpHeight * 13.f + baseHeight}, {gPlatformHeight, gJumpHeight * 2.f})));

    segment.push_back(aEntityManager.addEntity(makeAnchor({-12.f + gPlatformHeight, gJumpHeight * 13.f + baseHeight}, {5.f, gPlatformHeight})));
    segment.push_back(aEntityManager.addEntity(makeAnchor({12.f - 5.f, gJumpHeight * 13.f + baseHeight}, {5.f, gPlatformHeight})));

    segment.push_back(aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth, gJumpHeight * 15 - gPlatformHeight + baseHeight}, {gLevelHalfWidth - 12.f, gPlatformHeight})));
    segment.push_back(aEntityManager.addEntity(makeAnchor({12.f, gJumpHeight * 15 - gPlatformHeight + baseHeight}, {gLevelHalfWidth - 12.f, gPlatformHeight})));

    return segment;
}

std::vector<aunteater::weak_entity> 
createStackFive(aunteater::EntityManager & aEntityManager, float baseHeight)
{
    std::vector<aunteater::weak_entity> segment;

    segment.push_back(aEntityManager.addEntity(makeWall({-gLevelHalfWidth - gWallWidth, 0.f + baseHeight})));
    segment.push_back(aEntityManager.addEntity(makeWall({gLevelHalfWidth, 0.f + baseHeight})));

    segment.push_back(aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth, gJumpHeight * 3.f - gPlatformHeight + baseHeight}, {28.f, gPlatformHeight})));
    segment.push_back(aEntityManager.addEntity(makeAnchor({gLevelHalfWidth - 28.f, gJumpHeight * 3.f - gPlatformHeight + baseHeight}, {28.f, gPlatformHeight})));

    segment.push_back(aEntityManager.addEntity(makeGrappleAnchor({0.f, gJumpHeight * 3.f + baseHeight}, gSmallAnchorSize)));

    segment.push_back(aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth + 28.f - gPlatformHeight, gJumpHeight * 3.f + baseHeight}, {gPlatformHeight, gJumpHeight * 9.f})));
    segment.push_back(aEntityManager.addEntity(makeAnchor({gLevelHalfWidth - 28.f, gJumpHeight * 3.f + baseHeight}, {gPlatformHeight, gJumpHeight * 9.f})));

    segment.push_back(aEntityManager.addEntity(makeAnchor({-3.f - gPlatformHeight / 2, gJumpHeight * 7.f - gPlatformHeight + baseHeight}, {6.f + gPlatformHeight, gPlatformHeight})));

    segment.push_back(aEntityManager.addEntity(makeAnchor({0.f - gPlatformHeight / 2, gJumpHeight * 7.f + baseHeight}, {gPlatformHeight, gJumpHeight * 8.f})));

    segment.push_back(aEntityManager.addEntity(makeGrappleAnchor({-gLevelHalfWidth + 14.f - gSmallAnchorSize / 2, gJumpHeight * 10.f + baseHeight}, gSmallAnchorSize)));
    segment.push_back(aEntityManager.addEntity(makeGrappleAnchor({gLevelHalfWidth - 14 - gSmallAnchorSize / 2, gJumpHeight * 10.f + baseHeight}, gSmallAnchorSize)));

    segment.push_back(aEntityManager.addEntity(makeAnchor({-gLevelHalfWidth + 10.f, gJumpHeight * 15 - gPlatformHeight + baseHeight}, {gLevelHalfWidth * 2 - 20.f, gPlatformHeight})));

    return segment;
}

}
}
