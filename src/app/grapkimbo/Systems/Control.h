#pragma once

#include "Input.h"

#include <Components/Controllable.h>
#include <Components/Body.h>
#include <Components/AccelAndSpeed.h>
#include <Components/GrappleControl.h>
#include <Components/Pendular.h>
#include <Components/PlayerData.h>
#include <Components/Position.h>
#include <Components/Mass.h>

#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/EntityManager.h>
#include <aunteater/System.h>

#include <utility>

namespace ad {
namespace grapito
{

typedef aunteater::Archetype<Controllable, Position, AccelAndSpeed, Mass> CartesianControlled;
typedef aunteater::Archetype<Controllable, Position, Pendular, Mass> PolarControlled;
typedef aunteater::Archetype<Controllable, AccelAndSpeed, GrappleControl, Position> Grappler;
typedef aunteater::Archetype<Controllable, GrappleControl, PlayerData> ModeSelectable; // est une bande de mecs sympas
// Currently using EnvironmentCollisionBox as a tag, because it is not correctly positioned...
typedef aunteater::Archetype<Position, Body> AnchorElement;

class Control : public aunteater::System<GameInputState>
{

public:
    Control(aunteater::EntityManager & aEntityManager);

    void update(const aunteater::Timer aTimer, const GameInputState & aInputState) override;

    static constexpr double gPendularControlAccelerationFactor = 1./6.;
    static constexpr double gAirControlAcceleration = 12.; // m/s

private:
    std::pair<Position2, double> anchor(const Position2 aPosition);

    aunteater::EntityManager & mEntityManager;
    const aunteater::FamilyHelp<CartesianControlled> mCartesianControllables;
    const aunteater::FamilyHelp<PolarControlled> mPolarControllables;
    const aunteater::FamilyHelp<Grappler> mGrapplers;
    const aunteater::FamilyHelp<ModeSelectable> mModeSelectables;
    const aunteater::FamilyHelp<AnchorElement> mAnchorables;

};

} // namespace grapito
} // namespace ad
