#include "Render.h"

namespace ad {

Render::Render(aunteater::Engine & aEngine, Application & aApplication) :
    mEngine(aEngine),
    mRenderables(mEngine),
    mApplication(aApplication),
    mTrivialShaping{aApplication.getEngine()->getWindowSize()}
{}


void Render::update(const aunteater::Timer aTimer)
{
    mTrivialShaping.clearShapes();
    mApplication.getEngine()->clear();

    for(auto renderable : mRenderables)
    {
        mTrivialShaping.addRectangle({{renderable->get<Position>().position, {100, 100}}, Color{255,255,255}});
    }

    mTrivialShaping.render();
}

} // namespace ad
