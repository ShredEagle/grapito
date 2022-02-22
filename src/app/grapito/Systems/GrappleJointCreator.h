#pragma once

#include "../Input.h"

#include <Components/Body.h>
#include <Components/Position.h>
#include <Components/RopeCreator.h>

#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>


namespace ad {
namespace grapito {

typedef aunteater::Archetype<Body, Position, RopeCreator> RopeCreatorArchetype;

class GrappleJointCreator : public aunteater::System<GrapitoTimer, GameInputState>
{

public:
    GrappleJointCreator(aunteater::EntityManager & aEntityManager);

    void update(const GrapitoTimer, const GameInputState &) override;

private:
    void handleThrow(RopeCreator & aRopeCreator);

    const aunteater::FamilyHelp<RopeCreatorArchetype> mRopeCreator;
    aunteater::EntityManager & mEntityManager;
};


/// \brief Return the other entity in a collision pair, i.e. the entity which does not have `aThisBody`. 
aunteater::weak_entity getOtherEntity(const Body & aThisBody, const CollisionPair * aCollisionPair);


} // namespace grapito
} // namespace ad
