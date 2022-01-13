#include "RopeGame.h"

#include "../Entities.h"

#include "LevelStacks.h"
#include "Systems/DelayDeleter.h"
#include "Systems/GrappleCleanup.h"
#include "Systems/GrappleInteractions.h"
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
    mSystemManager.add<GrappleInteractions>();

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
        mContext->loadOggSoundData("sounds/slash.ogg", false);
        mContext->loadOggSoundData("sounds/bgmusic.ogg", false);
    }

    // Camera
    aunteater::weak_entity camera = mEntityManager.addEntity(makeCamera());

    //
    // Level
    //
    {
        constexpr float gLevelHalfWidth = 40.f;
        constexpr float gWallWidth = 4.f;
        constexpr float gJumpHeight = 3.6f;
        constexpr float gStackHeight = gJumpHeight * 15.f;

        mEntityManager.addEntity(
            makeAnchor(Position2{ -gLevelHalfWidth - gWallWidth, -gWallWidth }, math::Size<2, float>{(gLevelHalfWidth + gWallWidth) * 2, gWallWidth}));

        createStackFive(mEntityManager, 0.f);
        createStackThree(mEntityManager, gStackHeight);
        createStackOne(mEntityManager, gStackHeight * 2);
        createStackTwo(mEntityManager, gStackHeight * 3);
        createStackFour(mEntityManager, gStackHeight * 4);
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
