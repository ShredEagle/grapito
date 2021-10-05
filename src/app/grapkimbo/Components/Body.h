#pragma once

#include "../Utilities.h"
#include "Components/AccelAndSpeed.h"
#include "Utils/CollisionBox.h"
#include "Utils/Shape.h"
#include "Utils/PhysicsStructs.h"

#include <algorithm>
#include <aunteater/Component.h>

namespace ad {
namespace grapito
{

struct Body : public aunteater::Component<Body>
{
    explicit Body(
        math::Rectangle<double> aBox,
        BodyType aBodyType,
        ShapeType aShapeType,
        CollisionType aCollisionType,
        float aMass = 1.,
        double aTheta = 0.,
        double aFriction = 0.,
        bool noMaxFriction = false,
        std::vector<CollisionType> aAcceptedCollision = {}
    ) :
        shape{aBox},
        bodyType{aBodyType},
        shapeType{aShapeType},
        collisionType{aCollisionType},
        friction{aFriction},
        noMaxFriction{noMaxFriction},
        moi{0.},
        theta{math::Radian<double>{aTheta}},
        mass{aMass},
        acceptedCollision{aAcceptedCollision}
    {
        updateData();
    }

    void updateData()
    {
        double area = 0.;

        Vec2 vecMassCenter = Vec2::Zero();
        for (int i = 0; i < shape.mFaceCount; ++i)
        {
            Shape::Edge edge = shape.getEdge(i);
            Vec2 vertexA = edge.origin.as<math::Vec>();
            Vec2 vertexB = edge.end.as<math::Vec>();
            double areaStep = twoDVectorCross(vertexA, vertexB) / 2.;
            Vec2 centerStep = (vertexA + vertexB) / 3.;
            double moiStep = areaStep * (vertexA.dot(vertexA) + vertexB.dot(vertexB) + vertexA.dot(vertexB)) / 6.;

            if (area + areaStep != 0.)
            {
                vecMassCenter = (vecMassCenter * area + centerStep * areaStep)/(area + areaStep);
            }

            area += areaStep;
            moi += moiStep;
        }

        massCenter = static_cast<Position2>(vecMassCenter);

        if (bodyType == BodyType_Static)
        {
            mass = 0.;
            invMass = 0.;
            moi = 0.;
            invMoi = 0.;
        }
        else if (bodyType == BodyType_Kinematic)
        {
            //Kinematic object are affected by gravity but not physic collision
            invMass = 0.;
            invMoi = 0.;
        }
        else
        {
            double density = mass / area;
            moi *= density;
            moi -= mass * vecMassCenter.dot(vecMassCenter);
            invMass = 1 / mass;
            invMoi = 1 / moi;
        }
    }

    void updateConstructedBody()
    {
        (*constructedBodyIt).forceUpdateData(this);
    }

    void debugRender(Position2 pos)
    {
        for (int i = 0; i < shape.mFaceCount; ++i)
        {
            auto vertex = shape.getVertice(i);
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
    Position2 massCenter = {0., 0.};

    double friction;
    double restitution;
    bool noMaxFriction;

    math::Radian<double> theta;

    Shape shape;

    BodyType bodyType;
    ShapeType shapeType;
    CollisionType collisionType;
    std::vector<CollisionType> acceptedCollision;

    std::list<ConstructedBody>::iterator constructedBodyIt;
};

} // namespace grapito
} // namespace ad
