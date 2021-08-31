#pragma once

#include "engine/Application.h"
#include "engine/TrivialLineStrip.h"
#include "engine/commons.h"
#include "math/Angle.h"
#include <engine/TrivialShaping.h>

#include <vector>

namespace ad
{
    namespace debug
    {
        struct Rectangle
        {
            Rectangle(
                const math::Position<2, double> & aOrigin,
                const math::Size<2, double> & aDimension,
                const float aAngle,
                const Color & aColor
            ) :
                origin{static_cast<math::Position<2, int>>(aOrigin)},
                dimension{static_cast<math::Size<2, int>>(aDimension)},
                angle{aAngle},
                color{aColor}
            {}

            math::Position<2, GLfloat> origin;
            math::Size<2, GLfloat> dimension;
            math::Angle<float> angle;
            Color color;
        };

        struct Line
        {
            Line(
                const math::Position<2, double> & aOrigin,
                const math::Position<2, double> & aEnd,
                float aWidth,
                const Color & aColor
            ) :
                origin{static_cast<math::Position<2, float>>(aOrigin)},
                end{static_cast<math::Position<2, float>>(aEnd)},
                width{aWidth},
                color{aColor}
            {}
            math::Position<2, GLfloat> origin;
            math::Position<2, GLfloat> end;
            float width;
            Color color;
        };

        struct Arrow
        {
            Arrow(
                const math::Position<2, double> & aOrigin,
                const math::Position<2, double> & aEnd,
                const math::Size<2, double> & aHeadDimension,
                float aWidth,
                const Color & aColor
            ) :
                origin{static_cast<math::Position<2, int>>(aOrigin)},
                end{static_cast<math::Position<2, int>>(aEnd)},
                headDimension{static_cast<math::Size<2, int>>(aHeadDimension)},
                width{aWidth},
                color{aColor}
            {}
            math::Position<2, int> origin;
            math::Position<2, int> end;
            math::Size<2, int> headDimension;
            float width;
            Color color;
        };

        struct Point
        {
            Point(
                const math::Position<2, double> & aOrigin,
                float aRadius,
                const Color & aColor
            ) :
                origin{aOrigin},
                radius{aRadius},
                color{aColor}
            {}
            math::Position<2, GLfloat> origin;
            float radius;
            Color color;
        };

        class DrawDebugStuff
        {
            public:
                DrawDebugStuff(const Application & aApplication, const float aPixelsPerMeter) :
                    mTrivialShaping{aApplication.getEngine()->getWindowSize()},
                    mTrivialLineStrip{aApplication.getEngine()->getWindowSize()},
                    pixelsPerMeter{aPixelsPerMeter}
                {}
                void drawRectangle(Rectangle aRectangle);
                void drawLine(Line aLine);
                void drawPoint(Point aPoint);
                //void drawArrow(Arrow aArrow);
                //void drawPoint(Point aPoint);
                void render();
                void clear();
            private:
                std::vector<Rectangle> mRectangles;
                std::vector<Line> mLines;
                std::vector<Arrow> mArrows;
                std::vector<Point> mPoints;

                TrivialShaping mTrivialShaping;
                TrivialLineStrip mTrivialLineStrip;
                float pixelsPerMeter;
        };
    }
    extern debug::DrawDebugStuff * debugDrawer;
} // namespace
