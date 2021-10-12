#pragma once

#include "commons.h"

#include <graphics/ApplicationGlfw.h>
#include <graphics/commons.h>
#include <graphics/TrivialLineStrip.h>
#include <graphics/TrivialShaping.h>

#include <math/Angle.h>
#include <math/Matrix.h>

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
                const math::sdr::Rgb & aColor
            ) :
                origin{static_cast<math::Position<2, float>>(aOrigin)},
                dimension{static_cast<math::Size<2, float>>(aDimension)},
                transform{aTransform},
                color{aColor}
            {}

            math::Position<2, GLfloat> origin;
            math::Size<2, GLfloat> dimension;
            math::Matrix<3, 3, float> transform;
            const math::sdr::Rgb color;
        };

        struct Line
        {
            Line(
                const grapito::Position2 & aOrigin,
                const grapito::Position2 & aEnd,
                float aWidth,
                const math::sdr::Rgb & aColor
            ) :
                origin{static_cast<math::Position<2, float>>(aOrigin)},
                end{static_cast<math::Position<2, float>>(aEnd)},
                width{aWidth},
                color{aColor}
            {}
            math::Position<2, GLfloat> origin;
            math::Position<2, GLfloat> end;
            float width;
            math::sdr::Rgb color;
        };

        struct Arrow
        {
            Arrow(
                const grapito::Position2 & aOrigin,
                const grapito::Position2 & aEnd,
                const math::Size<2, double> & aHeadDimension,
                float aWidth,
                const math::sdr::Rgb & aColor
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
            math::sdr::Rgb color;
        };

        struct Point
        {
            Point(
                const grapito::Position2 & aOrigin,
                float aRadius,
                const math::sdr::Rgb & aColor
            ) :
                origin{aOrigin},
                radius{aRadius},
                color{aColor}
            {}
            math::Position<2, GLfloat> origin;
            float radius;
            math::sdr::Rgb color;
        };

        class DrawDebugStuff
        {
            public:
                DrawDebugStuff(const graphics::ApplicationGlfw & aApplication) :
                    mTrivialShaping{aApplication.getAppInterface()->getWindowSize()},
                    mTrivialLineStrip{aApplication.getAppInterface()->getWindowSize()}
                {}
                void drawRectangle(Rectangle aRectangle);
                void drawOutline(Rectangle aRectangle);
                void drawLine(Line aLine);
                void drawPoint(Point aPoint);
                //void drawArrow(Arrow aArrow);
                //void drawPoint(Point aPoint);
                void render();
                void clear();
                graphics::TrivialShaping mTrivialShaping;
                graphics::TrivialLineStrip mTrivialLineStrip;

                bool mShown{false};

            private:
                std::vector<Rectangle> mRectangles;
                std::vector<Line> mLines;
                std::vector<Arrow> mArrows;
                std::vector<Point> mPoints;
        };
    }

    /// RFP I actually don't even know if I would prefer a singleton over a mutable global.
    /// (it might only matter when we go down the concurrency path)
    /// TODO rename to gDebugDrawer
    inline std::unique_ptr<debug::DrawDebugStuff> debugDrawer;
} // namespace
