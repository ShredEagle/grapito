#include "CameraGuidedControl.h"

#include "../Configuration.h"

#include "../Utils/DrawDebugStuff.h"


namespace ad {
namespace grapito {

CameraGuidedControl::CameraGuidedControl(aunteater::EntityManager & aEntityManager) :
    mCameras{aEntityManager},
    mCameraPoints{aEntityManager}
{}


void CameraGuidedControl::update(const GrapitoTimer aTimer, const GameInputState &)
{
    math::Vec<2, float> accumulatedPosition = math::Vec<2, float>::Zero();
    float totalInfluence = 0.;
    for (auto cameraPoint : mCameraPoints)
    {
        auto & [cameraGuide, geometry] = cameraPoint;

        if (cameraGuide.influenceInterpolation) 
        {
            cameraGuide.influence = cameraGuide.influenceInterpolation->advance(aTimer.delta());
            if (cameraGuide.completionBehaviour == CameraGuide::OnCompletion::Remove
                && cameraGuide.influenceInterpolation->isCompleted())
            {
                cameraPoint->markComponentToRemove<CameraGuide>();
            }
        }
        accumulatedPosition += geometry.position.as<math::Vec>() * cameraGuide.influence;
        totalInfluence += cameraGuide.influence;
    }

    // We know there is only one camera, but this allows to treat the camera an any other entity.
    for(const auto camera : mCameras)
    {
        camera->get<Position>().position = (accumulatedPosition / totalInfluence).as<math::Position>() ;
        debugDrawer->drawCross({camera->get<Position>().position, debug::gCrossSize, math::sdr::gMagenta});
    }
}

} // namespace grapito
} // namespace ad
