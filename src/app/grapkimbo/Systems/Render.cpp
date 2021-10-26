#include "Render.h"

#include "../Configuration.h"
#include "../Utils/DrawDebugStuff.h"
#include "../Utils/RopeUtilities.h"

#include <graphics/CameraUtilities.h>

#include <math/Transformations.h>
#include <math/VectorUtilities.h>


namespace ad {
namespace grapito
{

Render::Render(aunteater::EntityManager & aEntityManager, graphics::ApplicationGlfw & aApplication) :
    mEntityManager{aEntityManager},
    mRectangles{mEntityManager},
    mBodyRectangles{mEntityManager},
    mOutlines{mEntityManager},
    mRopes{mEntityManager},
    mCrosshairs{mEntityManager},
    mCameras{mEntityManager},
    mAppInterface(aApplication.getAppInterface()),
    mTrivialShaping{aApplication.getAppInterface()->getWindowSize()},
    mTrivialLineStrip{aApplication.getAppInterface()->getWindowSize()},
    mCurving{render::gBezierSubdivisions}
{}

void Render::update(const GrapitoTimer aTimer, const GameInputState &)
{
    mTrivialShaping.clearShapes();
    mTrivialLineStrip.clearLines();
    debugDrawer->clear();
    mAppInterface->clear();

    for (auto & [geometry, body, visualRectangle] : mBodyRectangles)
    {
        visualRectangle.transform = static_cast<math::Matrix<3, 3, float>>(
            math::trans2d::rotateAbout(body.theta, geometry.position + body.massCenter.as<math::Vec>())
        );
    }

    for(const auto [geometry, visualRectangle] : mRectangles)
    {
        // TODO should control drawing of Scope::RopeStructure rectangles
        // based on Imgui widgets.
        mTrivialShaping.addRectangle({
            {
                static_cast<math::Position<2, GLfloat>>(geometry.position),
                static_cast<math::Size<2, GLfloat>>(geometry.dimension)  
            },
            visualRectangle.color,
            visualRectangle.transform,
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

    Spline beziers;
    for (const auto [ropeCreator, _position, _body] : mRopes)
    {
        appendRopeSpline(ropeCreator, std::back_inserter(beziers));
    }

    for (const auto & [geometry, body, data] : mCrosshairs)
    {
        mTrivialShaping.addRectangle({
                {
                    static_cast<math::Position<2, GLfloat>>(geometry.position + body.massCenter.as<math::Vec>() + data.mAimVector * 5.f),
                    {0.5f, 0.5f}
                },
                math::sdr::gGreen,
        });
        mTrivialLineStrip.addLine({
                {
                    static_cast<math::Position<2, GLfloat>>(geometry.position + body.massCenter.as<math::Vec>()),
                    math::sdr::gGreen
                },
                {
                    static_cast<math::Position<2, GLfloat>>(geometry.position + body.massCenter.as<math::Vec>() + data.mAimVector * 5.f),
                    math::sdr::gGreen
                }
            });
    }

    for(const auto & [cameraTag, geometry] : mCameras)
    {
        auto viewed = math::Rectangle<GLfloat>{
            static_cast<math::Position<2, GLfloat>>(geometry.position),
            math::makeSizeFromHeight(render::gViewedHeight, math::getRatio<GLfloat>(mAppInterface->getWindowSize()))
        }.centered();
        setViewedRectangle(mTrivialShaping, viewed);
        setViewedRectangle(mTrivialLineStrip, viewed);
        setOrthographicView(mCurving,
                            // TODO FPASS
                            {static_cast<math::Position<2, GLfloat>>(geometry.position), 0.f},
                            graphics::getViewVolume(mAppInterface->getWindowSize(), render::gViewedHeight, 1.f, 2.f));
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
