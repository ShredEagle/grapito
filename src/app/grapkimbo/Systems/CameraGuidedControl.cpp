#include "CameraGuidedControl.h"


namespace ad {
namespace grapito {

CameraGuidedControl::CameraGuidedControl(aunteater::EntityManager & aEntityManager) :
    mCameras{aEntityManager},
    mCameraPoints{aEntityManager}
{}


void CameraGuidedControl::update(const aunteater::Timer aTimer, const GameInputState & aInputState)
{
    math::Vec<2, double> accumulatedPosition = math::Vec<2, double>::Zero();
    double totalInfluence = 0.;
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

    for(const auto camera : mCameras)
    {
        camera->get<Position>().position = (accumulatedPosition / totalInfluence).as<math::Position>() ;
    }
}

} // namespace grapito
} // namespace ad