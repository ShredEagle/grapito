#pragma once

#include <Components/Position.h>
#include <Components/EnvironmentCollisionBox.h>
#include <Components/Controllable.h>
#include <Components/Environment.h>

#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>

namespace ad {

typedef aunteater::Archetype<Position, EnvironmentCollisionBox, Controllable> EnvironmentCollider;
typedef aunteater::Archetype<Position, EnvironmentCollisionBox, Environment> EnvironmentTerrain;

class EnvironmentCollision : public aunteater::System<GameInputState>
{

public:
    EnvironmentCollision(aunteater::EntityManager & aEntityManager);

    void update(const aunteater::Timer aTimer, const GameInputState &) override;

private:
    aunteater::EntityManager & mEntityManager;
    const aunteater::FamilyHelp<EnvironmentCollider> mColliders;
    const aunteater::FamilyHelp<EnvironmentTerrain> mEnvironments;

};

} // namespace ad
