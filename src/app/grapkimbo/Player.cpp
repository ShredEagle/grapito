#include "Player.h"

#include "Components/AnchorSelector.h"
#include "Components/Controllable.h"
#include <Components/GrappleControl.h>
#include "Components/Pendular.h"
#include "Components/PlayerData.h"
#include "Components/Position.h"
#include "Components/VisualOutline.h"
#include "Components/VisualRectangle.h"
#include "Components/Weight.h"

#include "Systems/ControlAnchorSight.h"

#include <math/VectorUtilities.h>


namespace ad {


aunteater::Entity makePlayer(int aIndex,
                             Controller aController,
                             Color aColor,
                             Pendular aPendular,
                             GrappleMode aGrappleMode)
{
    return aunteater::Entity()
        .add<Controllable>(aController)
        .add<GrappleControl>(aGrappleMode)
        .add<Pendular>(std::move(aPendular))
        .add<PlayerData>(aIndex, aColor)
        .add<Position>(math::Position<2, double>{0., 0.}, player::gSize) // The position will be set by pendulum simulation
        .add<VisualRectangle>(aColor)
        .add<Weight>(player::gMass)
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


Pendular makePendular(math::Position<2, double> aGrappleOrigin,
                      math::Position<2, double> aAnchorPoint,
                      math::Vec<2, double> aCartesianSpeed,
                      double aGrappleLength)
{
    math::Vec<2, double> grappleLine = aAnchorPoint - aGrappleOrigin;
    // grapple line goes from origin to anchor, we need the angle with -Y
    math::Radian<double> angle{std::atan2(-grappleLine.x(), grappleLine.y())};

    math::Vec<2, double> tangent{grappleLine.y(), - grappleLine.x()};
    math::Radian<double> angularSpeed{ cos(math::getOrientedAngle(aCartesianSpeed, tangent)) 
                                       * aCartesianSpeed.getNorm() / aGrappleLength };

    return Pendular{aAnchorPoint, angle, aGrappleLength, angularSpeed};
}


Pendular makePendular(aunteater::weak_entity aAnchor,
                      double aRopeLength,
                      math::Radian<double> aInitialAngle)
{
    auto rectangle = aAnchor->get<Position>().rectangle();
    return Pendular{
        (rectangle.bottomLeft() + rectangle.bottomRight().as<math::Vec>()) / 2.,
        aInitialAngle,
        aRopeLength
    };
}


} // namespace ad
