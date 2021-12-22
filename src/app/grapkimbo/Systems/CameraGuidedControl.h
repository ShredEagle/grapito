#pragma once


#include "../Input.h"

#include "../Components/CameraGuide.h"
#include "../Components/CameraLimits.h"
#include "../Components/CameraTag.h"
#include "../Components/Position.h"

#include <aunteater/EntityManager.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>


namespace ad {
namespace grapito
{

using Camera = aunteater::Archetype<CameraTag, Position>;
using CameraPoints = aunteater::Archetype<CameraGuide, Position>;
// the CameraLimits are accessed via a separate family, instead of merged into CameraPoints.
// this is because for player-eliminiation fade-out guides, limits would be a nuisance.
using CameraLimiter = aunteater::Archetype<CameraLimits, Position>;


class CameraGuidedControl : public aunteater::System<GrapitoTimer, GameInputState>
{
public:
    CameraGuidedControl(aunteater::EntityManager & aEntityManager);

    void update(const GrapitoTimer aTimer, const GameInputState &) override;

private:
    const aunteater::FamilyHelp<Camera> mCameras;
    const aunteater::FamilyHelp<CameraPoints> mCameraPoints;
    const aunteater::FamilyHelp<CameraLimiter> mCameraLimiters;

};

} // namespace grapito
} // namespace ad
