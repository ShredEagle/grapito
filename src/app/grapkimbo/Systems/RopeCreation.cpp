#include "RopeCreation.h"

#include "Components/PivotJoint.h"
#include "Entities.h"

#include "Components/Body.h"
#include "Components/Position.h"
#include "Components/RopeCreator.h"
#include "aunteater/Entity.h"
#include "aunteater/globals.h"
#include "math/Rectangle.h"

#include <math/Vector.h>

namespace ad {
namespace grapito
{

RopeCreation::RopeCreation(aunteater::EntityManager & aEntityManager) :
    mRopeCreator{aEntityManager},
    mEntityManager{aEntityManager}
{
    mEntityManager.getFamily<RopeCreatorType>().registerObserver(this);
}

void RopeCreation::addedEntity(aunteater::LiveEntity & aEntity)
{
    RopeCreator & ropeCreator = aEntity.get<RopeCreator>();
    Body & body = aEntity.get<Body>();
    Position & pos = aEntity.get<Position>();
    aunteater::weak_entity player = ropeCreator.mTargetEntity;

    Position2 end = player->get<Position>().position + player->get<Body>().massCenter.as<math::Vec>();
    Position2 origin = pos.position + body.massCenter.as<math::Vec>();

    aunteater::weak_entity link = mEntityManager.addEntity(createRopeSegment(
        origin,
        end
    ));

    /*mEntityManager.addEntity(
            aunteater::Entity()
            .add<PivotJoint>(
                Position2{0., 0.},
                player->get<Body>().massCenter,
                link,
                player
            ));*/
    mEntityManager.addEntity(
            aunteater::Entity()
            .add<PivotJoint>(
                Position2{0., rope::ropeHalfwidth},
                body.massCenter,
                link,
                &aEntity
            ));
    ropeCreator.mRopeSegments.push_back(link);
}

void RopeCreation::removedEntity(aunteater::LiveEntity & aEntity)
{
}

void RopeCreation::handleThrow(RopeCreator & aRopeCreator)
{
        aunteater::weak_entity player = aRopeCreator.mTargetEntity;
        if (player != nullptr && player->get<PlayerData>().controlState & ControlState_Throwing)
        {
            aunteater::weak_entity lastSegment = aRopeCreator.mRopeSegments.back();
            Position2 end = player->get<Position>().position + player->get<Body>().massCenter.as<math::Vec>();
            Position2 origin = getLocalPointInWorld(lastSegment->get<Body>(), lastSegment->get<Position>(),
                                                    {lastSegment->get<Position>().dimension.width(),
                                                    rope::ropeHalfwidth});
            double length = (end - origin).getNorm();

            if (length > .4)
            {
                aunteater::weak_entity link = mEntityManager.addEntity(createRopeSegment(
                    origin,
                    end
                ));

                mEntityManager.addEntity(
                        aunteater::Entity()
                        .add<PivotJoint>(
                            Position2{0., rope::ropeHalfwidth},
                            Position2{lastSegment->get<Position>().dimension.width(), rope::ropeHalfwidth},
                            link,
                            lastSegment
                        ));

                aRopeCreator.mRopeSegments.push_back(link);
            }
        }
}


void RopeCreation::update(const aunteater::Timer aTimer, const GameInputState &)
{
    for (auto & ropeCreatorEntity : mRopeCreator)
    {
        RopeCreator & ropeCreator = ropeCreatorEntity->get<RopeCreator>();
        handleThrow(ropeCreator);
    }
}


} // namespace grapito
} // namespace ad