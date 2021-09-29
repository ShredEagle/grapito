#include "Entities.h"

#include "Components/AnchorSelector.h"
#include "Components/CameraGuide.h"
#include "Components/CameraTag.h"
#include "Components/Controllable.h"
#include <Components/GrappleControl.h>
#include "Components/Pendular.h"
#include "Components/PlayerData.h"
#include "Components/Position.h"
#include "Components/VisualOutline.h"
#include "Components/VisualRectangle.h"
#include "Components/Mass.h"

#include "Systems/ControlAnchorSight.h"
#include "commons.h"

#include <math/VectorUtilities.h>


namespace ad {
namespace grapito
{

aunteater::Entity makePlayer(int aIndex,
                             Controller aController,
                             Color aColor,
                             Pendular aPendular,
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
        .add<Pendular>(std::move(aPendular))
        .add<PlayerData>(aIndex, aColor)
        .add<AccelAndSpeed>()
        .add<Position>(Position2{3., 3.}, player::gSize) // The position will be set by pendulum simulation
        .add<Body>(
            math::Rectangle<double>{{0., 0.}, player::gSize},
            BodyType_Dynamic,
            ShapeType_Hull,
            CollisionType_Player,
            1.,
            0.
            )
        .add<VisualRectangle>(aColor)
        .add<Mass>(player::gMass)
    ;

    aPendular.connected->add<CameraGuide>(math::makeInterpolation<math::ease::SmoothStep>(0., 1., 0.3));

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
            CollisionType_Static_Env
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


} // namespace grapito
} // namespace ad
