#pragma once


#include "Configuration.h"
#include "Input.h"
#include "commons.h"

#include <Components/Body.h>
#include <Components/Mass.h>
#include <Components/Position.h>
#include <Components/AccelAndSpeed.h>
#include <Components/GrappleControl.h>
#include <Components/PlayerData.h>

#include <aunteater/Entity.h>
#include <aunteater/EntityManager.h>
#include <iostream>


namespace ad {
namespace grapito
{


aunteater::Entity makePlayer(int aIndex,
                             Controller aController,
                             math::sdr::Rgb aColor,
                             GrappleMode aGrappleMode = GrappleMode::Closest);

aunteater::Entity makeCamera(Position2 pos = {0.f, 0.f});


aunteater::Entity makeAnchor(Position2 aPosition, math::Size<2, float> aSize);
aunteater::Entity makeAnchor(Position2 aPosition, std::vector<Position2> aVertices);


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
    Position2 relativeLocalPoint= transformPosition(localPos, body.theta, body.massCenter);
    Vec2 posVectorDifference = worldPos - (pos.position + relativeLocalPoint.as<math::Vec>());

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
