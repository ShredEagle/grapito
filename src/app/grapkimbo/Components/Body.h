#pragma once

#include "../Components/AccelAndSpeed.h"
#include "../Utils/CollisionBox.h"
#include "../Utils/Shape.h"
#include "../Utils/PhysicsStructs.h"
#include "../Utils/PhysicsMathUtilities.h"

#include <algorithm>
#include <aunteater/Component.h>

namespace ad {
namespace grapito
{

struct Body : public aunteater::Component<Body>
{
    explicit Body(
        std::vector<Position2> aVertices,
        BodyType aBodyType,
        ShapeType aShapeType,
        CollisionType aCollisionType,
        float aMass = 1.f,
        float aTheta = 0.f,
        float aFriction = 0.f,
        float gravityScale = 1.f,
        std::vector<CollisionType> aAcceptedCollision = {},
        std::vector<CollisionType> aAcceptedDetection = {}
    ) :
        mass{aMass},
        moi{0.f},
        gravityScale{gravityScale},
        friction{aFriction},
        theta{math::Radian<float>{aTheta}},
        shape{aVertices},
        bodyType{aBodyType},
        shapeType{aShapeType},
        collisionType{aCollisionType},
        acceptedCollision{aAcceptedCollision},
        acceptedDetection{aAcceptedDetection}
    {
        updateData();
    }

    explicit Body(
        math::Rectangle<float> aBox,
        BodyType aBodyType,
        ShapeType aShapeType,
        CollisionType aCollisionType,
        float aMass = 1.f,
        float aTheta = 0.f,
        float aFriction = 0.f,
        float gravityScale = 1.f,
        std::vector<CollisionType> aAcceptedCollision = {},
        std::vector<CollisionType> aAcceptedDetection = {}
    ) :
        mass{aMass},
        moi{0.f},
        gravityScale{gravityScale},
        friction{aFriction},
        theta{math::Radian<float>{aTheta}},
        shape{aBox},
        bodyType{aBodyType},
        shapeType{aShapeType},
        collisionType{aCollisionType},
        acceptedCollision{aAcceptedCollision},
        acceptedDetection{aAcceptedDetection}
    {
        updateData();
    }

    void updateData()
    {
        float area = 0.f;

        Vec2 vecMassCenter = Vec2::Zero();
        for (size_t i = 0; i < shape.mFaceCount; ++i)
        {
            Shape::Edge edge = shape.getEdge(i);
            Vec2 vertexA = edge.origin.as<math::Vec>();
            Vec2 vertexB = edge.end.as<math::Vec>();
            float areaStep = std::abs(twoDVectorCross(vertexA, vertexB)) / 2.f;
            Vec2 centerStep = (vertexA + vertexB) / 3.f;
            float moiStep = areaStep * (vertexA.dot(vertexA) + vertexB.dot(vertexB) + vertexA.dot(vertexB)) / 6.f;

            if (area + areaStep != 0.f)
            {
                vecMassCenter = (vecMassCenter * area + centerStep * areaStep)/(area + areaStep);
            }

            area += areaStep;
            moi += moiStep;
        }

        massCenter = static_cast<Position2>(vecMassCenter);

        if (bodyType == BodyType_Static)
        {
            mass = 0.f;
            invMass = 0.f;
            moi = 0.f;
            invMoi = 0.f;
        }
        else if (bodyType == BodyType_Kinematic)
        {
            //Kinematic object are affected by gravity but not physic collision
            invMass = 0.f;
            invMoi = 0.f;
        }
        else
        {
            float density = mass / area;
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
        for (size_t i = 0; i < shape.mFaceCount; ++i)
        {
            auto vertex = shape.getVertice(i);
            debugDrawer->drawPoint({
                    transformPosition(
                            (Position2)pos + vertex.as<math::Vec>(),
                            theta,
                            (Position2)pos + massCenter.as<math::Vec>()
                    ),
                    .05f,
                    {255,255,0},
            });
        }
        debugDrawer->drawPoint({
                (Position2)pos + massCenter.as<math::Vec>(),
                .05f,
                {255,100,0},
        });
    }

    float mass;
    float invMass;
    float moi;
    float invMoi;
    float gravityScale;
    Position2 massCenter = {0.f, 0.f};

    float friction;
    float restitution;

    math::Radian<float> theta;

    Shape shape;

    BodyType bodyType;
    ShapeType shapeType;
    CollisionType collisionType;
    std::vector<CollisionType> acceptedCollision;
    std::vector<CollisionType> acceptedDetection;

    std::list<ConstructedBody>::iterator constructedBodyIt;
};

} // namespace grapito
} // namespace ad
