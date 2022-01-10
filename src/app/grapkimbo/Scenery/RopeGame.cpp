#include "RopeGame.h"

#include "../Entities.h"

#include "Systems/DelayDeleter.h"
#include "Systems/GrappleCleanup.h"
#include "Systems/GrappleJointCreator.h"
#include "Systems/Debug/DirectControl.h"
#include "Systems/SoundSystem.h"

#include <Components/AccelAndSpeed.h>
#include <Components/Body.h>
#include <Components/CameraGuide.h>
#include <Components/Controllable.h>
#include <Components/GrappleControl.h>
#include <Components/Position.h>
#include <Components/VisualRectangle.h>
#include <Components/Mass.h>
#include <Components/SoundPlayer.h>

#include <Systems/AccelSolver.h>
#include <Systems/CameraGuidedControl.h>
#include <Systems/Control.h>
#include <Systems/GameRule.h>
#include <Systems/Gravity.h>
#include <Systems/Hud.h>
#include <Systems/LevelGeneration.h>
#include <Systems/Render.h>
#include <Systems/Physics.h>
#include <Systems/RopeCreation.h>
#include <Systems/TransitionAnimationState.h>

#include <handy/StringId_Interning.h>

#include <aunteater/Entity.h>


namespace ad {
namespace grapito {

StringId soundId_MusicSid = handy::internalizeString("bgmusic");

std::vector<aunteater::Entity> setupPlayers()
{
    std::vector<aunteater::Entity> players;

    // Player 1
    {
        Controller controller = isGamepadPresent(Controller::Gamepad_0) ?
                                Controller::Gamepad_0 : Controller::KeyboardMouse;
        players.push_back(makePlayer(0, controller, math::sdr::gCyan));
    }

    // Player 2
    {
        Controller controller = Controller::Gamepad_1;
        if (isGamepadPresent(controller))
        {
            players.push_back(makePlayer(1, controller, math::sdr::gMagenta));
        }
    }

    return players;
}


RopeGame::RopeGame(std::shared_ptr<Context> aContext,
                   std::shared_ptr<graphics::AppInterface> aAppInterface) :
    GameScene{std::move(aContext), std::move(aAppInterface)}
{
    std::vector<aunteater::Entity> players = setupPlayers();

    mSystemManager.add<debug::DirectControl>();

    mSystemManager.add<Control>();
    mSystemManager.add<Gravity>();
    mSystemManager.add<RopeCreation>();

    mSystemManager.add<AccelSolver>();
    mSystemManager.add<GrappleCleanup>();
    mSystemManager.add<Physics>();
    mSystemManager.add<GrappleJointCreator>();

    std::shared_ptr<TransitionAnimationState> spriteAnimationSystem = 
        mSystemManager.add<TransitionAnimationState>();

    mSystemManager.add<CameraGuidedControl>();
    mSystemManager.add<DelayDeleter>();

    auto soundSystem = mSystemManager.add<SoundSystem>(mContext->mSoundManager);

    // Done after CameraGuidedControl, to avoid having two camera guides on the frame a player is killed.
    mSystemManager.add<GameRule>(players);

    mRenderSystem = mSystemManager.add<Render>(mAppInterface); 
    mSystemManager.add<Hud>(mContext->pathFor(hud::gFont), mAppInterface); 

    { // Load sprite animations
        // Note: It is not obvious whether it is better to maintain a permanent instance of the sprite sheet
        // (trading RAM for better load times), but this is a good pretext to use the ResourceManager.
        auto spriteSheets = {
            std::ref(mContext->loadAnimationSpriteSheet("sprite_sheet/idle.json")),
            std::ref(mContext->loadAnimationSpriteSheet("sprite_sheet/run.json")),
        };

        graphics::sprite::Animator animator;
        mRenderSystem->loadSpriteAnimations(spriteSheets.begin(), spriteSheets.end(), animator);
        spriteAnimationSystem->installAnimator(std::move(animator));
    }

    { // Load sounds
        mContext->loadOggSoundData("sounds/launch.ogg", false);
        mContext->loadOggSoundData("sounds/weld.ogg", false);
        mContext->loadOggSoundData("sounds/jump.ogg", false);
        mContext->loadOggSoundData("sounds/ropejump.ogg", false);
        mContext->loadOggSoundData("sounds/bgmusic.ogg", false);
    }

    // Camera
    aunteater::weak_entity camera = mEntityManager.addEntity(makeCamera());

    constexpr float gLevelHalfWidth = 50.f;

    //
    // Level
    //
    {
        //floors
        aunteater::weak_entity floor = mEntityManager.addEntity(
            makeAnchor(Position2{ -gLevelHalfWidth, -4.f }, math::Size<2, float>{gLevelHalfWidth, 2.f}));
        aunteater::weak_entity floor2 = mEntityManager.addEntity(
            makeAnchor(Position2{0.f, -2.f}, math::Size<2, float>{gLevelHalfWidth, 2.f} ));    

        // First anchor
        aunteater::weak_entity polygon = mEntityManager.addEntity(
            makeAnchor(Position2{ -15.f, 10.f }, std::vector<Position2>{
            Position2{ 0.f, 0.f }, Position2{ 1.5f, 0.f }, Position2{ 2.5f, 0.5f },
                Position2{ 2.75f, 2.f }, Position2{ 2.f, 3.f }, Position2{ 0.5f, 2.5f },
                Position2{ 0.f, 1.f }
        }));


        // 1st square
        mEntityManager.addEntity(
                makeAnchor(Position2{ 10.f, 28.f }, math::Size<2, float>{3.f, 3.f}));

        // 1st level platform
        mEntityManager.addEntity(
                makeAnchor(Position2{ 23.f, 17.f }, math::Size<2, float>{30.f, 2.f}));

        mEntityManager.addEntity(
            makeAnchor(Position2{ 35.f, 30.f }, std::vector<Position2>{
                Position2{ 0.f, 0.f }, Position2{ 11.5f, 0.f }, Position2{ 12.5f, 0.5f },
                Position2{ 12.75f, 2.f }, Position2{ 12.f, 3.f }, Position2{ 0.5f, 3.f },
                Position2{ 0.f, 1.f }
        }));

        // Triangle
        mEntityManager.addEntity(
            makeAnchor(Position2{ -30.f, 40.f }, std::vector<Position2>{
            Position2{ 0.f, 0.f }, Position2{ 7.f, -5.f }, Position2{ 14.f, 0.f },
        }));

        // 2nd square
        mEntityManager.addEntity(
                makeAnchor(Position2{ 0.f, 50.f }, math::Size<2, float>{3.f, 3.f}));

        // 3nd square
        mEntityManager.addEntity(
                makeAnchor(Position2{ 22.f, 65.f }, math::Size<2, float>{4.f, 4.f}));

        // 4th square
        mEntityManager.addEntity(
                makeAnchor(Position2{ -10.f, 80.f }, math::Size<2, float>{3.f, 3.f}));

        // 5th square
        mEntityManager.addEntity(
                makeAnchor(Position2{ -15.f, 100.f }, math::Size<2, float>{3.f, 3.f}));

        // 6th square
        mEntityManager.addEntity(
                makeAnchor(Position2{ 25.f, 85.f }, math::Size<2, float>{6.f, 6.f}));

        // 7th square
        mEntityManager.addEntity(
                makeAnchor(Position2{ 0.f, 115.f }, math::Size<2, float>{3.f, 3.f}));

        //wall
        mEntityManager.addEntity(
            makeAnchor(Position2{-gLevelHalfWidth, -2.f}, math::Size<2, float>{2.f, 100.f} ));
        //wall2
        mEntityManager.addEntity(
            makeAnchor(Position2{ gLevelHalfWidth, -2.f }, math::Size<2, float>{2.f, 100.f}));
    }

    //
    // Players
    //
    {
        for (const auto & player : players)
        {
            mEntityManager.addEntity(player);
        }

        // Debug direct control (for camera influence)
        mEntityManager.addEntity(
            makeDirectControllable(players[0].get<Controllable>().controller)
        );

    }
}

std::pair<TransitionProgress, UpdateStatus> RopeGame::enter(
            const GrapitoTimer &,
            const GameInputState &,
            const StateMachine &)
{
    mBgMusicSource = mContext->mSoundManager.playSound(soundId_MusicSid, true);
    return {TransitionProgress::Complete, UpdateStatus::KeepFrame};
}

std::pair<TransitionProgress, UpdateStatus> RopeGame::exit(
            const GrapitoTimer &,
            const GameInputState &,
            const StateMachine &)
{
    mContext->mSoundManager.stopSound(mBgMusicSource);
    return {TransitionProgress::Complete, UpdateStatus::KeepFrame};
}

void RopeGame::render() const
{
    mRenderSystem->render();
}


} // namespace grapito
} // namespace ad
