#include "DrawDebugStuff.h"

namespace ad
{
    namespace debug
    {
        void DrawDebugStuff::drawRectangle(Rectangle aRectangle)
        {
            mRectangles.push_back(aRectangle);
        }

        void DrawDebugStuff::drawLine(Line aLine)
        {
            mLines.push_back(aLine);
        }

        void DrawDebugStuff::drawPoint(Point aPoint)
        {
            mPoints.push_back(aPoint);
        }

        void DrawDebugStuff::render(const int pixelsPerMeter)
        {
            for (auto rectangle : mRectangles)
            {
                mTrivialShaping.addRectangle(
                    {
                        {
                            rectangle.origin * pixelsPerMeter,
                            rectangle.dimension * pixelsPerMeter,
                        },
                        rectangle.angle,
                        rectangle.color
                    }
                );
            }

            for (auto line : mLines)
            {
                mDrawLine.addLine(
                    {
                        line.origin * pixelsPerMeter,
                        line.end * pixelsPerMeter,
                        line.width,
                        line.color,
                    }
                );
            }

            for (auto point : mPoints)
            {
                mDrawLine.addLine(
                    {
                        static_cast<math::Position<2, int>>(point.origin * pixelsPerMeter + math::Vec<2, double>{1.f, 1.f} * point.radius),
                        static_cast<math::Position<2, int>>(point.origin * pixelsPerMeter + math::Vec<2, double>{-1.f, -1.f} * point.radius),
                        2.f,
                        point.color
                    }
                );
                mDrawLine.addLine(
                    {
                        static_cast<math::Position<2, int>>(point.origin * pixelsPerMeter + math::Vec<2, double>{1.f, -1.f} * point.radius),
                        static_cast<math::Position<2, int>>(point.origin * pixelsPerMeter + math::Vec<2, double>{-1.f, 1.f} * point.radius),
                        2.f,
                        point.color
                    }
                );
            }

            mTrivialShaping.render();
            mDrawLine.render();
            mRectangles.clear();
            mLines.clear();
            mPoints.clear();
            mArrows.clear();
        }

        void DrawDebugStuff::clear()
        {
            mTrivialShaping.clearShapes();
            mDrawLine.clearShapes();
        }
    }
}
