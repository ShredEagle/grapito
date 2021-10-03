#include "Entities.h"

#include "Components/AnchorSelector.h"
#include "Components/CameraGuide.h"
#include "Components/CameraTag.h"
#include "Components/Controllable.h"
#include <Components/GrappleControl.h>
#include "Components/Pendular.h"
#include "Components/PivotJoint.h"
#include "Components/PlayerData.h"
#include "Components/Position.h"
#include "Components/RopeCreator.h"
#include "Components/VisualOutline.h"
#include "Components/VisualRectangle.h"
#include "Components/Mass.h"

#include "Systems/ControlAnchorSight.h"
#include "Utils/PhysicsStructs.h"
#include "aunteater/EntityManager.h"
#include "commons.h"

#include <math/VectorUtilities.h>


namespace ad {
namespace grapito
{

aunteater::Entity makePlayer(int aIndex,
                             Controller aController,
                             Color aColor,
                             GrappleMode aGrappleMode)
{
    aunteater::Entity player = aunteater::Entity()
        // Needed for gravity
        //.add<Body>(
        //    math::Rectangle<double>{{0., 0.}, {2., 2.}},
        //    BodyType::DYNAMIC,
        //    ShapeType::HULL,
        //    player::gMass,
        //    0.)
        .add<Controllable>(aController)
        .add<GrappleControl>(aGrappleMode)
        .add<PlayerData>(aIndex, aColor)
        .add<AccelAndSpeed>()
        .add<Position>(Position2{3., 3.}, player::gSize) // The position will be set by pendulum simulation
        .add<Body>(
            math::Rectangle<double>{{0., 0.}, player::gSize},
            BodyType_Dynamic,
            ShapeType_Hull,
            CollisionType_Player,
            15.,
            0.,
            0.,
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


aunteater::Entity makeAnchor(math::Position<2, double> aPosition, math::Size<2, double> aSize)
{
    return aunteater::Entity()
        .add<Body>(
            math::Rectangle<double>{{0., 0.}, aSize},
            BodyType_Static,
            ShapeType_Hull,
            CollisionType_Static_Env,
            0.,
            0.,
            1.
            )
        .add<Position>(aPosition, aSize)
        .add<AccelAndSpeed>()
        .add<VisualRectangle>(anchor::gColor)
    ;
}


void connectGrapple(aunteater::weak_entity aEntity, Pendular aPendular)
{
    (*aEntity)
        .add<Pendular>(std::move(aPendular))
        ;

    aPendular.connected->add<CameraGuide>(
        math::makeInterpolation<math::ease::SmoothStep>(0., 1., camera::gAnchorGuideFadeIn));
}


void retractGrapple(aunteater::weak_entity aEntity)
{
    CameraGuide & guide = aEntity->get<Pendular>().connected->get<CameraGuide>();
    guide = CameraGuide{
        math::makeInterpolation<math::ease::SmoothStep>(guide.influence, 0., camera::gAnchorGuideFadeOut),
        CameraGuide::OnCompletion::Remove,
    };

    (*aEntity)
        .markComponentToRemove<Pendular>()
        ;

}


aunteater::weak_entity setGrappleMode(aunteater::weak_entity aEntity,
                                      const PlayerData & aPlayerData,
                                      GrappleMode aMode,
                                      aunteater::EntityManager & aEntityManager)
{
    (*aEntity)
        .get<GrappleControl>().mode = aMode;

    switch (aMode)
    {
    case GrappleMode::AnchorSight:
        aEntityManager.addEntity(
            aunteater::Entity()
                .add<AnchorSelector>(sight::gReachDistance, sight::gAngleTolerance, aEntity, nullptr)
                .add<Controllable>(aEntity->get<Controllable>())
                .add<Position>() // Will be handled by ControlAnchorSight system
                .add<VisualOutline>(aPlayerData.color, (aPlayerData.id + 1) * sight::gBaseOutlineOffset)
        );
        break;
    case GrappleMode::Closest:
        for(const auto anchorSight : aunteater::FamilyHelp<AnchorSight>{aEntityManager})
        {
            if (anchorSight->get<AnchorSelector>().player == aEntity)
            {
                anchorSight->markToRemove();
            }
        }
        break;
    }

    return aEntity;
}


Pendular makePendular(Position2 aGrappleOrigin,
                      Position2 aAnchorPoint,
                      aunteater::weak_entity aConnected,
                      Vec2 aCartesianSpeed,
                      double aGrappleLength)
{
    Vec2 grappleLine = aAnchorPoint - aGrappleOrigin;
    // grapple line goes from origin to anchor, we need the angle with -Y
    math::Radian<double> angle{std::atan2(-grappleLine.x(), grappleLine.y())};

    Vec2 tangent{grappleLine.y(), - grappleLine.x()};
    math::Radian<double> angularSpeed{ cos(math::getOrientedAngle(aCartesianSpeed, tangent)) 
                                       * aCartesianSpeed.getNorm() / aGrappleLength };

    return Pendular{aAnchorPoint, aConnected, angle, aGrappleLength, angularSpeed};
}


Pendular makePendular(aunteater::weak_entity aConnected,
                      double aRopeLength,
                      math::Radian<double> aInitialAngle)
{
    auto rectangle = aConnected->get<Position>().rectangle();
    return Pendular{
        (rectangle.bottomLeft() + rectangle.bottomRight().as<math::Vec>()) / 2.,
        aConnected,
        aInitialAngle,
        aRopeLength
    };
}

aunteater::Entity createRopeSegment(Position2 origin, Position2 end)
{
    Vec2 ropeVector = end - origin;
    double length = ropeVector.getNorm();
    math::Radian<double> angle{atan2(ropeVector.y(), ropeVector.x())};
    math::Size<2, double> size{length, .1};
    aunteater::Entity rope = aunteater::Entity()
            .add<Position>(
                Position2::Zero(),
                size
                )
            .add<Body>(
                math::Rectangle<double>{{0., 0.}, size},
                BodyType_Dynamic,
                ShapeType_Hull,
                CollisionType_Moving_Env,
                1.,
                angle.value(),
                0.,
                std::vector<CollisionType>{CollisionType_Static_Env}
                )
            .add<VisualRectangle>(math::sdr::gGreen)
            .add<AccelAndSpeed>();

    setLocalPointToWorldPos(rope, {0., 0.05}, origin);
    return rope;
}

void throwGrapple(aunteater::weak_entity aEntity, aunteater::EntityManager & aEntityManager)
{
    math::Size<2, double> size{.25, .25};
    Position2 playerPos = static_cast<Position2>(aEntity->get<Position>().position.as<math::Vec>() + aEntity->get<Body>().massCenter.as<math::Vec>() + Vec2{.5, .5});
    aEntity->get<PlayerData>().grapple = aEntityManager.addEntity(aunteater::Entity()
            .add<Position>(
                playerPos,
                size
                )
            .add<Body>(
                math::Rectangle<double>{{0., 0.}, size},
                BodyType_Dynamic,
                ShapeType_Hull,
                CollisionType_Moving_Env,
                5.,
                0.,
                1.,
                std::vector<CollisionType>{CollisionType_Static_Env}
                )
            .add<VisualRectangle>(math::sdr::gYellow)
            .add<AccelAndSpeed>(Vec2{20., 20.}, 0.)
            .add<RopeCreator>(aEntity)
            );
}

void attachPlayerToGrapple(aunteater::weak_entity aPlayer, aunteater::EntityManager & aEntityManager)
{
    assert(aPlayer->get<PlayerData>().grapple != nullptr);

    aunteater::weak_entity & grapple = aPlayer->get<PlayerData>().grapple;
    RopeCreator & ropeCreator = grapple->get<RopeCreator>();

    aunteater::weak_entity lastSegment = ropeCreator.mRopeSegments.back();
    Position2 end = static_cast<Position2>(aPlayer->get<Position>().position.as<math::Vec>() + aPlayer->get<Body>().massCenter.as<math::Vec>());
    Position2 origin = static_cast<Position2>(getLocalPointInWorld(lastSegment->get<Body>(), lastSegment->get<Position>(), {lastSegment->get<Position>().dimension.width(), 0.05}));
    double length = (end - origin).getNorm();

    aunteater::weak_entity link = aEntityManager.addEntity(createRopeSegment(
        origin,
        end
    ));

    aEntityManager.addEntity(
            aunteater::Entity()
            .add<PivotJoint>(
                Position2{0., 0.05},
                Position2{lastSegment->get<Position>().dimension.width(), 0.05},
                link,
                lastSegment
            ));
    aPlayer->get<PlayerData>().grappleAttachment = aEntityManager.addEntity(
            aunteater::Entity()
            .add<PivotJoint>(
                Position2{lastSegment->get<Position>().dimension.width(), 0.05},
                aPlayer->get<Body>().massCenter,
                link,
                aPlayer
            ));

    ropeCreator.mRopeSegments.push_back(link);
}

void detachPlayerFromGrapple(aunteater::weak_entity aPlayer)
{
    aPlayer->get<PlayerData>().grapple->get<RopeCreator>().mTargetEntity = nullptr;
    aPlayer->get<PlayerData>().grappleAttachment->markToRemove();
}

} // namespace grapito
} // namespace ad
