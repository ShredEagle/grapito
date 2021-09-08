#pragma once

#include <Components/Position.h>
#include <Components/Body.h>
#include <Components/Controllable.h>

#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>

namespace ad {
namespace grapito
{

typedef aunteater::Archetype<Position, Body> BodyPosition;

enum ReferenceFace
{
    FACEA,
    FACEB,
};

class ContactConstraintCreation : public aunteater::System<GameInputState>
{

public:
    ContactConstraintCreation(aunteater::EntityManager & aEntityManager);

    void update(const aunteater::Timer aTimer, const GameInputState & aInputState) override;

    ReferenceFace getBestQuery(
        ContactQuery & bestQuery,
        ShapeType ecbAType,
        ShapeType ecbBType,
        const CollisionBox & ecbA,
        const CollisionBox & ecbB,
        const Position2 massCenterA,
        const Position2 massCenterB,
        const math::Radian<double> thetaA,
        const math::Radian<double> thetaB
    );

private:
    const aunteater::FamilyHelp<BodyPosition> mColliders;
    std::array<
        std::array<
            std::function<void(
                    ContactQuery & query,
                    const CollisionBox & collisionBoxA,
                    const CollisionBox & collisionBoxB,
                    const Position2 massCenterA,
                    const Position2 massCenterB,
                    const math::Radian<double> thetaA,
                    const math::Radian<double> thetaB
                    )>,
            ShapeType::type_count
        >,
        ShapeType::type_count
    > queryFunctions;
};

} // namespace grapito
} // namespace ad
