#include "GrappleInteractions.h"

#include "../Entities.h"
#include "../Logging.h"
#include "../Timer.h"

#include "../Utils/Grapple.h"

#include "../Components/PlayerData.h"
#include "../Components/RopeSegment.h"
#include "../Components/SoundPlayer.h"

#include <handy/StringId_Interning.h>


namespace ad {
namespace grapito {


const StringId soundId_CutRopeSid = handy::internalizeString("slash");


GrappleInteractions::GrappleInteractions(aunteater::EntityManager & aEntityManager) :
    mRopeCreators{aEntityManager},
    mPlayerDatas{aEntityManager}
{}


// Note Ad 2022/01/24: Yes, I came up with this name
// The Grapple can cut other only if it is currently being thrown 
// (i.e. connected to the player, but not to the environment) or if it is detached.
std::optional<int> getPlayerIdIfGrappleCanCut(const RopeCreator & aRopeCreator)
{
        if(isBeingThrown(aRopeCreator))
        {
            return aRopeCreator.mTargetEntity->get<PlayerData>().id;
        }
        else if(isDetached(aRopeCreator))
        {
            assert(aRopeCreator.mRopeSegments.size() > 0);
            return aRopeCreator.mRopeSegments.front()->get<RopeSegment>().playerId;
        }
        return std::nullopt;
}

void GrappleInteractions::update(const GrapitoTimer, const GameInputState &)
{
    // Generate the set of players to be detached.
    std::set<int> playersToDetach;
    for (const auto & [body, position, ropeCreator] : mRopeCreators)
    {
        // If the grapple is still connected to a player, but not to the environment
        if(auto grapplePlayerId = getPlayerIdIfGrappleCanCut(ropeCreator))
        {
            for (const auto & collisionPair : body.constructedBodyIt->contactList)
            {
                if (collisionPair->bodyA.collisionType == CollisionType_Moving_Env
                    && collisionPair->bodyB.collisionType == CollisionType_Moving_Env)
                {
                    aunteater::weak_entity collidedEntity = getOtherEntity(body, collisionPair);
                    if (collidedEntity->has<RopeSegment>() 
                        && collidedEntity->get<RopeSegment>().playerId != *grapplePlayerId)
                    {
                        int segmentPlayerId = collidedEntity->get<RopeSegment>().playerId;
                        ADLOG(info)("Grapple of p#{} collided the rope of p#{}.",
                                    *grapplePlayerId, segmentPlayerId);
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
            addSoundToEntity(player, soundId_CutRopeSid);
        }
    }
}


} // namespace grapito
} // namespace ad
