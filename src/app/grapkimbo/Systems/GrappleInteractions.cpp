#include "GrappleInteractions.h"

#include "../Entities.h"
#include "../Logging.h"
#include "../Timer.h"

#include "../Utils/Grapple.h"

#include "../Components/PlayerData.h"
#include "../Components/RopeSegment.h"



namespace ad {
namespace grapito {


GrappleInteractions::GrappleInteractions(aunteater::EntityManager & aEntityManager) :
    mRopeCreators{aEntityManager},
    mPlayerDatas{aEntityManager}
{}


void GrappleInteractions::update(const GrapitoTimer, const GameInputState &)
{
    // Generate the set of players to be detached.
    std::set<int> playersToDetach;
    for (const auto & [body, position, ropeCreator] : mRopeCreators)
    {
        // If the grapple is still connected to a player, but not to the environment
        if(isBeingThrown(ropeCreator))
        {
            int grapplePlayerId = ropeCreator.mTargetEntity->get<PlayerData>().id;
            for (const auto & collisionPair : body.constructedBodyIt->contactList)
            {
                //ADLOG(info)("Collided other");
                if (collisionPair->bodyA.collisionType == CollisionType_Moving_Env
                    && collisionPair->bodyB.collisionType == CollisionType_Moving_Env)
                {
                    aunteater::weak_entity collidedEntity = getOtherEntity(body, collisionPair);
                    if (collidedEntity->has<RopeSegment>() 
                        && collidedEntity->get<RopeSegment>().playerId != grapplePlayerId)
                    {
                        int segmentPlayerId = collidedEntity->get<RopeSegment>().playerId;
                        ADLOG(info)("Grapple of p#{} collided the rope of p#{}.",
                                    grapplePlayerId, segmentPlayerId);
                        playersToDetach.insert(segmentPlayerId);
                    }
                }
            }
        }
    }

    // Actually cut the ropes.
    for (auto player : mPlayerDatas)
    {
        const auto & [playerData] = player;
        if(playersToDetach.contains(playerData.id))
        {
            detachPlayerFromGrapple(player);
        }
    }
}


} // namespace grapito
} // namespace ad
