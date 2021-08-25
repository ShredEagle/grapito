#pragma once

#include "PendulumSimulation.h" // for pendulum archetype

#include <Components/Position.h>
#include <Components/VisualRectangle.h>
#ifdef KIMBO_DEBUG
#include <Components/EnvironmentCollisionBox.h>
#endif

#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>

#include <engine/TrivialLineStrip.h>
#include <engine/TrivialShaping.h>
#include <engine/Application.h>

namespace ad {

typedef aunteater::Archetype<Position, VisualRectangle> Rendered;
#ifdef KIMBO_DEBUG
typedef aunteater::Archetype<EnvironmentCollisionBox> Collider;
#endif

class Render : public aunteater::System<GameInputState>
{

public:
    Render(aunteater::EntityManager & aEntityManager, Application & aApplication);

    void update(const aunteater::Timer aTimer, const GameInputState &) override;

    static constexpr int gPixelsPerMeter = 40;

private:
    aunteater::EntityManager & mEntityManager;
    const aunteater::FamilyHelp<Rendered> mRenderables;
    const aunteater::FamilyHelp<Pendulum> mPendulums;
    Application & mApplication;
    TrivialShaping mTrivialShaping;
    TrivialLineStrip mTrivialLineStrip;
#ifdef KIMBO_DEBUG
    const aunteater::FamilyHelp<Collider> mColliders;
#endif

};

} // namespace ad
