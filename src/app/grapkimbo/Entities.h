#pragma once


#include "Configuration.h"
#include "Input.h"

#include "Components/AccelAndSpeed.h"
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

aunteater::Entity makeCamera();


aunteater::Entity makeAnchor(math::Position<2, double> aPosition, math::Size<2, double> aSize);


Pendular makePendular(aunteater::weak_entity aConnected,
                      double aRopeLength = player::gInitialRopeLength,
                      math::Radian<double> aInitialAngle = player::gInitialAngle);


Pendular makePendular(math::Position<2, double> aGrappleOrigin,
                      math::Position<2, double> aAnchorPoint,
                      aunteater::weak_entity aConnected,
                      math::Vec<2, double> aCartesianSpeed,
                      double aGrappleLength);



inline Pendular makePendular(math::Position<2, double> aGrappleOrigin,
                             math::Position<2, double> aAnchorPoint,
                             aunteater::weak_entity aConnected,
                             math::Vec<2, double> aCartesianSpeed)
{
    return makePendular(aGrappleOrigin,
                        aAnchorPoint,
                        aConnected,
                        aCartesianSpeed,
                        (aAnchorPoint - aGrappleOrigin).getNorm());
}

void connectGrapple(aunteater::weak_entity aEntity, Pendular aPendular);


void retractGrapple(aunteater::weak_entity aEntity, AccelAndSpeed aForceAndSpeed);


aunteater::weak_entity setGrappleMode(aunteater::weak_entity aEntity,
                                      const PlayerData & aPlayerData,
                                      GrappleMode aMode,
                                      aunteater::EntityManager & aEntityManager);


} // namespace ad
