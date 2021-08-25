#include "Render.h"

namespace ad {

Render::Render(aunteater::EntityManager & aEntityManager, Application & aApplication) :
    mEntityManager{aEntityManager},
    mRectangles{mEntityManager},
    mOutlines{mEntityManager},
    mPendulums{mEntityManager},
    mApplication(aApplication),
#ifdef KIMBO_DEBUG
    mColliders{mEntityManager},
#endif
    mTrivialShaping{aApplication.getEngine()->getWindowSize()},
    mTrivialLineStrip{aApplication.getEngine()->getWindowSize()}
{}

void Render::update(const aunteater::Timer aTimer, const GameInputState &)
{
    mTrivialShaping.clearShapes();
    mTrivialLineStrip.clearLines();
    mApplication.getEngine()->clear();

    for(const auto [geometry, visualRectangle] : mRectangles)
    {
        mTrivialShaping.addRectangle({
            {
                static_cast<math::Position<2, int>>(geometry.position * gPixelsPerMeter),
                static_cast<math::Size<2, int>>(geometry.dimension * gPixelsPerMeter)  
            },
            visualRectangle.color
        });
    }

    for(const auto [geometry, visualOutline] : mOutlines)
    {
        mTrivialLineStrip.outlineRectangle(
            {
                static_cast<math::Position<2, GLfloat>>((geometry.position - visualOutline.overshoot) * gPixelsPerMeter),
                static_cast<math::Size<2, GLfloat>>((geometry.dimension + 2*visualOutline.overshoot.as<math::Size>()) * gPixelsPerMeter)  
            },
            visualOutline.color
        );
    }

    for(const auto [pendular, geometry] : mPendulums)
    {
        mTrivialLineStrip.addLine({
            {static_cast<math::Position<2, GLfloat>>(pendular.anchor * gPixelsPerMeter), math::sdr::gRed},
            {static_cast<math::Position<2, GLfloat>>(geometry.center() * gPixelsPerMeter), math::sdr::gGreen},
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

    mTrivialLineStrip.render();
    mTrivialShaping.render();
}

} // namespace ad
