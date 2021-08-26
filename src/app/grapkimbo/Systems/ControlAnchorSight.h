#pragma once

#include <Components/AnchorSelector.h>
#include <Components/Controllable.h>
#include <Components/Position.h>

#include <Systems/Control.h> // for AnchorElement archetype

#include <aunteater/EntityManager.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>


namespace ad {


using AnchorSight = aunteater::Archetype<AnchorSelector, Controllable, Position>;


class ControlAnchorSight : public aunteater::System<GameInputState>
{
public:
    ControlAnchorSight(aunteater::EntityManager &aEngine);

    void update(const aunteater::Timer aTimer, const GameInputState & aInputState) override;

private:
    const aunteater::FamilyHelp<AnchorSight> mAnchorSights;
    const aunteater::FamilyHelp<AnchorElement> mAnchorables;
};


} // namespace ad