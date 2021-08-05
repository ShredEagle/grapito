#pragma once

#include <Components/Position.h>

#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>

#include <engine/TrivialShaping.h>
#include <engine/Application.h>

namespace ad {

typedef aunteater::Archetype<Position> Rendered;

class Render : public aunteater::System
{

public:
    Render(aunteater::Engine & aEngine, Application & aApplication);

    void update(const aunteater::Timer aTimer) override;

private:
    aunteater::Engine & mEngine;
    const aunteater::FamilyHelp<Rendered> mRenderables;
    Application & mApplication;
    TrivialShaping mTrivialShaping;

};

} // namespace ad
