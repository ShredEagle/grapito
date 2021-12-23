#include "GameRule.h"

#include "../Configuration.h"
#include "../Entities.h"
#include "../Logging.h"

#include "../Utils/Camera.h"


namespace ad {
namespace grapito {

GameRule::GameRule(aunteater::EntityManager & aEntityManager) :
    mEntityManager{aEntityManager},
    mCompetitors{aEntityManager},
    mCameras{aEntityManager},
    mCameraPoints{aEntityManager}
{}


void GameRule::update(const GrapitoTimer, const GameInputState &)
{
    // There is the one camera
    Position2 cameraPosition = mCameras.begin()->get<Position>().position;

    for (auto competitor : mCompetitors)
    {
        auto & [cameraGuide, playerData, geometry] = competitor;            
        if (geometry.position.y() < (cameraPosition.y() - game::gCompetitorEliminationDistance))
        {
            ADLOG(info)("Player {} eliminated!", playerData.id);

            // Patch the camera guide:
            // A player has a CameraGuide, but also CameraLimits, and the limits override the position computed from the guide.
            // A fade-out-guide entity will be added to smoothly transition the camera once the competitor is removed, but 
            // it will not use CameraLimits. So we need to compute the equivalent guide position.
            Influence cameraInfluence = accumulateCameraGuides(mCameraPoints);
            Vec2 withoutEliminated = cameraInfluence.accumulatedPosition - weightedGuideContribution(geometry, cameraGuide);

            // The guide position that should be added to keep the camera position unchanged, after the competitor entity is removed.
            Position2 patchedGuidePosition = (cameraPosition * cameraInfluence.totalWeight - withoutEliminated) / cameraGuide.influence;
            // The fade-out-guide will be copied from the player CameraGuide: write the result there.
            // with the added benefit that if several competitors are elmininated in the same step, this will remain correct.
            cameraGuide.offset = patchedGuidePosition - geometry.position;

            // Remove the player from the game, and 
            // TODO This is a good example of circumventing the type system with entities:
            // provide a competitor where the archetype of a player is expected.
            kill(competitor, mEntityManager);
        }

    }
}


} // namespace grapitor
} // namespace ad
