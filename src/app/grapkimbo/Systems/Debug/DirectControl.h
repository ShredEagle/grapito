#pragma once

#include <Components/Controllable.h>
#include <Components/Position.h>
#include <Components/Debug/DirectControlTag.h>

#include <aunteater/EntityManager.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>


namespace ad {
namespace grapito {
namespace debug {


using DirectControllable = aunteater::Archetype<Controllable, DirectControlTag, Position>;


class DirectControl : public aunteater::System<GrapitoTimer, GameInputState>
{
public:
    DirectControl(aunteater::EntityManager & aEntityManager);

    void update(const GrapitoTimer aTimer, const GameInputState & aInputState) override;

private:
    const aunteater::FamilyHelp<DirectControllable> mDirectControllables;

};


} // namespace debug
} // namespace grapito
} // namespace ad
