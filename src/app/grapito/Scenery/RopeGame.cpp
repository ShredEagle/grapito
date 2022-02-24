#include "RopeGame.h"

#include "../Entities.h"
#include "../TopLevelStates.h"

#include "Input.h"
#include "LevelStacks.h"

#include "Systems/DelayDeleter.h"
#include "Systems/GrappleCleanup.h"
#include "Systems/GrappleInteractions.h"
#include "Systems/GrappleJointCreator.h"
#include "Systems/PlayerJoin.h"
#include "Systems/SoundSystem.h"

#include "Systems/Debug/DirectControl.h"

#include <Components/AccelAndSpeed.h>
#include <Components/Body.h>
#include <Components/CameraGuide.h>
#include <Components/Controllable.h>
#include <Components/Position.h>
#include <Components/VisualRectangle.h>
#include <Components/Mass.h>
#include <Components/SoundPlayer.h>

#include <Systems/AccelSolver.h>
#include <Systems/CameraGuidedControl.h>
#include <Systems/Control.h>
#include <Systems/Rules/CompetitionRule.h>
#include <Systems/Rules/FloorIsLavaRule.h>
#include <Systems/Rules/FreesoloRule.h>
#include <Systems/Gravity.h>
#include <Systems/LevelGeneration.h>
#include <Systems/Physics.h>
#include <Systems/RenderBackground.h>
#include <Systems/RenderHud.h>
#include <Systems/RenderToScreen.h>
#include <Systems/RenderWorld.h>
#include <Systems/RopeCreation.h>
#include <Systems/SegmentStacker.h>
#include <Systems/TransitionAnimationState.h>

#include <handy/StringId_Interning.h>

#include <aunteater/Entity.h>


namespace ad {
namespace grapito {

StringId soundId_MusicSid = handy::internalizeString("bgmusic");

RopeGame::RopeGame(std::shared_ptr<Context> aContext,
                   std::shared_ptr<graphics::AppInterface> aAppInterface,
                   GameMode aGameMode,
                   const Controller aController) :
    GameScene{std::move(aContext), std::move(aAppInterface)},
    mGameMode{aGameMode}
{
    //Cleanup player list context to make sure its empty
    mContext->mPlayerList.clear();
    mContext->mPlayerList.addPlayer(aController, PlayerJoinState_Queued);

    // See `makeDirectControllable()` to instantiate a direct control guide.
    mSystemManager.add<debug::DirectControl>();

    mSystemManager.add<PlayerJoin>(mContext);
    std::shared_ptr<Control> controlSystem = mSystemManager.add<Control>(mContext);
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
    mSystemManager.add<SegmentStacker>(0.f); // after the camera is repositioned
    mSystemManager.add<DelayDeleter>();

    if (!mContext->nosound)
    {
        mSystemManager.add<SoundSystem>(*mContext->mSoundManager);
    }

    auto renderToScreen = std::make_shared<RenderToScreen>(mEntityManager, mAppInterface, *this); 
    // Done after CameraGuidedControl, to avoid having two camera guides on the frame a player is killed.

    switch (mGameMode)
    {
    case GameMode::Multiplayer:
        mSystemManager.add<CompetitionRule>(mContext, controlSystem, renderToScreen);
        break;
    case GameMode::Freesolo:
        mSystemManager.add<FreesoloRule>(mContext, controlSystem, renderToScreen);
        break;
    case GameMode::FloorIsLava:
        mSystemManager.add<FloorIsLavaRule>(mContext, controlSystem, renderToScreen);
        break;
    }

    mRenderBackgroundSystem = mSystemManager.add<RenderBackground>(mAppInterface);
    mRenderBackgroundSystem->addTiledLayer(mContext->pathFor(background::gSpaceImage), background::gSpaceScrollFactor);
    mRenderBackgroundSystem->addTiledLayer(mContext->pathFor(background::gSmallStarImage), background::gSmallStarScrollFactor);
    mRenderBackgroundSystem->addTiledLayer(mContext->pathFor(background::gStarImage), background::gStarScrollFactor);
    mRenderBackgroundSystem->addLayer(mContext->pathFor(background::gEarthImage), { -gLevelHalfWidth - gWallWidth, -60.f * background::gEarthScrollFactor * render::gSpritePixelWorldSize, background::gEarthScrollFactor });
    mRenderBackgroundSystem->addLayer(mContext->pathFor(background::gBuildingImage), {-gLevelHalfWidth - gWallWidth - 4.f, -15.f * background::gBuidlingScrollFactor * render::gSpritePixelWorldSize, background::gBuidlingScrollFactor});

    for (int i = 0; i < 5; i++)
    {
        mRenderBackgroundSystem->addLayer(mContext->pathFor(background::gBuildingWindowImage), { -gLevelHalfWidth, 322.f * i * render::gSpritePixelWorldSize, 0.f });
    }

    mRenderWorldSystem = mSystemManager.add<RenderWorld>(mAppInterface); 
    mRenderHudSystem = mSystemManager.add<RenderHud>(mContext->pathFor(hud::gFont), mAppInterface); 
    
    // Will actually render everything to screen.
    mSystemManager.add(std::move(renderToScreen)); 

    { // Load sprite animations
        // Note: It is not obvious whether it is better to maintain a permanent instance of the sprite sheet
        // (trading RAM for better load times), but this is a good pretext to use the ResourceManager.
        auto spriteSheets = {
            std::ref(mContext->loadAnimationSpriteSheet("sprite_sheet/idle.json")),
            std::ref(mContext->loadAnimationSpriteSheet("sprite_sheet/run.json")),
            std::ref(mContext->loadAnimationSpriteSheet("sprite_sheet/jump.json")),
            std::ref(mContext->loadAnimationSpriteSheet("sprite_sheet/walled.json")),
        };

        graphics::sprite::Animator animator;
        // TODO cache via resource system
        // Install the blue (default) variant
        mRenderWorldSystem->installAtlas(animator.load(spriteSheets.begin(), spriteSheets.end()));
        spriteAnimationSystem->installAnimator(std::move(animator));

        using namespace std::string_literals;
        // The blue variant is already installed above.
        for (const auto & color : {"green"s, "red"s, "yellow"s})
        {
            arte::Image<math::sdr::Rgba> colorVariation = arte::stackVertical<math::sdr::Rgba>(
                std::vector<arte::ImageRgba>{
                    arte::ImageRgba{mContext->pathFor("sprite_sheet/idle_" + color + ".png")},
                    arte::ImageRgba{mContext->pathFor("sprite_sheet/run_" + color + ".png")},
                    arte::ImageRgba{mContext->pathFor("sprite_sheet/jump_" + color + ".png")},
                    arte::ImageRgba{mContext->pathFor("sprite_sheet/walled_" + color + ".png")},
                });
            mRenderWorldSystem->installAtlas(graphics::sprite::loadAtlas(colorVariation));
        }
    }

    { // Load grapple sprite
        const graphics::sprite::LoadedAtlas & atlas = mContext->loadSingleSprite("sprite_sheet/grapple.png").first;
        // TODO Ad 2022/02/10: Refactor this major SMELL.
        gGrappleAtlasIndex = mRenderWorldSystem->installAtlas(atlas);
    }


    { // Load sounds
        mContext->loadOggSoundData("sounds/burn.ogg", false);
        mContext->loadOggSoundData("sounds/grapple_throwing.ogg", false);
        mContext->loadOggSoundData("sounds/grapple_throwing2.ogg", false);
        mContext->loadOggSoundData("sounds/grapple_attached.ogg", false);
        mContext->loadOggSoundData("sounds/grapple_attached2.ogg", false);
        mContext->loadOggSoundData("sounds/jump.ogg", false);
        mContext->loadOggSoundData("sounds/slash.ogg", false);
        mContext->loadOggSoundData("sounds/bgmusic.ogg", false);
    }

    // Camera
    mEntityManager.addEntity(makeCamera());

    //
    // Level
    //
    {
        mEntityManager.addEntity(
            makeAnchor(Position2{ -gLevelHalfWidth - gWallWidth, -gWallWidth }, math::Size<2, float>{(gLevelHalfWidth + gWallWidth) * 2, gWallWidth}));

        createStackFive(mEntityManager, 0.f);
    }

    // Players are added by GameRule system.
}

std::pair<TransitionProgress, UpdateStatus> RopeGame::enter(
            const GrapitoTimer &,
            const GameInputState &,
            const StateMachine &)
{
    if (!mContext->nosound && !mContext->nobgmusic)
    {
        mBgMusicSource = mContext->mSoundManager->playSound(soundId_MusicSid, {.gain = 0.3f, .looping = AL_TRUE});
    }
    return {TransitionProgress::Complete, UpdateStatus::KeepFrame};
}

std::pair<TransitionProgress, UpdateStatus> RopeGame::exit(
            const GrapitoTimer &,
            const GameInputState &,
            const StateMachine &)
{
    if (!mContext->nosound && !mContext->nobgmusic)
    {
        mContext->mSoundManager->stopSound(mBgMusicSource);
    }
    return {TransitionProgress::Complete, UpdateStatus::KeepFrame};
}

void RopeGame::render() const
{
    mRenderBackgroundSystem->render();
    mRenderWorldSystem->render();
    mRenderHudSystem->render();
}

std::shared_ptr<MenuScene> RopeGame::getPauseMenu()
{
    return setupPauseMenu(mContext, mAppInterface, shared_from_this(), mGameMode);
}


} // namespace grapito
} // namespace ad
