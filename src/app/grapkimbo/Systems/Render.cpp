#include "Render.h"

#include "../Configuration.h"
#include "../Utils/DrawDebugStuff.h"
#include "../Utils/RopeUtilities.h"

#include <engine/CameraUtilities.h>

#include <math/Transformations.h>
#include <math/VectorUtilities.h>


namespace ad {
namespace grapito
{

Render::Render(aunteater::EntityManager & aEntityManager, Application & aApplication) :
    mEntityManager{aEntityManager},
    mRectangles{mEntityManager},
    mBodyRectangles{mEntityManager},
    mOutlines{mEntityManager},
    mPendulums{mEntityManager},
    mRopes{mEntityManager},
    mCameras{mEntityManager},
    mEngine(aApplication.getEngine()),
#ifdef KIMBO_DEBUG
    mColliders{mEntityManager},
#endif
    mTrivialShaping{aApplication.getEngine()->getWindowSize()},
    mTrivialLineStrip{aApplication.getEngine()->getWindowSize()},
    mCurving{render::gBezierSubdivisions}
{}

void Render::update(const aunteater::Timer aTimer, const GameInputState &)
{
    mTrivialShaping.clearShapes();
    mTrivialLineStrip.clearLines();
    debugDrawer->clear();
    mEngine->clear();

    for (auto & [geometry, body, visualRectangle] : mBodyRectangles)
    {
        visualRectangle.transform = static_cast<math::Matrix<3, 3, float>>(
                createPrefixedTransform(
                    body.theta,
                    static_cast<Position2>(geometry.position.as<math::Vec>() + body.massCenter.as<math::Vec>())
                    )
                );
    }

    for(const auto [geometry, visualRectangle] : mRectangles)
    {
        mTrivialShaping.addRectangle({
            {
                static_cast<math::Position<2, GLfloat>>(geometry.position),
                static_cast<math::Size<2, GLfloat>>(geometry.dimension)  
            },
            visualRectangle.transform,
            visualRectangle.color,
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

    for(const auto [pendular, geometry, aas] : mPendulums)
    {
        mTrivialLineStrip.addLine({
            {static_cast<math::Position<2, GLfloat>>(pendular.anchor), math::sdr::gRed},
            {static_cast<math::Position<2, GLfloat>>(geometry.center()), math::sdr::gGreen},
        });
    }

    Spline beziers;
    for (const auto [ropeCreator, _position, _body] : mRopes)
    {
        appendRopeSpline(ropeCreator, std::back_inserter(beziers));
    }

    for(const auto & [cameraTag, geometry] : mCameras)
    {
        auto viewed = math::Rectangle<GLfloat>{
            static_cast<math::Position<2, GLfloat>>(geometry.position),
            math::makeSizeFromHeight(render::gViewedHeight, math::getRatio<GLfloat>(mEngine->getWindowSize()))
        }.centered();
        setViewedRectangle(mTrivialShaping, viewed);
        setViewedRectangle(mTrivialLineStrip, viewed);
        setViewedRectangle(mCurving, viewed);
        setViewedRectangle(debugDrawer->mTrivialShaping, viewed);
        setViewedRectangle(debugDrawer->mTrivialLineStrip, viewed);
    }

    mTrivialLineStrip.render();
    mTrivialShaping.render();
    mCurving.render(beziers);
    debugDrawer->render();
}

} // namespace grapito
} // namespace ad
