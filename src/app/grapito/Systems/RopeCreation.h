#pragma once

#include "../Input.h"

#include <Components/Body.h>
#include <Components/Position.h>
#include <Components/RopeCreator.h>

#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>


namespace ad {
namespace grapito
{

typedef aunteater::Archetype<RopeCreator, Position, Body> RopeCreatorType;

class RopeCreation : public aunteater::System<GrapitoTimer, GameInputState>, public aunteater::FamilyObserver
{

public:
    RopeCreation(aunteater::EntityManager & aEntityManager);

    void update(const GrapitoTimer aTimer, const GameInputState &) override;

    void addedEntity(aunteater::LiveEntity & aEntity) override;

    void removedEntity(aunteater::LiveEntity & aEntity) override;

private:
    void handleThrow(aunteater::weak_entity aRopeCreatorEntity);

    const aunteater::FamilyHelp<RopeCreatorType> mRopeCreator;
    aunteater::EntityManager & mEntityManager;
};


} // namespace grapito
} // namespace ad
