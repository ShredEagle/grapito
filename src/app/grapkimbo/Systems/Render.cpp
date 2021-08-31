#include "Render.h"

#include "Utils/DrawDebugStuff.h"
#include "Configuration.h"

#include <engine/CameraUtilities.h>

#include <math/Transformations.h>
#include <math/VectorUtilities.h>


namespace ad {

Render::Render(aunteater::EntityManager & aEntityManager, Application & aApplication) :
    mEntityManager{aEntityManager},
    mRectangles{mEntityManager},
    mOutlines{mEntityManager},
    mPendulums{mEntityManager},
    mCameras{mEntityManager},
    mEngine(aApplication.getEngine()),
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
    mEngine->clear();

    for(const auto [geometry, visualRectangle] : mRectangles)
    {
        mTrivialShaping.addRectangle({
            {
                static_cast<math::Position<2, GLfloat>>(geometry.position),
                static_cast<math::Size<2, GLfloat>>(geometry.dimension)  
            },
            visualRectangle.color,
            visualRectangle.angle
        });
    }

    for(const auto [geometry, visualOutline] : mOutlines)
    {
        mTrivialLineStrip.outlineRectangle(
            {
                static_cast<math::Position<2, GLfloat>>(geometry.position - visualOutline.overshoot),
                static_cast<math::Size<2, GLfloat>>(geometry.dimension + 2*visualOutline.overshoot.as<math::Size>())  
            },
            visualOutline.color
        );
    }

    for(const auto [pendular, geometry] : mPendulums)
    {
        mTrivialLineStrip.addLine({
            {static_cast<math::Position<2, GLfloat>>(pendular.anchor), math::sdr::gRed},
            {static_cast<math::Position<2, GLfloat>>(geometry.center()), math::sdr::gGreen},
        });
    }


    for(const auto & [cameraTag, geometry] : mCameras)
    {
        auto viewed = math::Rectangle<GLfloat>{
            static_cast<math::Position<2, GLfloat>>(geometry.position),
            math::makeSizeFromHeight(render::gViewedHeight, math::getRatio<GLfloat>(mEngine->getWindowSize()))
        }.centered();
        setViewedRectangle(mTrivialShaping, viewed);
        setViewedRectangle(mTrivialLineStrip, viewed);
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
    debugDrawer->render();
}

} // namespace ad
