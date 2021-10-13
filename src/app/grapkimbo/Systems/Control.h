#pragma once

#include "Input.h"

#include <Components/Controllable.h>
#include <Components/Body.h>
#include <Components/AccelAndSpeed.h>
#include <Components/GrappleControl.h>
#include <Components/PlayerData.h>
#include <Components/Position.h>
#include <Components/Mass.h>

#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/EntityManager.h>
#include <aunteater/System.h>

namespace ad {
namespace grapito
{

typedef aunteater::Archetype<Controllable, Position, AccelAndSpeed, Mass, PlayerData> CartesianControlled;
typedef aunteater::Archetype<Controllable, PlayerData> PolarControlled;
typedef aunteater::Archetype<Controllable, AccelAndSpeed, GrappleControl, Position, PlayerData> Grappler;

class Control : public aunteater::System<GrapitoTimer, GameInputState>
{

public:
    Control(aunteater::EntityManager & aEntityManager);

    void update(const GrapitoTimer aTimer, const GameInputState & aInputState) override;

    static constexpr double gPendularControlAccelerationFactor = 1./6.;
    static constexpr double gAirControlAcceleration = 12.; // m/s
    static constexpr double gJumpImpulse = 10.; // m/s

private:
    std::pair<Position2, double> anchor(const Position2 aPosition);

    aunteater::EntityManager & mEntityManager;
    const aunteater::FamilyHelp<CartesianControlled> mCartesianControllables;
    const aunteater::FamilyHelp<PolarControlled> mPolarControllables;
    const aunteater::FamilyHelp<Grappler> mGrapplers;

};

} // namespace grapito
} // namespace ad
