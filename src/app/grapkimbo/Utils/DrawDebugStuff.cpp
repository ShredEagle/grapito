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
                static_cast<grapito::Position2>(static_cast<grapito::Vec2>(aRectangle.origin) + grapito::Vec2{aRectangle.dimension.width(), 0.f}),
                0.,
                aRectangle.color,
            });
            mLines.push_back({
                static_cast<grapito::Position2>(aRectangle.origin),
                static_cast<grapito::Position2>(static_cast<grapito::Vec2>(aRectangle.origin) + grapito::Vec2{0.f, aRectangle.dimension.height()}),
                0.,
                aRectangle.color,
            });
            mLines.push_back({
                static_cast<grapito::Position2>(static_cast<grapito::Vec2>(aRectangle.origin) + grapito::Vec2{0.f, aRectangle.dimension.height()}),
                static_cast<grapito::Position2>(static_cast<grapito::Vec2>(aRectangle.origin) + grapito::Vec2{aRectangle.dimension.width(), aRectangle.dimension.height()}),
                0.,
                aRectangle.color,
            });
            mLines.push_back({
                static_cast<grapito::Position2>(static_cast<grapito::Vec2>(aRectangle.origin) + grapito::Vec2{aRectangle.dimension.width(), 0.f}),
                static_cast<grapito::Position2>(static_cast<grapito::Vec2>(aRectangle.origin) + grapito::Vec2{aRectangle.dimension.width(), aRectangle.dimension.height()}),
                0.,
                aRectangle.color,
            });
        }

        void DrawDebugStuff::drawCross(const Cross & aCross)
        {
            grapito::Vec2 slashOffset{aCross.size / 2.f, aCross.size / 2.f};
            grapito::Vec2 antislashOffset{aCross.size / 2.f, -aCross.size / 2.f};
            mLines.push_back({
                aCross.center - slashOffset,
                aCross.center + slashOffset,
                0.,
                aCross.color,
            });
            mLines.push_back({
                aCross.center - antislashOffset,
                aCross.center + antislashOffset,
                0.,
                aCross.color,
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
            if (! mShown)
            {
                return;
            }

            std::vector<graphics::TrivialShaping::Rectangle> rectangles;
            for (auto rectangle : mRectangles)
            {
                // TODO this should probably be the data member type, instead of having yet another triangle type.
                rectangles.push_back(
                    {
                        {
                            rectangle.origin,
                            rectangle.dimension,
                        },
                        rectangle.color,
                        rectangle.transform,
                    }
                );
            }
            mTrivialShaping.updateInstances(rectangles);

            for (auto line : mLines)
            {
                mTrivialLineStrip.addLine({
                    {line.origin, line.color},
                    {line.end, line.color},
                });
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
            mTrivialLineStrip.clearLines();
        }
    }
}
