#pragma once


#include <graphics/CameraUtilities.h>
#include <graphics/Curving.h>
#include <graphics/TrivialLineStrip.h>

#include <math/Transformations.h>
#include <math/Curves/Bezier.h>


namespace ad {


constexpr int gWindowHeight = 500;
constexpr int gNear = 500;
constexpr int gDepth = 2*gNear;
constexpr GLfloat gCardinalTension = 0.f;
constexpr int gSubdivisions = 100;
constexpr int gBezierSubdivisions = 100;
constexpr GLfloat gBezierWidth = 5.0f;


using Point = graphics::TrivialLineStrip::LinePoint;

// Just a fantasy to use math::Vec here
using CardinalCubic = math::Vec<4, math::Position<2, GLfloat>>;
using Bezier = math::Bezier<4, 3, GLfloat>;


inline math::Vec<4, GLfloat> getCardinalCubicBlendingCoefficients(GLfloat aParameter, GLfloat aTension)
{
    const GLfloat s = (1.f - aTension) / 2.f;
    const GLfloat t = aParameter;
    const GLfloat t2 = std::pow(t, 2);
    const GLfloat t3 = std::pow(t, 3);

    return {
            -s * t  + (2 * s) * t2      - s * t3,
        1.f         + (s - 3) * t2      + (2 - s) * t3,
            s * t   + (3 - 2 * s) * t2  + (s - 2) * t3,
                    - s * t2            + s * t3
    };
}


inline Bezier toBezier(const CardinalCubic & aCardinal, GLfloat aTension)
{
    const GLfloat s = (1.f - aTension) / 2.f;

    return Bezier{
        Bezier::Position_t{aCardinal[1], 0.f},
        Bezier::Position_t{aCardinal[1] + s/3.f * (aCardinal[2] - aCardinal[0]), 0.f},
        Bezier::Position_t{aCardinal[2] - s/3.f * (aCardinal[3] - aCardinal[1]), 0.f},
        Bezier::Position_t{aCardinal[2], 0.f}
    };
}


auto get2dProjection(math::Size<2, int> aRenderResolution)
{
    return math::trans2d::orthographicProjection(graphics::getViewRectangle(aRenderResolution, gWindowHeight));
}

auto get3dProjection(math::Size<2, int> aRenderResolution, GLfloat aNear = gNear, GLfloat aDepth = gDepth)
{
    return math::trans3d::orthographicProjection(graphics::getViewVolume(aRenderResolution, gWindowHeight, aNear, aDepth));
}



struct Scene
{
    Scene(math::Size<2, int> aRenderResolution) :
        mTrivialLineStrip{aRenderResolution},
        mCurving{gBezierSubdivisions, get3dProjection(aRenderResolution)}
    {
        mTrivialLineStrip.setProjectionTransformation(get2dProjection(aRenderResolution));
    }

    void step()
    {
        prepareSplines();

        mTrivialLineStrip.clearLines();
        mTrivialLineStrip.addLine(mPoints.begin(), mPoints.end());
        mTrivialLineStrip.addLine(mCardinalApproximation.begin(), mCardinalApproximation.end());
    }

    void prepareSplines()
    {
        mCardinalApproximation.clear();
        for (int segment = 0; (segment + 3) < mPoints.size(); ++segment)
        {
            CardinalCubic cardinal{
                mPoints[segment].mPosition,
                mPoints[segment + 1].mPosition,
                mPoints[segment + 2].mPosition,
                mPoints[segment + 3].mPosition
            };
            approximateCardinalCubic(cardinal);
            mBeziers.push_back({toBezier(cardinal, gCardinalTension), gBezierWidth});
        }
    }


    void approximateCardinalCubic(CardinalCubic aControlPoints)
    {
        // Attention: starting with the second curve, the endpoint will be redrawn
        // by the next segment startpoint.
        for (int subdivision = 0; subdivision <= gSubdivisions; ++subdivision)
        {
            GLfloat t = subdivision / (GLfloat)gSubdivisions;
            auto coeffs = getCardinalCubicBlendingCoefficients(t, gCardinalTension);

            auto evaluated = math::Position<2, GLfloat>::Zero();
            for (int controlId = 0; controlId != 4; ++controlId)
            {
                evaluated += aControlPoints[controlId].as<math::Vec>() * coeffs[controlId];
            }
            mCardinalApproximation.push_back({evaluated, math::sdr::gRed});
        }
    }


    void render()
    {
        mCurving.render(mBeziers);
        mTrivialLineStrip.render();
    }

    std::vector<Point> mPoints;
    std::vector<Point> mCardinalApproximation;
    std::vector<graphics::Curving::Instance> mBeziers;
    graphics::TrivialLineStrip mTrivialLineStrip;
    graphics::Curving mCurving;
};

} // namespace ad
