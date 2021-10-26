#include "GrappleJointCreator.h"

#include "../Configuration.h"

#include "../Components/Body.h"
#include "../Components/Position.h"
#include "../Components/RopeCreator.h"
#include "../Components/PivotJoint.h"
#include "../Components/PlayerData.h"
#include "../Components/WeldJoint.h"
#include "../Components/DistanceJoint.h"

#include "aunteater/Entity.h"
#include "aunteater/globals.h"
#include "math/Rectangle.h"

#include <math/Vector.h>

namespace ad {
namespace grapito
{

GrappleJointCreator::GrappleJointCreator(aunteater::EntityManager & aEntityManager) :
    mRopeCreator{aEntityManager},
    mEntityManager{aEntityManager}
{}

void GrappleJointCreator::update(const GrapitoTimer aTimer, const GameInputState &)
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

            Position & playerPos = player->get<Position>();
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
                            Position2 localPointOther = static_cast<Position2>(contactPoint - otherEntity->get<Position>().position);

                            playerData.mGrappleDistanceJoint = mEntityManager.addEntity(
                                    aunteater::Entity()
                                    .add<DistanceJoint>(
                                        localPointPlayer,
                                        localPointOther,
                                        30.f,
                                        0.f,
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
                            Position2 localPointGrapple = static_cast<Position2>(contactPoint - pos.position);
                            Position2 localPointOther = static_cast<Position2>(contactPoint - otherEntity->get<Position>().position);

                            playerData.mGrappleWeldJoint = mEntityManager.addEntity(
                                    aunteater::Entity()
                                    .add<WeldJoint>(
                                        localPointGrapple,
                                        localPointOther,
                                        0.f,
                                        0.f,
                                        ropeCreatorEntity,
                                        otherEntity
                                    ));

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
                WeldJoint & weldJoint = playerData.mGrappleWeldJoint->get<WeldJoint>();
                Position2 localPointPlayer = playerBody.massCenter;
                playerData.mGrappleDistanceJoint = mEntityManager.addEntity(
                        aunteater::Entity()
                        .add<DistanceJoint>(
                            localPointPlayer,
                            weldJoint.localAnchorB,
                            2.f,
                            1.0f,
                            1.f,
                            0.5f,
                            length * player::gRopeDistanceJointFactor,
                            player,
                            weldJoint.bodyB
                        ));
            }
        }
    }
}


} // namespace grapito
} // namespace ad
