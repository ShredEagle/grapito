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
                Position2{0., 0.05},
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
    for (auto & [ropeCreator, pos, body] : mRopeCreator)
    {
        aunteater::weak_entity player = ropeCreator.mTargetEntity;
        aunteater::weak_entity lastSegment = ropeCreator.mRopeSegments.back();
        Position2 end = static_cast<Position2>(player->get<Position>().position.as<math::Vec>() + player->get<Body>().massCenter.as<math::Vec>());
        Position2 origin = static_cast<Position2>(lastSegment->get<Position>().position.as<math::Vec>() + lastSegment->get<Body>().massCenter.as<math::Vec>() + lastSegment->get<Body>().shape.getVertice(0).as<math::Vec>());
        double length = (end - origin).getNorm();
        std::cout << length << "\n";
        debugDrawer->drawPoint({
                end,
                0.07,
                {255,200,0}
                });
        debugDrawer->drawPoint({
                origin,
                0.07,
                {255,200,0}
                });

        if (length > 1.)
        {
            aunteater::weak_entity link = mEntityManager.addEntity(createRopeSegment(
                origin,
                end
            ));

            mEntityManager.addEntity(
                    aunteater::Entity()
                    .add<PivotJoint>(
                        Position2{0., 0.05},
                        lastSegment->get<Body>().shape.getVertice(2),
                        link,
                        lastSegment
                    ));

            ropeCreator.mRopeSegments.push_back(link);
        }
        else
        {
            math::Size<2, double> size{length, .1};
            
            lastSegment->get<Position>().dimension = size;
            lastSegment->get<Body>().shape = math::Rectangle<double>{{0., 0.}, size};
            lastSegment->get<Body>().updateData();
            lastSegment->get<Body>().updateConstructedBody();
        }
    }
}
} // namespace grapito
} // namespace ad
