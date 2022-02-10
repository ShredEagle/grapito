#include "RopeCreation.h"

#include "../Entities.h"
#include "../Timer.h"

#include "Components/Body.h"
#include "Components/PivotJoint.h"
#include "Components/Position.h"
#include "Components/RopeCreator.h"
#include "Components/SoundPlayer.h"

#include "aunteater/Entity.h"
#include "aunteater/globals.h"
#include "math/Rectangle.h"

#include <math/Vector.h>

#include <handy/StringId_Interning.h>

namespace ad {
namespace grapito
{

const StringId soundId_LaunchSid = handy::internalizeString("launch");

RopeCreation::RopeCreation(aunteater::EntityManager & aEntityManager) :
    mRopeCreator{aEntityManager},
    mEntityManager{aEntityManager}
{
    mEntityManager.getFamily<RopeCreatorType>().registerObserver(this);
}

void RopeCreation::addedEntity(aunteater::LiveEntity & aEntity)
{
    RopeCreator & ropeCreator = aEntity.get<RopeCreator>();
    Position & pos = aEntity.get<Position>();
    aunteater::weak_entity player = ropeCreator.mTargetEntity;

    Position2 end = player->get<Position>().position + player->get<Body>().massCenter.as<math::Vec>();
    Position2 origin = pos.position;
    addSoundToEntity(player, soundId_LaunchSid);

    aunteater::weak_entity link = mEntityManager.addEntity(
        createRopeSegment(origin, end, player));

    aunteater::weak_entity joint = mEntityManager.addEntity(
            aunteater::Entity()
            .add<PivotJoint>(
                Position2{0.f, rope::ropeHalfwidth},
                Position2{0.f, 0.f},
                link,
                &aEntity
            ));
    ropeCreator.mRopeSegments.push_back(link);
    ropeCreator.mPivotJoints.push_back(joint);
}

void RopeCreation::removedEntity(aunteater::LiveEntity & aEntity)
{
    RopeCreator & creator = aEntity.get<RopeCreator>();
    for(auto link : creator.mRopeSegments)
    {
        link->markToRemove();
    }

    for(auto joint : creator.mPivotJoints)
    {
        joint->markToRemove();
    }
}

void RopeCreation::handleThrow(aunteater::weak_entity aRopeCreatorEntity)
{
        RopeCreator & ropeCreator = aRopeCreatorEntity->get<RopeCreator>();
        aunteater::weak_entity player = ropeCreator.mTargetEntity;
        if (player != nullptr && player->get<PlayerData>().controlState & ControlState_Throwing)
        {
            aunteater::weak_entity lastSegment = aRopeCreator.mRopeSegments.back();
            Position2 playerMassCenter = player->get<Position>().position + player->get<Body>().massCenter.as<math::Vec>();
            Position2 lastSegmentPlayerEnd = getLocalPointInWorld(lastSegment->get<Body>(), lastSegment->get<Position>(),
                                                    {lastSegment->get<Position>().dimension.width(),
                                                    rope::ropeHalfwidth});
            float length = (lastSegmentPlayerEnd - playerMassCenter).getNorm();

            if (length > .4f)
            {
                while (length > 0.f)
                {
                    lastSegment = ropeCreator.mRopeSegments.back();
                    lastSegmentPlayerEnd = getLocalPointInWorld(lastSegment->get<Body>(), lastSegment->get<Position>(),
                                                        {lastSegment->get<Position>().dimension.width(),
                                                        rope::ropeHalfwidth});
                    aunteater::weak_entity link;
                    if (length > .4f)
                    {
                        Vec2 direction = (playerMassCenter  - lastSegmentPlayerEnd).normalize();
                        link = mEntityManager.addEntity(
                            createRopeSegment(lastSegmentPlayerEnd, lastSegmentPlayerEnd + direction * .4f, player));
                    }
                    else
                    {
                        link = mEntityManager.addEntity(
                            createRopeSegment(lastSegmentPlayerEnd, playerMassCenter, player));
                    }

                    aunteater::weak_entity joint = mEntityManager.addEntity(
                            aunteater::Entity()
                            .add<PivotJoint>(
                                Position2{0.f, rope::ropeHalfwidth},
                                Position2{lastSegment->get<Position>().dimension.width(), rope::ropeHalfwidth},
                                link,
                                lastSegment
                            ));

                aRopeCreator.mRopeSegments.push_back(link);
                aRopeCreator.mPivotJoints.push_back(joint);
                    length -= .4f;
                }
            }
        }
}


void RopeCreation::update(const GrapitoTimer, const GameInputState &)
{
    for (auto & ropeCreatorEntity : mRopeCreator)
    {
        RopeCreator & ropeCreator = ropeCreatorEntity->get<RopeCreator>();
        handleThrow(ropeCreator);
    }
}


} // namespace grapito
} // namespace ad
