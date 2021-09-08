#pragma once

#include "commons.h"
#include "engine/Application.h"
#include "engine/TrivialLineStrip.h"
#include "engine/commons.h"
#include "math/Angle.h"
#include "math/Matrix.h"
#include <engine/TrivialShaping.h>

#include <vector>

namespace ad
{
    namespace debug
    {
        struct Rectangle
        {
            Rectangle(
                const grapito::Position2 & aOrigin,
                const math::Size<2, double> & aDimension,
                const math::Matrix<3, 3> aTransform,
                const Color & aColor
            ) :
                origin{static_cast<math::Position<2, int>>(aOrigin)},
                dimension{static_cast<math::Size<2, int>>(aDimension)},
                transform{aTransform},
                color{aColor}
            {}

            math::Position<2, GLfloat> origin;
            math::Size<2, GLfloat> dimension;
            math::Matrix<3, 3, float> transform;
            Color color;
        };

        struct Line
        {
            Line(
                const grapito::Position2 & aOrigin,
                const grapito::Position2 & aEnd,
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
                const grapito::Position2 & aOrigin,
                const grapito::Position2 & aEnd,
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
                const grapito::Position2 & aOrigin,
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
                DrawDebugStuff(const Application & aApplication) :
                    mTrivialShaping{aApplication.getEngine()->getWindowSize()},
                    mTrivialLineStrip{aApplication.getEngine()->getWindowSize()}
                {}
                void drawRectangle(Rectangle aRectangle);
                void drawLine(Line aLine);
                void drawPoint(Point aPoint);
                //void drawArrow(Arrow aArrow);
                //void drawPoint(Point aPoint);
                void render();
                void clear();
                TrivialShaping mTrivialShaping;
                TrivialLineStrip mTrivialLineStrip;
            private:
                std::vector<Rectangle> mRectangles;
                std::vector<Line> mLines;
                std::vector<Arrow> mArrows;
                std::vector<Point> mPoints;
        };
    }
    extern std::unique_ptr<debug::DrawDebugStuff> debugDrawer;
} // namespace
