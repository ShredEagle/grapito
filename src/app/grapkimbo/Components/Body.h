#pragma once

#include "../Utilities.h"
#include "Utils/Contact.h"
#include "Utils/CollisionBox.h"

#include <algorithm>
#include <aunteater/Component.h>

namespace ad {
namespace grapito
{

enum ShapeType
{
    HULL,
    type_count,
};

enum BodyType
{
    STATIC,
    DYNAMIC,
};

struct Body : public aunteater::Component<Body>
{
    explicit Body(
        math::Rectangle<double> aBox,
        BodyType aBodyType,
        ShapeType aShapeType,
        double aMass = 1.,
        double aTheta = 0.,
        double aFriction = 0.
    ) :
        box{std::move(aBox)},
        bodyType{aBodyType},
        shapeType{aShapeType},
        friction{aFriction}
    {
        radius = std::max(aBox.height(), aBox.width());

        double area = 0.;
        Vec2 vecMassCenter = Vec2::Zero();
        for (int i = 0; i < box.mFaceCount; ++i)
        {
            auto edge = box.getEdge(i);
            auto vertexA = edge.origin.as<math::Vec>();
            auto vertexB = edge.end.as<math::Vec>();
            double areaStep = twoDVectorCross(vertexA, vertexB) / 2;
            Vec2 centerStep = (vertexA + vertexB) / 3;
            double moiStep = areaStep * (vertexA.dot(vertexA) + vertexB.dot(vertexB) + vertexA.dot(vertexB)) / 6;

            if (area + areaStep != 0)
            {
                vecMassCenter = (vecMassCenter * area + centerStep * areaStep)/(area + areaStep);
            }
            area += areaStep;
            moi += moiStep;
        }

        massCenter = static_cast<Position2>(vecMassCenter);
        theta = math::Radian<double>{aTheta};

        if (bodyType != BodyType::DYNAMIC)
        {
            mass = 0.;
            invMass = 0.;
            moi = 0.;
            invMoi = 0.;
        }
        else
        {
            mass = aMass;
            double density = aMass / area;
            moi *= density;
            moi -= aMass * vecMassCenter.dot(vecMassCenter);
            invMass = 1 / aMass;
            invMoi = 1 / moi;
        }

    }

    void debugRender(Position2 pos)
    {
        for (int i = 0; i < box.mFaceCount; ++i)
        {
            auto vertex = box.getVertice(i);
            debugDrawer->drawPoint({
                    transformPosition(
                            (Position2)pos.as<math::Vec>() + vertex.as<math::Vec>(),
                            theta,
                            (Position2)pos.as<math::Vec>() + massCenter.as<math::Vec>()
                    ),
                    .05f,
                    {255,255,0},
            });
        }
        debugDrawer->drawPoint({
                (Position2)pos.as<math::Vec>() + massCenter.as<math::Vec>(),
                .05f,
                {255,100,0},
        });
    }

    double mass;
    double invMass;
    double moi;
    double invMoi;
    double friction;
    Position2 massCenter = {0., 0.};

    double radius;
    math::Radian<double> theta;

    CollisionBox box;

    BodyType bodyType;
    ShapeType shapeType;

    std::vector<ContactQuery> collidingWith;
};


} // namespace grapito
} // namespace ad
