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

class GrappleJointCreator : public aunteater::System<GrapitoTimer, GameInputState>
{

public:
    GrappleJointCreator(aunteater::EntityManager & aEntityManager);

    void update(const GrapitoTimer, const GameInputState &) override;

private:
    void handleThrow(RopeCreator & aRopeCreator);

    const aunteater::FamilyHelp<RopeCreatorType> mRopeCreator;
    aunteater::EntityManager & mEntityManager;
};


} // namespace grapito
} // namespace ad
