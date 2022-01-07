#include "GrappleJointCreator.h"

#include "../Configuration.h"
#include "../Entities.h"

#include "../Components/Body.h"
#include "../Components/Position.h"
#include "../Components/RopeCreator.h"
#include "../Components/PivotJoint.h"
#include "../Components/PlayerData.h"
#include "../Components/WeldJoint.h"
#include "../Components/DistanceJoint.h"
#include "../Components/SoundPlayer.h"

#include "aunteater/Entity.h"
#include "aunteater/globals.h"
#include "math/Rectangle.h"

#include <math/Vector.h>
#include <handy/StringId_Interning.h>

namespace ad {
namespace grapito
{

const StringId soundId_WeldSid = handy::internalizeString("weld");

GrappleJointCreator::GrappleJointCreator(aunteater::EntityManager & aEntityManager) :
    mRopeCreator{aEntityManager},
    mEntityManager{aEntityManager}
{}

void GrappleJointCreator::update(const GrapitoTimer, const GameInputState &)
{
    for (auto & ropeCreatorEntity : mRopeCreator)
    {
        Body & body = ropeCreatorEntity->get<Body>();
        Position & pos = ropeCreatorEntity->get<Position>();
        RopeCreator & rope = ropeCreatorEntity->get<RopeCreator>();
        ConstructedBody & cBody = *body.constructedBodyIt;
        aunteater::weak_entity player = rope.mTargetEntity;
        
        if (player != nullptr)
        {
            PlayerData & playerData = player->get<PlayerData>();

            Body & playerBody = player->get<Body>();
            float length = 0.f;

            for (auto ropeIt = rope.mRopeSegments.rbegin(); ropeIt != rope.mRopeSegments.rend(); ++ropeIt)
            {
                //Here we go from the end attache to the player to the grapple and try
                //to find a contact point to attach the player to
                //We attach the player directly to the env to relax the rope
                aunteater::weak_entity rope = *ropeIt;
                Body & ropeBody = rope->get<Body>();
                ConstructedBody & ropeConstructedBody = *ropeBody.constructedBodyIt;

                length += (ropeBody.shape.getVertice(0) - ropeBody.shape.getVertice(1)).getNorm();

                for (auto & collisionPair : ropeConstructedBody.contactList)
                {
                    if (
                        collisionPair->bodyA.collisionType == CollisionType_Static_Env ||
                        collisionPair->bodyB.collisionType == CollisionType_Static_Env
                    )
                    {
                        //We found a static env contact with the grapple
                        //We can weld the grapple to it
                        aunteater::weak_entity otherEntity;

                        if (&collisionPair->bodyA == &ropeConstructedBody)
                        {
                            otherEntity = collisionPair->bodyB.entity;
                        }
                        else
                        {
                            otherEntity = collisionPair->bodyA.entity;
                        }

                        if (collisionPair->manifold.contacts.size() > 0)
                        {
                            Position2 contactPoint = collisionPair->manifold.contacts[0].contactPoint;
                            Position2 localPointPlayer = playerBody.massCenter;
                            Position2 localPointOther = getWorldPointInLocal(otherEntity->get<Body>(), otherEntity->get<Position>(), contactPoint);

                            playerData.mGrappleDistanceJoint = mEntityManager.addEntity(
                                    aunteater::Entity()
                                    .add<DistanceJoint>(
                                        localPointPlayer,
                                        localPointOther,
                                        15.f,
                                        0.8f,
                                        1.f,
                                        0.5f,
                                        length * player::gRopeDistanceJointFactor,
                                        player,
                                        otherEntity
                                    ));
                            break;
                        }
                    }
                }
                
                if (playerData.mGrappleDistanceJoint)
                {
                    break;
                }
            }

            if (playerData.mGrappleWeldJoint == nullptr)
            {
                for (auto & collisionPair : cBody.contactList)
                {
                    //We go through all the collision pair the grapple is a part of
                    //and we try to find if it's in contact with a static environment
                    //if he is we weld the grapple to the static environment
                    if (
                        collisionPair->bodyA.collisionType == CollisionType_Static_Env ||
                        collisionPair->bodyB.collisionType == CollisionType_Static_Env
                    )
                    {
                        //We found a static env contact with the grapple
                        //We can weld the grapple to it
                        aunteater::weak_entity otherEntity;

                        if (&collisionPair->bodyA == &cBody)
                        {
                            otherEntity = collisionPair->bodyB.entity;
                        }
                        else
                        {
                            otherEntity = collisionPair->bodyA.entity;
                        }

                        if (collisionPair->manifold.contacts.size() > 0)
                        {
                            Position2 contactPoint = collisionPair->manifold.contacts[0].contactPoint;
                            Position2 localPointGrapple = getWorldPointInLocal(body, pos, contactPoint);
                            Position2 localPointOther = getWorldPointInLocal(otherEntity->get<Body>(), otherEntity->get<Position>(), contactPoint);
                            playerData.mGrappleWeldJoint = mEntityManager.addEntity(
                                    aunteater::Entity()
                                    .add<PivotJoint>(
                                        localPointGrapple,
                                        localPointOther,
                                        ropeCreatorEntity,
                                        otherEntity
                                    ));
                            addSoundToEntity(ropeCreatorEntity, soundId_WeldSid);
                            break;
                        }
                    }
                }
            }

            if (playerData.mGrappleDistanceJoint == nullptr && playerData.mGrappleWeldJoint != nullptr)
            {
                //If we did not successfuly attached the player
                //to the environment during the rope phase we attach the
                //player to the environment the grapple is welded to
                PivotJoint & pivotJoint = playerData.mGrappleWeldJoint->get<PivotJoint>();
                Position2 localPointPlayer = playerBody.massCenter;
                playerData.mGrappleDistanceJoint = mEntityManager.addEntity(
                        aunteater::Entity()
                        .add<DistanceJoint>(
                            localPointPlayer,
                            pivotJoint.localAnchorB,
                            15.f,
                            0.8f,
                            1.f,
                            0.5f,
                            length * player::gRopeDistanceJointFactor,
                            player,
                            pivotJoint.bodyB
                        ));
            }
        }
    }
}


} // namespace grapito
} // namespace ad
