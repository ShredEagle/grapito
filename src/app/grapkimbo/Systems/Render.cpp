#include "Render.h"

#include "Utils/DrawDebugStuff.h"
#include <math/Transformations.h>


namespace ad {

Render::Render(aunteater::EntityManager & aEntityManager, Application & aApplication) :
    mEntityManager{aEntityManager},
    mRectangles{mEntityManager},
    mOutlines{mEntityManager},
    mPendulums{mEntityManager},
    mGuides{mEntityManager},
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
    debugDrawer->clear();
    mApplication.getEngine()->clear();

    for(const auto [geometry, visualRectangle] : mRectangles)
    {
        mTrivialShaping.addRectangle({
            {
                static_cast<math::Position<2, GLfloat>>(geometry.position * gPixelsPerMeter),
                static_cast<math::Size<2, GLfloat>>(geometry.dimension * gPixelsPerMeter)  
            },
            visualRectangle.angle,
            visualRectangle.color
        });
    }

    for(const auto [geometry, visualOutline] : mOutlines)
    {
        mTrivialLineStrip.outlineRectangle(
            {
                static_cast<math::Position<2, GLfloat>>((geometry.position - visualOutline.overshoot) 
                                                        * gPixelsPerMeter),
                static_cast<math::Size<2, GLfloat>>((geometry.dimension + 2*visualOutline.overshoot.as<math::Size>()) 
                                                    * gPixelsPerMeter)  
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


    for(const auto & [cameraGuide, geometry] : mGuides)
    {
        // Naively follow the latest guide
        auto cameraTransformation = math::trans2d::translate(- static_cast<math::Vec<2, GLfloat>>(geometry.position) * gPixelsPerMeter
                                                             + math::Vec<2, GLfloat>{800.f, 450.f});
        mTrivialShaping.setCameraTransformation(cameraTransformation);
    }

#ifdef KIMBO_DEBUG
    for(auto collider : mColliders)
    {
        Color vecColor = Color{200,200,20};

        for (auto contact : collider->get<Body>().collidingWith)
        {
            contact.debugRender(vecColor);
        }

    }
#endif

    mTrivialLineStrip.render();
    mTrivialShaping.render();
    debugDrawer->render(gPixelsPerMeter);
}

} // namespace ad
