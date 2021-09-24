#include "DrawDebugStuff.h"
#include "commons.h"

namespace ad
{
    namespace debug
    {
        void DrawDebugStuff::drawRectangle(Rectangle aRectangle)
        {
            mRectangles.push_back(aRectangle);
        }

        void DrawDebugStuff::drawOutline(Rectangle aRectangle)
        {
            mLines.push_back({
                static_cast<grapito::Position2>(aRectangle.origin),
                static_cast<grapito::Position2>(static_cast<grapito::Vec2>(aRectangle.origin) + grapito::Vec2{aRectangle.dimension.width(), 0.}),
                0.,
                aRectangle.color,
            });
            mLines.push_back({
                static_cast<grapito::Position2>(aRectangle.origin),
                static_cast<grapito::Position2>(static_cast<grapito::Vec2>(aRectangle.origin) + grapito::Vec2{0., aRectangle.dimension.height()}),
                0.,
                aRectangle.color,
            });
            mLines.push_back({
                static_cast<grapito::Position2>(static_cast<grapito::Vec2>(aRectangle.origin) + grapito::Vec2{0., aRectangle.dimension.height()}),
                static_cast<grapito::Position2>(static_cast<grapito::Vec2>(aRectangle.origin) + grapito::Vec2{aRectangle.dimension.width(), aRectangle.dimension.height()}),
                0.,
                aRectangle.color,
            });
            mLines.push_back({
                static_cast<grapito::Position2>(static_cast<grapito::Vec2>(aRectangle.origin) + grapito::Vec2{aRectangle.dimension.width(), 0.}),
                static_cast<grapito::Position2>(static_cast<grapito::Vec2>(aRectangle.origin) + grapito::Vec2{aRectangle.dimension.width(), aRectangle.dimension.height()}),
                0.,
                aRectangle.color,
            });
        }

        void DrawDebugStuff::drawLine(Line aLine)
        {
            mLines.push_back(aLine);
        }

        void DrawDebugStuff::drawPoint(Point aPoint)
        {
            mPoints.push_back(aPoint);
        }

        void DrawDebugStuff::render()
        {
            for (auto rectangle : mRectangles)
            {
                mTrivialShaping.addRectangle(
                    {
                        {
                            rectangle.origin,
                            rectangle.dimension,
                        },
                        rectangle.transform,
                        rectangle.color,
                    }
                );
            }

            for (auto line : mLines)
            {
                mTrivialLineStrip.addLine(
                    {
                    {line.origin, line.color},
                    {line.end, line.color},
                    }
                );
            }

            for (auto point : mPoints)
            {
                mTrivialLineStrip.addLine(
                    {
                    {point.origin + math::Vec<2, GLfloat>{1.f, 1.f} * point.radius, point.color},
                    {point.origin + math::Vec<2, GLfloat>{-1.f, -1.f} * point.radius, point.color},
                    }
                );
                mTrivialLineStrip.addLine(
                    {
                    {point.origin + math::Vec<2, GLfloat>{1.f, -1.f} * point.radius, point.color},
                    {point.origin + math::Vec<2, GLfloat>{-1.f, 1.f} * point.radius, point.color},
                    }
                );
            }

            mTrivialShaping.render();
            mTrivialLineStrip.render();
            mRectangles.clear();
            mLines.clear();
            mPoints.clear();
            mArrows.clear();
        }

        void DrawDebugStuff::clear()
        {
            mTrivialShaping.clearShapes();
            mTrivialLineStrip.clearLines();
        }
    }
}
