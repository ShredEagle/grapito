#include "RopeGame.h"

#include "../Entities.h"
#include "Systems/DelayDeleter.h"
#include "Systems/GrappleCleanup.h"
#include "Systems/GrappleJointCreator.h"

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
#include <Systems/TransitionAnimationState.h>

#include <aunteater/Entity.h>


namespace ad {
namespace grapito {


RopeGame::RopeGame(std::shared_ptr<Context> aContext,
                   std::shared_ptr<graphics::AppInterface> aAppInterface) :
    GameScene{std::move(aContext), std::move(aAppInterface)}
{

    mSystemManager.add<Control>();
    mSystemManager.add<Gravity>();
    mSystemManager.add<RopeCreation>();
    mSystemManager.add<GrappleJointCreator>();

    mSystemManager.add<AccelSolver>();
    mSystemManager.add<Physics>();

    std::shared_ptr<TransitionAnimationState> spriteAnimationSystem = 
        mSystemManager.add<TransitionAnimationState>();

    mSystemManager.add<CameraGuidedControl>();
    mSystemManager.add<GrappleCleanup>();
    mSystemManager.add<DelayDeleter>();

    mRenderSystem = mSystemManager.add<Render>(mAppInterface); 

    { // Load sprite animations
        auto spriteSheets = {
            arte::AnimationSpriteSheet::LoadAseFile(mContext->pathFor("sprite_sheet/idle.json")),
            arte::AnimationSpriteSheet::LoadAseFile(mContext->pathFor("sprite_sheet/run.json")),
        };

        graphics::sprite::Animator animator;
        mRenderSystem->loadSpriteAnimations(spriteSheets.begin(), spriteSheets.end(), animator);
        spriteAnimationSystem->installAnimator(std::move(animator));
    }

    // Camera
    aunteater::weak_entity camera = mEntityManager.addEntity(makeCamera());
    //floors
    aunteater::weak_entity polygon = mEntityManager.addEntity(
        makeAnchor(Position2{ 5.f, 7.f }, std::vector<Position2>{
        Position2{ 0.f, 0.f }, Position2{ 1.5f, 0.f }, Position2{ 2.5f, 0.5f },
            Position2{ 2.75f, 2.f }, Position2{ 2.f, 3.f }, Position2{ 0.5f, 2.5f },
            Position2{ 0.f, 1.f }
    }));
    aunteater::weak_entity floor = mEntityManager.addEntity(
        makeAnchor(Position2{ -20.f, -4.f }, math::Size<2, float>{40.f, 2.f}));
    aunteater::weak_entity floor2 = mEntityManager.addEntity(
        makeAnchor(Position2{20.f, -2.f}, math::Size<2, float>{40.f, 2.f} ));    
    mEntityManager.addEntity(
            makeAnchor(Position2{ 15.f, 15.f }, math::Size<2, float>{40.f, 2.f}));
    mEntityManager.addEntity(
        makeAnchor(Position2{ 5.f, 25.f }, std::vector<Position2>{
            Position2{ 0.f, 0.f }, Position2{ 11.5f, 0.f }, Position2{ 12.5f, 0.5f },
            Position2{ 12.75f, 2.f }, Position2{ 12.f, 3.f }, Position2{ 0.5f, 3.f },
            Position2{ 0.f, 1.f }
    }));
    mEntityManager.addEntity(
        makeAnchor(Position2{ -10.f, 45.f }, std::vector<Position2>{
        Position2{ 0.f, 0.f }, Position2{ 7.f, -5.f }, Position2{ 14.f, 0.f },
    }));
    //wall
    mEntityManager.addEntity(
        makeAnchor(Position2{-20.f, -2.f}, math::Size<2, float>{2.f, 100.f} ));
    //wall2
    mEntityManager.addEntity(
        makeAnchor(Position2{ 60.f, -2.f }, math::Size<2, float>{2.f, 100.f}));

    // Player 1
    Controller controller = isGamepadPresent(Controller::Gamepad_0) ?
                            Controller::Gamepad_0 : Controller::KeyboardMouse;

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
