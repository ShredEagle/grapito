#pragma once

#include "Input.h"

#include <Components/AccelAndSpeed.h>
#include <Components/Body.h>
#include <Components/Controllable.h>
#include <Components/Mass.h>
#include <Components/PlayerData.h>
#include <Components/Position.h>

#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/EntityManager.h>
#include <aunteater/System.h>

namespace ad {
namespace grapito
{

typedef aunteater::Archetype<Controllable, Position, AccelAndSpeed, Mass, PlayerData, Body> CartesianControlled;
typedef aunteater::Archetype<Controllable, AccelAndSpeed, Position, PlayerData> Grappler;

class Control : public aunteater::System<GrapitoTimer, GameInputState>
{

public:
    Control(aunteater::EntityManager & aEntityManager, std::shared_ptr<Context> aContext);

    void update(const GrapitoTimer, const GameInputState & aInputState) override;

    void switchGrappling(bool aEnabled)
    { mGrapplingEnabled = aEnabled; }

private:
    std::pair<Position2, double> anchor(const Position2 aPosition);

    aunteater::EntityManager & mEntityManager;
    std::shared_ptr<Context> mContext;
    const aunteater::FamilyHelp<CartesianControlled> mCartesianControllables;
    const aunteater::FamilyHelp<Grappler> mGrapplers;

    bool mGrapplingEnabled{true};
};

} // namespace grapito
} // namespace ad
