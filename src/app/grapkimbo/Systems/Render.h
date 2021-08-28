#pragma once

#include "PendulumSimulation.h" // for pendulum archetype

#include <Components/CameraGuide.h>
#include <Components/Position.h>
#include <Components/VisualOutline.h>
#include <Components/VisualRectangle.h>
#ifdef KIMBO_DEBUG
#include <Components/Body.h>
#endif

#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>

#include <engine/TrivialLineStrip.h>
#include <engine/TrivialShaping.h>
#include <engine/DrawLine.h>
#include <engine/Application.h>

namespace ad {

typedef aunteater::Archetype<Position, VisualRectangle> RenderedRectangle;
typedef aunteater::Archetype<Position, VisualOutline> RenderedOutline;
typedef aunteater::Archetype<CameraGuide, Position> Guide;

#ifdef KIMBO_DEBUG
typedef aunteater::Archetype<Body> Collider;
#endif

class Render : public aunteater::System<GameInputState>
{

public:
    Render(aunteater::EntityManager & aEntityManager, Application & aApplication);

    void update(const aunteater::Timer aTimer, const GameInputState &) override;

private:
    aunteater::EntityManager & mEntityManager;
    const aunteater::FamilyHelp<RenderedRectangle> mRectangles;
    const aunteater::FamilyHelp<RenderedOutline> mOutlines;
    const aunteater::FamilyHelp<Pendulum> mPendulums;
    const aunteater::FamilyHelp<Guide> mGuides;
    std::shared_ptr<Engine> mEngine;
    TrivialShaping mTrivialShaping;
    TrivialLineStrip mTrivialLineStrip;
#ifdef KIMBO_DEBUG
    const aunteater::FamilyHelp<Collider> mColliders;
#endif

};

} // namespace ad
