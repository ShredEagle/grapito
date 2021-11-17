#include "Render.h"

#include "../Configuration.h"
#include "../Utils/DrawDebugStuff.h"
#include "../Utils/RopeUtilities.h"
#include <graphics/TrivialPolygon.h>

#include <graphics/CameraUtilities.h>

#include <math/Transformations.h>
#include <math/VectorUtilities.h>


namespace ad {
namespace grapito
{

Render::Render(aunteater::EntityManager & aEntityManager,
               std::shared_ptr<graphics::AppInterface> aAppInterface) :
    mEntityManager{aEntityManager},
    mRectangles{mEntityManager},
    mPolygons{mEntityManager},
    mBodyRectangles{mEntityManager},
    mBodyPolygons{mEntityManager},
    mOutlines{mEntityManager},
    mRopes{mEntityManager},
    mCrosshairs{mEntityManager},
    mCameras{mEntityManager},
    mAppInterface{aAppInterface},
    mTrivialShaping{aAppInterface->getWindowSize()},
    mTrivialLineStrip{aAppInterface->getWindowSize()},
    mTrivialPolygon{aAppInterface->getWindowSize()},
    mCurving{render::gBezierSubdivisions}
{}

void Render::update(const GrapitoTimer, const GameInputState &)
{
    mTrivialLineStrip.clearLines();
    mTrivialPolygon.clearPolygons();
    mAppInterface->clear();

    for (auto & [geometry, body, visualRectangle] : mBodyRectangles)
    {
        visualRectangle.transform = static_cast<math::Matrix<3, 3, float>>(
            math::trans2d::rotateAbout(body.theta, geometry.position + body.massCenter.as<math::Vec>())
        );
    }

    for (auto & [geometry, body, visualPolygon] : mBodyPolygons)
    {
        visualPolygon.transform = static_cast<math::Matrix<3, 3, float>>(
            math::trans2d::rotateAbout(body.theta, geometry.position + body.massCenter.as<math::Vec>())
        );
    }

    // shapes vector to receive all rectangle to be drawn by TrivialShaping renderer.
    std::vector<graphics::TrivialShaping::Rectangle> shapes;
    for(const auto [geometry, visualRectangle] : mRectangles)
    {
        // TODO should control drawing of Scope::RopeStructure rectangles
        // based on Imgui widgets.
        shapes.push_back({
            {
                static_cast<math::Position<2, GLfloat>>(geometry.position),
                static_cast<math::Size<2, GLfloat>>(geometry.dimension)  
            },
            visualRectangle.color,
            visualRectangle.transform,
        });
    }

    for(const auto [geometry, visualPolygon] : mPolygons)
    {
        std::vector<graphics::TrivialPolygon::PolygonPoint> vertices;

        for (auto vertex : visualPolygon.mVertices)
        {
            vertices.emplace_back(graphics::TrivialPolygon::PolygonPoint{
                        vertex + geometry.position.as<math::Vec>(),
                        visualPolygon.color,
                        visualPolygon.transform
                    });
        }

        mTrivialPolygon.addVertices(vertices);
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

    mBeziers.clear();
    for (const auto [ropeCreator, _position, _body] : mRopes)
    {
        appendRopeSpline(ropeCreator, std::back_inserter(mBeziers));
    }

    for (const auto & [geometry, body, data] : mCrosshairs)
    {
        shapes.push_back({
                {
                    static_cast<math::Position<2, GLfloat>>(geometry.position + body.massCenter.as<math::Vec>() + data.mAimVector * 5.f - Vec2{0.25f, 0.25f}),
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
        setViewedRectangle(mTrivialPolygon, viewed);
        setOrthographicView(mCurving,
                            {geometry.position, 0.f},
                            graphics::getViewVolume(mAppInterface->getWindowSize(), render::gViewedHeight, 1.f, 2.f));
        debugDrawer->setViewedRectangle(viewed);
    }

    mTrivialShaping.updateInstances(shapes);
    render();
}


void Render::render() const
{
    mTrivialLineStrip.render();
    mTrivialShaping.render();
    mTrivialPolygon.render();
    mCurving.render(mBeziers);
}

} // namespace grapito
} // namespace ad
