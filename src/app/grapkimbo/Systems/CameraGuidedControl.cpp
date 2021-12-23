#include "CameraGuidedControl.h"

#include "../Configuration.h"

#include "../Utils/Camera.h"
#include "../Utils/DrawDebugStuff.h"


namespace ad {
namespace grapito {

CameraGuidedControl::CameraGuidedControl(aunteater::EntityManager & aEntityManager) :
    mCameras{aEntityManager},
    mCameraPoints{aEntityManager},
    mCameraLimiters{aEntityManager}
{}


Influence accumulateCameraGuides(const aunteater::FamilyHelp<CameraPoint> & aCameraPoints)
{
    Influence result{};
    for (auto & [cameraGuide, geometry] : aCameraPoints)
    {
        result.accumulatedPosition += weightedGuideContribution(geometry, cameraGuide);
        result.totalWeight += cameraGuide.influence;
    }
    return result;
}


Position2 placeCamera(Position2 aAveragePosition, float aLowerLimit, float aUpperLimit)
{
    // Important: this expects that the window ratio cannot change.
    math::Rectangle<GLfloat> viewed = 
        getViewedRectangle(aAveragePosition, math::getRatio<GLfloat>(game::gAppResolution));

    // Lower limit has lower priority, handle it first so it can be overriden
    if(viewed.yMin() > aLowerLimit)
    {
        viewed.y() -= (viewed.yMin() - aLowerLimit);
    }

    if(viewed.yMax() < aUpperLimit)
    {
        viewed.y() += (aUpperLimit - viewed.yMax());
    }

    return viewed.center();
}


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
            if (cameraGuide.completionBehaviour == CameraGuide::OnCompletion::RemoveComponent
                && cameraGuide.influenceInterpolation->isCompleted())
            {
                cameraPoint->markComponentToRemove<CameraGuide>();
            }
            else if (cameraGuide.completionBehaviour == CameraGuide::OnCompletion::RemoveEntity
                     && cameraGuide.influenceInterpolation->isCompleted())
            {
                cameraPoint->markToRemove();
            }
        }
        Position2 guidePosition = geometry.position + cameraGuide.offset;
        debugDrawer->drawCross({guidePosition, debug::gCrossSize, math::sdr::gBlue});

        accumulatedPosition += guidePosition.as<math::Vec>() * cameraGuide.influence;
        totalInfluence += cameraGuide.influence;
    }

    float lowerLimit = std::numeric_limits<float>::max();
    float upperLimit = std::numeric_limits<float>::lowest();
    for (const auto & [cameraLimits, geometry] : mCameraLimiters)
    {
        lowerLimit = std::min(lowerLimit, geometry.position.y() + cameraLimits.below);
        upperLimit = std::max(upperLimit, geometry.position.y() + cameraLimits.above);
    }

    Position2 cameraPosition =
        placeCamera((accumulatedPosition / totalInfluence).as<math::Position>(),
                    lowerLimit, upperLimit);

    // We know there is only one camera, but this allows to treat the camera an any other entity.
    for(const auto camera : mCameras)
    {
        camera->get<Position>().position = cameraPosition;
        debugDrawer->drawCross({cameraPosition, debug::gCrossSize, math::sdr::gMagenta});
    }
}


} // namespace grapito
} // namespace ad
