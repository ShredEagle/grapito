#include "RopeGame.h"

#include "../Entities.h"

#include <Components/AccelAndSpeed.h>
#include <Components/Body.h>
#include <Components/CameraGuide.h>
#include <Components/Controllable.h>
#include <Components/GrappleControl.h>
#include <Components/Position.h>
#include <Components/VisualRectangle.h>
#include <Components/Mass.h>

#include <Systems/AccelSolver.h>
#include <Systems/CameraGuidedControl.h>
#include <Systems/Control.h>
#include <Systems/Gravity.h>
#include <Systems/LevelGeneration.h>
#include <Systems/Render.h>
#include <Systems/Physics.h>
#include <Systems/RopeCreation.h>

#include <aunteater/Entity.h>


namespace ad {
namespace grapito {


RopeGame::RopeGame(std::shared_ptr<graphics::AppInterface> aAppInterface) :
    GameScene{std::move(aAppInterface)}
{
    mSystemManager.add<LevelGeneration>();
    mSystemManager.add<Control>();
    mSystemManager.add<Gravity>();
    mSystemManager.add<RopeCreation>();

    mSystemManager.add<AccelSolver>();
    mSystemManager.add<Physics>();

    mSystemManager.add<CameraGuidedControl>();

    mRenderSystem = mSystemManager.add<Render>(mAppInterface); 
    

    // Camera
    aunteater::weak_entity camera = mEntityManager.addEntity(makeCamera());
    // Environment anchors

    aunteater::weak_entity anchor_2 = mEntityManager.addEntity(
        makeAnchor(Position2{12.f, 5.f}, math::Size<2, float>{2.f, 2.f} ));

    aunteater::weak_entity anchor_3 = mEntityManager.addEntity(
        makeAnchor(Position2{24.f, 9.f}, math::Size<2, float>{2.f, 2.f} ));

    aunteater::weak_entity floor = mEntityManager.addEntity(
        makeAnchor(Position2{-20.f, -4.f}, math::Size<2, float>{40.f, 2.f} ));

    // Player 1
    Controller controller = isGamepadPresent(Controller::Gamepad_0) ?
                            Controller::Gamepad_0 : Controller::Keyboard;

    mEntityManager.addEntity(
        makePlayer(0, controller, math::sdr::gCyan)
            .add<CameraGuide>(1.0f));

    // Player 2
    if (isGamepadPresent(Controller::Gamepad_1))
    {
        mEntityManager.addEntity(
            makePlayer(1, Controller::Gamepad_1, math::sdr::gMagenta));
    }
}


void RopeGame::render() const
{
    mRenderSystem->render();
}


} // namespace grapito
} // namespace ad
