#include "Render.h"

namespace ad {

Render::Render(aunteater::Engine & aEngine, Application & aApplication) :
    mEngine(aEngine),
    mRenderables(mEngine),
    mApplication(aApplication),
#ifdef KIMBO_DEBUG
    mColliders(mEngine),
#endif
    mTrivialShaping{aApplication.getEngine()->getWindowSize()}
{}

void Render::update(const aunteater::Timer aTimer)
{
    mTrivialShaping.clearShapes();
    mApplication.getEngine()->clear();

    for(const auto [geometry] : mRenderables)
    {
        mTrivialShaping.addRectangle({
            {
                static_cast<math::Position<2, int>>(geometry.position * gPixelsPerMeter),
                static_cast<math::Size<2, int>>(geometry.dimension * gPixelsPerMeter)  
            },
            Color{255, 255, 255}
        });
    }
#ifdef KIMBO_DEBUG
    for(auto collider : mColliders)
    {
        Color color = Color{255, 80, 80};

        if (collider->get<EnvironmentCollisionBox>().collidingWith.size())
        {
            color = Color{80,255,80};
        }

        mTrivialShaping.addRectangle(
            {
                {
                    static_cast<math::Position<2, int>>(collider->get<Position>().position 
                                                       + collider->get<EnvironmentCollisionBox>().box.mPosition.as<math::Vec>()),
                    static_cast<math::Size<2, int>>(collider->get<EnvironmentCollisionBox>().box.mDimension)
                },
                color
            }
        );
    }
#endif

    mTrivialShaping.render();
}

} // namespace ad
