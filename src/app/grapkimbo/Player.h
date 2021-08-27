#pragma once


#include "Configuration.h"
#include "Input.h"

#include "Components/ForceAndSpeed.h"
#include "Components/GrappleControl.h"
#include "Components/Pendular.h"
#include "Components/PlayerData.h"

#include <aunteater/EntityManager.h>


namespace ad {


aunteater::Entity makePlayer(int aIndex,
                             Controller aController,
                             Color aColor,
                             Pendular aPendular,
                             GrappleMode aGrappleMode = GrappleMode::Closest);

Pendular makePendular(aunteater::weak_entity aAnchor,
                      double aRopeLength = player::gInitialRopeLength,
                      math::Radian<double> aInitialAngle = player::gInitialAngle);


Pendular makePendular(math::Position<2, double> aGrappleOrigin,
                      math::Position<2, double> aAnchorPoint,
                      math::Vec<2, double> aCartesianSpeed,
                      double aGrappleLength);


inline Pendular makePendular(math::Position<2, double> aGrappleOrigin, math::Position<2, double> aAnchorPoint, math::Vec<2, double> aCartesianSpeed)
{
    return makePendular(aGrappleOrigin, aAnchorPoint, aCartesianSpeed, (aAnchorPoint - aGrappleOrigin).getNorm());
}

inline void connectGrapple(aunteater::weak_entity aEntity, Pendular aPendular)
{
    (*aEntity)
        .markComponentToRemove<ForceAndSpeed>()
        .add<Pendular>(std::move(aPendular))
        ;
}


inline void retractGrapple(aunteater::weak_entity aEntity, ForceAndSpeed aForceAndSpeed)
{
    (*aEntity)
        .markComponentToRemove<Pendular>()
        .add<ForceAndSpeed>(std::move(aForceAndSpeed))
        ;
}


aunteater::weak_entity setGrappleMode(aunteater::weak_entity aEntity,
                                      const PlayerData & aPlayerData,
                                      GrappleMode aMode,
                                      aunteater::EntityManager & aEntityManager);


} // namespace ad
