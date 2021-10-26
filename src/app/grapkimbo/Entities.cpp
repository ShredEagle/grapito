#include "Entities.h"

#include "Components/CameraGuide.h"
#include "Components/CameraTag.h"
#include "Components/Controllable.h"
#include <Components/GrappleControl.h>
#include "Components/DelayDeletion.h"
#include "Components/PivotJoint.h"
#include "Components/PlayerData.h"
#include "Components/Position.h"
#include "Components/RopeCreator.h"
#include "Components/VisualOutline.h"
#include "Components/VisualRectangle.h"
#include "Components/Mass.h"

#include "Utils/PhysicsStructs.h"
#include "aunteater/EntityManager.h"
#include "commons.h"

#include <math/VectorUtilities.h>


namespace ad {
namespace grapito
{

aunteater::Entity makePlayer(int aIndex,
                             Controller aController,
                             math::sdr::Rgb aColor,
                             GrappleMode aGrappleMode)
{
    aunteater::Entity player = aunteater::Entity()
        .add<Controllable>(aController)
        .add<GrappleControl>(aGrappleMode)
        .add<PlayerData>(aIndex, aColor)
        .add<AccelAndSpeed>()
        .add<Position>(Position2{3.f, 3.f}, player::gSize) // The position will be set by pendulum simulation
        .add<Body>(
            math::Rectangle<float>{{0.f, 0.f}, player::gSize},
            BodyType_Dynamic,
            ShapeType_Hull,
            CollisionType_Player,
            15.f,
            0.f,
            0.f,
            1.f,
            std::vector<CollisionType>{CollisionType_Static_Env}
            )
        .add<VisualRectangle>(aColor)
        .add<Mass>(player::gMass)
    ;

    //aPendular.connected->add<CameraGuide>(math::makeInterpolation<math::ease::SmoothStep>(0., 1., 0.3));

    return player;
}


aunteater::Entity makeCamera(Position2 pos)
{
    return aunteater::Entity()
        .add<CameraTag>()
        .add<Position>(pos)
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

aunteater::Entity createRopeSegment(Position2 origin, Position2 end)
{
    Vec2 ropeVector = end - origin;
    float length = ropeVector.getNorm();
    math::Radian<float> angle{atan2(ropeVector.y(), ropeVector.x())};
    math::Size<2, float> size{length, rope::ropeWidth};
    aunteater::Entity rope = aunteater::Entity()
            .add<Position>(
                Position2::Zero(),
                size
                )
            .add<Body>(
                math::Rectangle<float>{{0.f, 0.f}, size},
                BodyType_Dynamic,
                ShapeType_Hull,
                CollisionType_Moving_Env,
                0.5f,
                angle.value(),
                0.f,
                0.1f,
                std::vector<CollisionType>{CollisionType_Static_Env}
                )
            .add<VisualRectangle>(math::sdr::gGreen, VisualRectangle::Scope::RopeStructure)
            .add<AccelAndSpeed>();

    setLocalPointToWorldPos(rope, {0.f, rope::ropeHalfwidth}, origin);
    return rope;
}

void throwGrapple(aunteater::weak_entity aEntity, aunteater::EntityManager & aEntityManager)
{
    math::Size<2, float> size{.25f, .25f};
    Position2 grapplePos = static_cast<Position2>(aEntity->get<Position>().position.as<math::Vec>() + aEntity->get<Body>().massCenter.as<math::Vec>() + Vec2{.5f, .5f});
    Vec2 grappleImpulse = aEntity->get<PlayerData>().mAimVector * player::gGrappleBaseImpulse;
    aEntity->get<PlayerData>().grapple = aEntityManager.addEntity(aunteater::Entity()
            .add<Position>(
                grapplePos,
                size
                )
            .add<Body>(
                math::Rectangle<float>{{0.f, 0.f}, size},
                BodyType_Dynamic,
                ShapeType_Hull,
                CollisionType_Moving_Env,
                4.f,
                0.f,
                1.f,
                0.3f,
                std::vector<CollisionType>{CollisionType_Static_Env}
                )
            .add<VisualRectangle>(math::sdr::gYellow)
            .add<AccelAndSpeed>(grappleImpulse, 0.f)
            );

    // IMPORTANT: bugfix by adding RopeCreator only after Body has been added.
    // It is required that Body observers are visited
    aEntity->get<PlayerData>().grapple->add<RopeCreator>(aEntity);
}

void attachPlayerToGrapple(aunteater::weak_entity aPlayer, aunteater::EntityManager & aEntityManager)
{
    assert(aPlayer->get<PlayerData>().grapple != nullptr);

    aunteater::weak_entity & grapple = aPlayer->get<PlayerData>().grapple;
    RopeCreator & ropeCreator = grapple->get<RopeCreator>();
    Body & grappleBody = grapple->get<Body>();
    grappleBody.mass = 0.5f;
    grappleBody.updateData();
    grappleBody.updateConstructedBody();

    Body & playerBody = aPlayer->get<Body>();
    playerBody.mass = 15.f;
    playerBody.updateData();
    playerBody.updateConstructedBody();

    aunteater::weak_entity lastSegment = ropeCreator.mRopeSegments.back();
    Position2 end = aPlayer->get<Position>().position + aPlayer->get<Body>().massCenter.as<math::Vec>();
    Position2 origin = getLocalPointInWorld(lastSegment->get<Body>(), lastSegment->get<Position>(), {lastSegment->get<Position>().dimension.width(), 0.05f});

    aunteater::weak_entity link = aEntityManager.addEntity(createRopeSegment(
        origin,
        end
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
    aPlayer->get<PlayerData>().grapple->add<DelayDeletion>(60);
    aPlayer->get<PlayerData>().grapple->get<RopeCreator>().mTargetEntity = nullptr;
    aPlayer->get<PlayerData>().grappleAttachment->markToRemove();
    if (aPlayer->get<PlayerData>().mGrappleWeldJoint != nullptr)
    {
        aPlayer->get<PlayerData>().mGrappleWeldJoint->markToRemove();
        aPlayer->get<PlayerData>().mGrappleWeldJoint = nullptr;
    }
}

} // namespace grapito
} // namespace ad
