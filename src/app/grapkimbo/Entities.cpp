#include "Entities.h"

#include "commons.h"

#include "Components/AnimatedSprite.h"
#include "Components/AccelAndSpeed.h"
#include "Components/CameraGuide.h"
#include "Components/CameraLimits.h"
#include "Components/CameraTag.h"
#include "Components/Controllable.h"
#include "Components/DelayDeletion.h"
#include "Components/Mass.h"
#include "Components/PivotJoint.h"
#include "Components/PlayerData.h"
#include "Components/Position.h"
#include "Components/RopeCreator.h"
#include "Components/RopeSegment.h"
#include "Components/ScreenPosition.h"
#include "Components/Text.h"
#include "Components/VisualOutline.h"
#include "Components/VisualPolygon.h"
#include "Components/VisualRectangle.h"
#include "Components/VisualSprite.h"

#include "Components/Debug/DirectControlTag.h"

#include "Context/Context.h"

#include "Utils/Grapple.h"
#include "Utils/PhysicsStructs.h"

#include <aunteater/EntityManager.h>

#include <math/VectorUtilities.h>


namespace ad {
namespace grapito
{


aunteater::Entity makeDirectControllable(Controller aController, Position2 aInitialPosition)
{
    return aunteater::Entity{}
        .add<Controllable>(aController)
        .add<debug::DirectControlTag>()
        .add<Position>(aInitialPosition, Size2{0.f, 0.f})
        ;
}


aunteater::Entity makeHudText(std::string aMessage, Position2 aScreenPosition, ScreenPosition::Origin aMessageOrigin)
{
    return aunteater::Entity{}
        .add<ScreenPosition>(aScreenPosition, aMessageOrigin)
        .add<Text>(std::move(aMessage));
}

aunteater::Entity makeChoosingColorPlayer(int aIndex, Controller aController, math::sdr::Rgb aColor)
{
    aunteater::Entity player = aunteater::Entity()
        .add<Controllable>(aController)
        .add<PlayerData>(aIndex, aColor);
    return player;
}

void makeChoosingColorPlayerPlaying(aunteater::weak_entity player)
{
    player->add<AccelAndSpeed>()
        .add<AnimatedSprite>()
        .add<Body>(
            math::Rectangle<float>{{0.f, 0.f}, player::gSize},
            BodyType_Dynamic,
            ShapeType_Hull,
            CollisionType_Player,
            45.f,
            0.f,
            0.f,
            1.f,
            std::vector<CollisionType>{CollisionType_Static_Env}
            )
        .add<CameraGuide>(player::gCameraGuideWeight, player::gCameraGuideOffset)
        .add<CameraLimits>(player::gCameraLimits[0], player::gCameraLimits[1])
        .add<Mass>(player::gMass)
        .add<Position>(Position2{3.f, 3.f}, player::gSize) // The position will be set by pendulum simulation
        //.add<VisualRectangle>(aColor)
        .add<VisualSprite>((AtlasIndex)player->get<PlayerData>().id); // the sprite itself is handled by animation system
                                                                      // SMELL: we hardcode the fact that we use the player id as atlas index
}


aunteater::Entity makePlayingPlayer(int aIndex,
                             Controller aController,
                             math::sdr::Rgb aColor,
                             Position2 aPosition)
{
    aunteater::Entity player = aunteater::Entity()
        // The component will be populated by AnimationState system.
        .add<AccelAndSpeed>()
        .add<AnimatedSprite>()
        .add<Body>(
            math::Rectangle<float>{{0.f, 0.f}, player::gSize},
            BodyType_Dynamic,
            ShapeType_Hull,
            CollisionType_Player,
            45.f,
            0.f,
            0.f,
            1.f,
            std::vector<CollisionType>{CollisionType_Static_Env}
            )
        .add<CameraGuide>(player::gCameraGuideWeight, player::gCameraGuideOffset)
        .add<CameraLimits>(player::gCameraLimits[0], player::gCameraLimits[1])
        .add<Controllable>(aController)
        .add<Mass>(player::gMass)
        .add<PlayerData>(aIndex, aColor)
        .add<Position>(aPosition, player::gSize) // The position will be set by pendulum simulation
        //.add<VisualRectangle>(aColor)
        .add<VisualSprite>((AtlasIndex)aIndex) // the sprite itself is handled by animation system
                                               // SMELL: we hardcode the fact that we use the player id as atlas index
    ;

    //Player should not be rotated by any constraints
    player.get<Body>().invMoi = 0.f;

    //aPendular.connected->add<CameraGuide>(math::makeInterpolation<math::ease::SmoothStep>(0., 1., 0.3));

    return player;
}


void kill(aunteater::weak_entity aPlayer)
{
    if (aPlayer->get<PlayerData>().grapple != nullptr)
    {
        detachPlayerFromGrapple(aPlayer);
    }

    aPlayer->markToRemove();
}


aunteater::Entity makeCamera(Position2 pos)
{
    return aunteater::Entity()
        .add<CameraTag>()
        .add<Position>(pos)
        ;
}

aunteater::Entity makeAnchor(Position2 aPosition, std::vector<Position2> aVertices)
{
    return aunteater::Entity()
        .add<Body>(
            aVertices,
            BodyType_Static,
            ShapeType_Hull,
            CollisionType_Static_Env,
            0.f,
            0.f,
            1.f
            )
        .add<Position>(aPosition, math::Size<2, float>{0.f, 0.f})
        .add<AccelAndSpeed>()
        .add<VisualPolygon>(aVertices, anchor::gColor)
    ;
}

aunteater::Entity makeAnchor(math::Position<2, float> aPosition, math::Size<2, float> aSize)
{
    return aunteater::Entity()
        .add<Body>(
            math::Rectangle<float>{{0.f, 0.f}, aSize},
            BodyType_Static,
            ShapeType_Hull,
            CollisionType_Static_Env,
            0.f,
            0.f,
            1.f
            )
        .add<Position>(aPosition, aSize)
        .add<AccelAndSpeed>()
        .add<VisualRectangle>(anchor::gColor)
    ;
}

aunteater::Entity createRopeSegment(Position2 origin, Position2 end, aunteater::weak_entity aPlayer)
{
    Vec2 ropeVector = end - origin;
    float length = ropeVector.getNorm();
    math::Radian<float> angle{atan2(ropeVector.y(), ropeVector.x())};
    math::Size<2, float> size{length, rope::ropeWidth};
    aunteater::Entity rope = aunteater::Entity()
            .add<AccelAndSpeed>(aPlayer->get<AccelAndSpeed>().speed)
            .add<Body>(
                math::Rectangle<float>{{0.f, 0.f}, size},
                BodyType_Dynamic,
                ShapeType_Hull,
                CollisionType_Moving_Env,
                0.5f,
                angle.value(),
                0.f,
                0.1f,
                std::vector<CollisionType>{CollisionType_Static_Env},
                std::vector<CollisionType>{CollisionType_Static_Env, CollisionType_Moving_Env}
                )
            .add<Position>(
                Position2::Zero(),
                size
                )
            .add<RopeSegment>(aPlayer->get<PlayerData>().id)
            .add<VisualRectangle>(math::sdr::gGreen, VisualRectangle::Scope::RopeStructure)
    ;

    setLocalPointToWorldPos(rope, {0.f, rope::ropeHalfwidth}, origin);
    return rope;
}

void throwGrapple(aunteater::weak_entity aPlayer, aunteater::EntityManager & aEntityManager, Context & aContext)
{
    Position2 grapplePos = static_cast<Position2>(aPlayer->get<Position>().position.as<math::Vec>() + aPlayer->get<Body>().massCenter.as<math::Vec>() + Vec2{.5f, .5f});
    Vec2 grappleImpulse = aPlayer->get<PlayerData>().mAimVector * player::gGrappleBaseImpulse + aPlayer->get<AccelAndSpeed>().speed;
    aPlayer->get<PlayerData>().grapple = aEntityManager.addEntity(aunteater::Entity()
            .add<Position>(
                grapplePos,
                math::Size<2, float>{player::gGrappleSpritePixelSize * render::gSpritePixelWorldSize}
                )
            // TODO (Franz): named "configuration" instead of magic numbers.
            .add<Body>(
                rope::grappleVertices,
                BodyType_Dynamic,
                ShapeType_Hull,
                CollisionType_Moving_Env,
                4.f,
                0.f,
                player::gGrappleFriction,
                0.3f,
                std::vector<CollisionType>{CollisionType_Static_Env},
                std::vector<CollisionType>{CollisionType_Static_Env, CollisionType_Moving_Env}
                )
            //.add<VisualPolygon>(rope::grappleVertices, math::sdr::gYellow)
            .add<VisualSprite>(gGrappleAtlasIndex,
                               aContext.loadSingleSprite("sprite_sheet/grapple.png").second)
            .add<AccelAndSpeed>(grappleImpulse, 0.f)
            );

    // IMPORTANT: bugfix by adding RopeCreator only after Body has been added.
    // It is required that Body observers are visited
    aPlayer->get<PlayerData>().grapple->add<RopeCreator>(aPlayer);
}

void attachPlayerToGrapple(aunteater::weak_entity aPlayer, aunteater::EntityManager & aEntityManager)
{
    assert(aPlayer->get<PlayerData>().grapple != nullptr);
    assert(aPlayer->get<PlayerData>().grappleAttachment == nullptr);

    aunteater::weak_entity & grapple = aPlayer->get<PlayerData>().grapple;
    RopeCreator & ropeCreator = grapple->get<RopeCreator>();
    Body & grappleBody = grapple->get<Body>();
    grappleBody.mass = 0.5f;
    grappleBody.updateData();
    grappleBody.updateConstructedBody();

    aunteater::weak_entity lastSegment = ropeCreator.mRopeSegments.back();
    Position2 end = aPlayer->get<Position>().position + aPlayer->get<Body>().massCenter.as<math::Vec>();
    Position2 origin = getLocalPointInWorld(lastSegment->get<Body>(), lastSegment->get<Position>(), {lastSegment->get<Position>().dimension.width(), 0.05f});

    aunteater::weak_entity link = aEntityManager.addEntity(createRopeSegment(
        origin,
        end,
        aPlayer
    ));

    aEntityManager.addEntity(
            aunteater::Entity()
            .add<PivotJoint>(
                Position2{0.f, 0.05f},
                Position2{lastSegment->get<Position>().dimension.width(), 0.05f},
                link,
                lastSegment
            ));
    aPlayer->get<PlayerData>().grappleAttachment = aEntityManager.addEntity(
            aunteater::Entity()
            .add<PivotJoint>(
                Position2{lastSegment->get<Position>().dimension.width(), 0.05f},
                aPlayer->get<Body>().massCenter,
                link,
                aPlayer
            ));

    ropeCreator.mRopeSegments.push_back(link);
}

void detachPlayerFromGrapple(aunteater::weak_entity aPlayer)
{
    PlayerData & playerData = aPlayer->get<PlayerData>();

    // Dissociate each segment from the player
    std::ranges::for_each(
        playerData.grapple->get<RopeCreator>().mRopeSegments,
        [](RopeSegment & segment)
        {
            segment.playerId = playerIdToDetachedId(segment.playerId);
        },
        [](auto entity)->RopeSegment &{return entity->template get<RopeSegment>();});

    //Cleanup grapple entity
    playerData.grapple->add<DelayDeletion>(60);
    playerData.grapple->get<RopeCreator>().mTargetEntity = nullptr;
    playerData.grapple = nullptr;
    playerData.throwGrappleFrameCounter = 0;

    //Cleanup rope pivot joint to player if it exists
    if (playerData.grappleAttachment != nullptr)
    {
        playerData.grappleAttachment->markToRemove();
        playerData.grappleAttachment = nullptr;
    }

    //Cleanup grapple attachement to static environment if it exists
    if (playerData.mGrappleWeldJoint != nullptr)
    {
        playerData.mGrappleWeldJoint->markToRemove();
        playerData.mGrappleWeldJoint = nullptr;
    }

    playerData.controlState &= ~ControlState_Attached;
    playerData.controlState &= ~ControlState_Throwing;
}

} // namespace grapito
} // namespace ad
