#include "DirectControl.h"

#include "../../Configuration.h"

#include "../../Utils/DrawDebugStuff.h"

#include "../../Components/CameraGuide.h"


namespace ad {
namespace grapito {
namespace debug {

DirectControl::DirectControl(aunteater::EntityManager & aEntityManager) :
    mDirectControllables{aEntityManager}
{}


void DirectControl::update(const GrapitoTimer aTimer, const GameInputState & aInputState)
{
    for (auto directControllable : mDirectControllables)
    {
        auto & [controllable, _directControlTag, position] = directControllable;
        if (aInputState.get(controllable.controller)[R3].positiveEdge())
        {
            if (directControllable->has<CameraGuide>())
            {
                directControllable->markComponentToRemove<CameraGuide>();
            }
            else
            {
                directControllable->add<CameraGuide>(player::gCameraWeight);
            }
        }

        const ControllerInputState & inputs =
            aInputState.controllerState[(std::size_t)controllable.controller];

        position.position += aInputState.getRightDirection(controllable.controller)
                             * debug::gDirectControlSpeed 
                             * aTimer.delta();

        math::Rectangle<float> outline{position.position, debug::gDirectControlDrawSize};
        debugDrawer->drawOutline(ad::debug::Rectangle{outline.centered(), debug::gDirectControlDrawColor});
    }
}


} // namespace debug
} // namespace grapito
} // namespace ad
