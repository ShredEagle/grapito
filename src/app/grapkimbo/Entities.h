#pragma once


#include "Components/Body.h"
#include "Components/Mass.h"
#include "Components/Position.h"
#include "Configuration.h"
#include "Input.h"

#include "Components/AccelAndSpeed.h"
#include "Components/GrappleControl.h"
#include "Components/Pendular.h"
#include "Components/PlayerData.h"
#include "aunteater/Entity.h"
#include "commons.h"

#include <aunteater/EntityManager.h>
#include <iostream>


namespace ad {
namespace grapito
{


aunteater::Entity makePlayer(int aIndex,
                             Controller aController,
                             Color aColor,
                             GrappleMode aGrappleMode = GrappleMode::Closest);

aunteater::Entity makeCamera(Position2 pos = {0., 0.});


aunteater::Entity makeAnchor(math::Position<2, double> aPosition, math::Size<2, double> aSize);


Pendular makePendular(aunteater::weak_entity aConnected,
                      double aRopeLength = player::gInitialRopeLength,
                      math::Radian<double> aInitialAngle = player::gInitialAngle);


Pendular makePendular(Position2 aGrappleOrigin,
                      Position2 aAnchorPoint,
                      aunteater::weak_entity aConnected,
                      math::Vec<2, double> aCartesianSpeed,
                      double aGrappleLength);


inline Pendular makePendular(Position2 aGrappleOrigin,
                             Position2 aAnchorPoint,
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


void retractGrapple(aunteater::weak_entity aEntity);

aunteater::Entity createRopeSegment(Position2 origin, Position2 endRR);

void throwGrapple(aunteater::weak_entity aEntity, aunteater::EntityManager & aEntityManager);

void attachPlayerToGrapple(aunteater::weak_entity aPlayer, aunteater::EntityManager & aEntityManager);

void detachPlayerFromGrapple(aunteater::weak_entity aPlayer);

aunteater::weak_entity setGrappleMode(aunteater::weak_entity aEntity,
                                      const PlayerData & aPlayerData,
                                      GrappleMode aMode,
                                      aunteater::EntityManager & aEntityManager);

inline void setLocalPointToWorldPos(Body & body, Position & pos, Position2 localPos, Position2 worldPos)
{
    Position2 massCenterWorldPos = static_cast<Position2>(pos.position.as<math::Vec>() + body.massCenter.as<math::Vec>());
    Vec2 relativeLocalPointVector = transformPosition(localPos, body.theta, body.massCenter) - body.massCenter;
    Vec2 posVectorDifference = worldPos - (massCenterWorldPos + relativeLocalPointVector);

    pos.position += posVectorDifference;
}

inline void setLocalPointToWorldPos(aunteater::weak_entity aEntity, Position2 localPos, Position2 worldPos)
{
    Position & pos = aEntity->get<Position>();
    Body & body = aEntity->get<Body>();
    setLocalPointToWorldPos(body, pos, localPos, worldPos);
}

inline void setLocalPointToWorldPos(aunteater::Entity & aEntity, Position2 localPos, Position2 worldPos)
{
    Position & pos = aEntity.get<Position>();
    Body & body = aEntity.get<Body>();
    setLocalPointToWorldPos(body, pos, localPos, worldPos);
}

inline Position2 getLocalPointInWorld(Body & body, Position & pos, Position2 localPos)
{
    return transformPosition(pos.position + localPos.as<math::Vec>(), body.theta, pos.position + body.massCenter.as<math::Vec>());
}

} // namespace grapito
} // namespace ad
