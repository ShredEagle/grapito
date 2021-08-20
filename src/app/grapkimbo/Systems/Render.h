#pragma once

#include "PendulumSimulation.h" // for pendulum archetype

#include <Components/Position.h>
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

typedef aunteater::Archetype<Position> Rendered;
#ifdef KIMBO_DEBUG
typedef aunteater::Archetype<EnvironmentCollisionBox> Collider;
#endif

class Render : public aunteater::System
{

public:
    Render(aunteater::Engine & aEngine, Application & aApplication);

    void update(const aunteater::Timer aTimer) override;

    static constexpr int gPixelsPerMeter = 40;

private:
    aunteater::Engine & mEngine;
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
