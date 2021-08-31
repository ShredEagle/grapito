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
                mTrivialLineStrip.addLine(
                    {
                    {line.origin * static_cast<GLfloat>(pixelsPerMeter), line.color},
                    {line.end * static_cast<GLfloat>(pixelsPerMeter), line.color},
                    }
                );
            }

            for (auto point : mPoints)
            {
                mTrivialLineStrip.addLine(
                    {
                    {point.origin * pixelsPerMeter + math::Vec<2, GLfloat>{1.f, 1.f} * point.radius, point.color},
                    {point.origin * pixelsPerMeter + math::Vec<2, GLfloat>{-1.f, -1.f} * point.radius, point.color},
                    }
                );
                mTrivialLineStrip.addLine(
                    {
                    {point.origin * pixelsPerMeter + math::Vec<2, GLfloat>{1.f, -1.f} * point.radius, point.color},
                    {point.origin * pixelsPerMeter + math::Vec<2, GLfloat>{-1.f, 1.f} * point.radius, point.color},
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
