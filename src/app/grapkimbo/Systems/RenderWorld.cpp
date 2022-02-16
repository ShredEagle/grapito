#include "RenderWorld.h"

#include "../Configuration.h"
#include "../Timer.h"

#include "../Utils/Camera.h"
#include "../Utils/DrawDebugStuff.h"
#include "../Utils/RopeUtilities.h"

#include <graphics/TrivialPolygon.h>
#include <graphics/CameraUtilities.h>

#include <math/Transformations.h>


namespace ad {
namespace grapito
{

RenderWorld::RenderWorld(aunteater::EntityManager & aEntityManager,
                         std::shared_ptr<graphics::AppInterface> aAppInterface) :
    mEntityManager{aEntityManager},
    mAppInterface{aAppInterface},
    mRectangles{mEntityManager},
    mPolygons{mEntityManager},
    mBodyRectangles{mEntityManager},
    mBodyPolygons{mEntityManager},
    mOutlines{mEntityManager},
    mRopes{mEntityManager},
    mCrosshairs{mEntityManager},
    mSprites{mEntityManager},
    mCameras{mEntityManager},
    mTrivialShaping{aAppInterface->getWindowSize()},
    mTrivialLineStrip{aAppInterface->getWindowSize()},
    mTrivialPolygon{aAppInterface->getWindowSize()},
    mCurving{render::gBezierSubdivisions},
    mSpriting{render::gSpritePixelWorldSize}
{
    mCurving.setColor(rope::gColor);
}


AtlasIndex RenderWorld::installAtlas(graphics::sprite::LoadedAtlas aAtlas)
{
    mAtlases.push_back(std::move(aAtlas)); 
    mAtlasToSprites.emplace_back();
    return mAtlases.size() - 1;
}


void RenderWorld::update(const GrapitoTimer /*aTimer*/, const GameInputState &)
{
    mTrivialLineStrip.clearLines();
    mTrivialPolygon.clearPolygons();

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
                data.color,
        });
        mTrivialLineStrip.addLine({
                {
                    static_cast<math::Position<2, GLfloat>>(geometry.position + body.massCenter.as<math::Vec>()),
                    data.color,
                },
                {
                    static_cast<math::Position<2, GLfloat>>(geometry.position + body.massCenter.as<math::Vec>() + data.mAimVector * 5.f),
                    data.color,
                }
            });
    }

    { // Sprites
        std::ranges::for_each(mAtlasToSprites, [](auto & aSprites){aSprites.clear();});
        for(const auto & [body, geometry, visualSprite] : mSprites)
        {
            // Handle sprite alignment on the bouding box edges.
            Position2 position = [&, visualSprite = visualSprite, geometry = geometry]()
            {
                if (visualSprite.alignment == VisualSprite::AlignLeft)
                {
                    return geometry.position;
                }
                else
                {
                    return geometry.position 
                        + Vec2{geometry.dimension.width() - visualSprite.getWorldSize().width(), 0.f};
                }
            }();

            if (body.theta == Radian{0.f})
            {
                mAtlasToSprites
                    .at(visualSprite.atlas)
                        .emplace_back(position,
                                      visualSprite.sprite,
                                      render::gSpriteOpacity,
                                      visualSprite.mirroring);
            }
            else
            {
                auto modelTransform = 
                    math::trans2d::rotateAbout(body.theta, body.massCenter)
                    * math::trans2d::translate(position.as<math::Vec>()) 
                    ;
                mAtlasToSprites
                    .at(visualSprite.atlas)
                        .emplace_back(modelTransform,
                                      visualSprite.sprite,
                                      render::gSpriteOpacity,
                                      visualSprite.mirroring);
            }
        }
    }

    // Position camera
    {
        auto position = getCameraPosition(mCameras);
        math::Rectangle<GLfloat> viewed = 
            getViewedRectangle(position,
                               math::getRatio<GLfloat>(mAppInterface->getWindowSize()));
        setViewedRectangle(mTrivialShaping, viewed);
        setViewedRectangle(mTrivialLineStrip, viewed);
        setViewedRectangle(mTrivialPolygon, viewed);
        setOrthographicView(mCurving,
                            {position, 0.f},
                            graphics::getViewVolume(mAppInterface->getWindowSize(),
                                                    render::gViewedHeight,
                                                    1.f, 2.f));
        setViewedRectangle(mSpriting, viewed);
        debugDrawer->setViewedRectangle(viewed);
    }

    mTrivialShaping.updateInstances(shapes);
}


void RenderWorld::render() const
{
    mTrivialLineStrip.render();
    mTrivialShaping.render();
    mTrivialPolygon.render();
    mCurving.render(mBeziers);

    // Render all sprites for each atlas (batch rendering).
    for (AtlasIndex atlas = 0; atlas != mAtlases.size(); ++atlas)
    {
        mSpriting.updateInstances(mAtlasToSprites.at(atlas));
        mSpriting.render(mAtlases.at(atlas));
    }
}


} // namespace grapito
} // namespace ad
