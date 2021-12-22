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
using CameraPoints = aunteater::Archetype<CameraGuide, CameraLimits, Position>;


class CameraGuidedControl : public aunteater::System<GrapitoTimer, GameInputState>
{
public:
    CameraGuidedControl(aunteater::EntityManager & aEntityManager);

    void update(const GrapitoTimer aTimer, const GameInputState &) override;

private:
    const aunteater::FamilyHelp<Camera> mCameras;
    const aunteater::FamilyHelp<CameraPoints> mCameraPoints;

};

} // namespace grapito
} // namespace ad
