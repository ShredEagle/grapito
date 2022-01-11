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


aunteater::Entity makeCamera(Position2 pos = {0.f, 0.f});

aunteater::Entity makeDirectControllable(Controller aController,
                                         Position2 aInitialPosition = {0.f, 0.f});
                             

aunteater::Entity makeHudText(std::string aMessage, Position2 aScreenPosition);

aunteater::Entity makePlayer(int aIndex,
                             Controller aController,
                             math::sdr::Rgb aColor,
                             GrappleMode aGrappleMode = GrappleMode::Closest);

void kill(aunteater::weak_entity aPlayer);

aunteater::Entity makeAnchor(Position2 aPosition, math::Size<2, float> aSize);
aunteater::Entity makeAnchor(Position2 aPosition, std::vector<Position2> aVertices);


aunteater::Entity createRopeSegment(Position2 origin, Position2 end, aunteater::weak_entity aPlayer);

void throwGrapple(aunteater::weak_entity aPlayer, aunteater::EntityManager & aEntityManager);

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

inline Position2 getWorldPointInLocal(Body& body, Position& pos, Position2 worldPos)
{
    return static_cast<Position2>(transformPosition(worldPos, -body.theta, pos.position + body.massCenter.as<math::Vec>()) - pos.position);
}

} // namespace grapito
} // namespace ad
