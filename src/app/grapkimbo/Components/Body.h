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
        math::Rectangle<float> aBox,
        BodyType aBodyType,
        ShapeType aShapeType,
        CollisionType aCollisionType,
        float aMass = 1.f,
        float aTheta = 0.f,
        float aFriction = 0.f,
        bool noMaxFriction = false,
        std::vector<CollisionType> aAcceptedCollision = {}
    ) :
        mass{aMass},
        moi{0.f},
        friction{aFriction},
        noMaxFriction{noMaxFriction},
        theta{math::Radian<float>{aTheta}},
        shape{aBox},
        bodyType{aBodyType},
        shapeType{aShapeType},
        collisionType{aCollisionType},
        acceptedCollision{aAcceptedCollision}
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
            std::cout << "VertexA " << vertexA << std::endl;
            Vec2 vertexB = edge.end.as<math::Vec>();
            std::cout << "VertexB " << vertexB << std::endl;
            float areaStep = twoDVectorCross(vertexA, vertexB) / 2.f;
            std::cout << "area step " << areaStep << std::endl;
            Vec2 centerStep = (vertexA + vertexB) / 3.f;
            std::cout << "a.dot(a) " << vertexA.dot(vertexA) << std::endl;
            std::cout << "b.dot(b) " << vertexB.dot(vertexB) << std::endl;
            std::cout << "a.dot(b) " << vertexA.dot(vertexB) << std::endl;
            float moiStep = areaStep * (vertexA.dot(vertexA) + vertexB.dot(vertexB) + vertexA.dot(vertexB)) / 6.f;
            std::cout << "moi step " << moiStep << std::endl;

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
            std::cout << "Moi " << moi << std::endl;
            invMass = 1 / mass;
            invMoi = 1 / moi;
            std::cout << "invmoi " << invMoi << std::endl;
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

    float mass = 0.f;
    float invMass = 0.f;
    float moi = 0.f;
    float invMoi = 0.f;
    Position2 massCenter = {0.f, 0.f};

    float friction;
    float restitution;
    bool noMaxFriction;

    math::Radian<float> theta;

    Shape shape;

    BodyType bodyType;
    ShapeType shapeType;
    CollisionType collisionType;
    std::vector<CollisionType> acceptedCollision;

    std::list<ConstructedBody>::iterator constructedBodyIt;
};

} // namespace grapito
} // namespace ad
