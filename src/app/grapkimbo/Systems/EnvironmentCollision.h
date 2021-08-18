#pragma once

#include <Components/Position.h>
#include <Components/EnvironmentCollisionBox.h>
#include <Components/Controllable.h>

#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>

namespace ad {

typedef aunteater::Archetype<Position, EnvironmentCollisionBox> EnvironmentCollider;

class EnvironmentCollision : public aunteater::System<GameInputState>
{

public:
    EnvironmentCollision(aunteater::EntityManager & aEntityManager);

    void update(const aunteater::Timer aTimer, const GameInputState &) override;

    void getBestQuery(
        ContactPointQuery & bestQuery,
        CollisionBoxType ecbAType,
        CollisionBoxType ecbBType,
        CollisionBox ecbA,
        CollisionBox ecbB
    );

private:
    const aunteater::FamilyHelp<EnvironmentCollider> mColliders;
    std::array<
        std::array<
            std::function< ContactPointQuery(const CollisionBox & collisionBoxA, const CollisionBox & collisionBoxB)>,
            CollisionBoxType::type_count
        >,
        CollisionBoxType::type_count
    > queryFunctions;
};

} // namespace ad
