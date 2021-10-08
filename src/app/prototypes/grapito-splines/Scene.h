#pragma once


#include <engine/CameraUtilities.h>
#include <engine/Curving.h>
#include <engine/TrivialLineStrip.h>

#include <math/Transformations.h>
#include <math/Curves/Bezier.h>


namespace ad {


constexpr int gWindowHeight = 500;
constexpr GLfloat gCardinalTension = 0.f;
constexpr int gSubdivisions = 100;
constexpr int gBezierSubdivisions = 100;
constexpr GLfloat gBezierWidth = 5.0f;


using Point = TrivialLineStrip::LinePoint;

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


auto getProjection(Size2<int> aRenderResolution)
{
    return math::trans2d::orthographicProjection(getViewRectangle(aRenderResolution, gWindowHeight));
}


struct Scene
{
    Scene(Size2<int> aRenderResolution) :
        mTrivialLineStrip{aRenderResolution},
        mCurving{gBezierSubdivisions, getProjection(aRenderResolution)}
    {
        mTrivialLineStrip.setProjectionTransformation(getProjection(aRenderResolution));
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
    std::vector<Curving::Instance> mBeziers;
    TrivialLineStrip mTrivialLineStrip;
    Curving mCurving;
};

} // namespace ad
