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

    Position2 end = static_cast<Position2>(player->get<Position>().position.as<math::Vec>() + player->get<Body>().massCenter.as<math::Vec>());
    Position2 origin = static_cast<Position2>(pos.position.as<math::Vec>() + body.massCenter.as<math::Vec>());

    double length = (end - origin).getNorm();
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

void RopeCreation::update(const aunteater::Timer aTimer, const GameInputState &)
{
    for (auto & ropeCreatorEntity : mRopeCreator)
    {
        RopeCreator & ropeCreator = ropeCreatorEntity->get<RopeCreator>();
        aunteater::weak_entity player = ropeCreator.mTargetEntity;
        if (player != nullptr && player->get<PlayerData>().controlState & ControlState_Throwing)
        {
            aunteater::weak_entity lastSegment = ropeCreator.mRopeSegments.back();
            Position2 end = static_cast<Position2>(player->get<Position>().position.as<math::Vec>() + player->get<Body>().massCenter.as<math::Vec>());
            Position2 origin = static_cast<Position2>(getLocalPointInWorld(lastSegment->get<Body>(), lastSegment->get<Position>(), {lastSegment->get<Position>().dimension.width(), rope::ropeHalfwidth}));
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

                ropeCreator.mRopeSegments.push_back(link);
            }
        }
    }
}
} // namespace grapito
} // namespace ad
