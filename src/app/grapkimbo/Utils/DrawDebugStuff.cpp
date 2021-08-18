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

        void DrawDebugStuff::render()
        {
            for (auto rectangle : mRectangles)
            {
                mTrivialShaping.addRectangle(
                    {
                        {
                            rectangle.origin,
                            rectangle.dimension
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
                        line.origin,
                        line.end,
                        line.width,
                        line.color,
                    }
                );
            }

            mTrivialShaping.render();
            mDrawLine.render();
        }

        void DrawDebugStuff::clear()
        {
            mRectangles.clear();
            mLines.clear();
            mPoints.clear();
            mArrows.clear();
            mTrivialShaping.clearShapes();
            mDrawLine.clearShapes();
        }
    }
}
