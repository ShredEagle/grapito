#include "PhysicsStructs.h"
#include "Components/Body.h"
#include "Components/Position.h"

#include "Utils/DrawDebugStuff.h"
#include "math/Vector.h"
#include <iostream>


namespace ad {
namespace grapito {

ConstructedBody::ConstructedBody(Body & aBody, Position & aPos, AccelAndSpeed & aAas, aunteater::weak_entity aEntity) :
    mass{aBody.mass},
    invMass{aBody.invMass},
    moi{aBody.moi},
    invMoi{aBody.invMoi},
    friction{aBody.friction},
    speed{aAas.speed},
    w{aAas.w},
    massCenter{static_cast<Position2>(aBody.massCenter.as<math::Vec>() + aPos.position.as<math::Vec>())},
    position{aPos.position},
    theta{aBody.theta},
    radius{aBody.radius},
    box{createTransformedCollisionBox(aBody, aPos)},
    bodyType{aBody.bodyType},
    shapeType{aBody.shapeType},
    bodyRef{aBody},
    posRef{aPos},
    aasRef{aAas},
    entity{aEntity}
{
}

void ConstructedBody::synchronize()
{
    box = CollisionBox{createTransformedCollisionBox(bodyRef, posRef)};
    position = posRef.position;
    theta = bodyRef.theta;
    speed = aasRef.speed;
    w = aasRef.w;
    massCenter = static_cast<Position2>(bodyRef.massCenter.as<math::Vec>() + posRef.position.as<math::Vec>());
}

void ConstructedBody::updateEntity()
{
    bodyRef.theta = theta;
    aasRef.speed = speed;
    aasRef.w = w;
    posRef.position = position;
}

std::vector<Position2> ConstructedBody::createTransformedCollisionBox(Body & aBody, Position & aPos)
{
    std::vector<Position2> transformedVertices;

    for (auto vertex : aBody.shape.vertices)
    {
        auto transformedPos = transformPosition(
                static_cast<Position2>(vertex.as<math::Vec>() + aPos.position.as<math::Vec>()),
                aBody.theta,
                static_cast<Position2>(aBody.massCenter.as<math::Vec>() + aPos.position.as<math::Vec>())
                );
        transformedVertices.emplace_back(transformedPos);
    }

    return transformedVertices;
}

void ConstructedBody::debugRender()
{
    for (int i = 0; i < box.shape.mFaceCount; ++i)
    {
        auto vertex = box.shape.getVertice(i);
        debugDrawer->drawPoint({
                vertex,
                .05f,
                {255,255,0},
        });
    }
    debugDrawer->drawPoint({
            massCenter,
            .05f,
            {255,100,0},
    });
}

void CollisionPair::debugRender()
{
    debugDrawer->drawLine({
            bodyA.massCenter,
            bodyB.massCenter,
            0.,
            {255,0,255},
    });
}

void VelocityConstraint::debugRender()
{
    debugDrawer->drawPoint({
            cf.contactPoint,
            .05,
            {255,0,0},
    });
    debugDrawer->drawLine({
            cf.contactPoint,
            static_cast<Position2>(cf.contactPoint.as<math::Vec>() + normal * 3),
            .05,
            {255,0,0},
    });
}

std::ostream &operator<<(std::ostream & os, const VelocityConstraint & vc)
{
    os << "{ VelocityConstraint\n";
    os << "    contact : {\n";
    os << "        x : " << vc.cf.contactPoint.x() << "\n";
    os << "        y : " << vc.cf.contactPoint.y() << "\n";
    os << "    }\n";
    os << "    vA : " << vc.vA << "\n";
    os << "    vB : " << vc.vB << "\n";
    os << "    invMassA : " << vc.invMassA << "\n";
    os << "    invMassB : " << vc.invMassB << "\n";
    os << "}\n";
    return os;
}

std::ostream &operator<<(std::ostream & os, const ConstructedBody & cb)
{
    os << "{ ConstructedBody\n";
    os << "    x : " << cb.position.x() << "\n";
    os << "    y : " << cb.position.y() << "\n";
    os << "    speed : " << cb.speed << "\n";
    os << "    w : " << cb.w << "\n";
    os << "    bodyType : " << cb.bodyType << "\n";
    os << "}\n";
    return os;
}

std::ostream &operator<<(std::ostream & os, const ContactManifold & cm)
{
    os << "{ ContactManifold\n";
    os << "    Face : " << cm.face << "\n";
    os << "    refIndex : " << cm.referenceEdgeIndex << "\n";
    os << "    incIndex : " << cm.incidentEdgeIndex << "\n";
    os << "    separation : " << cm.separation << "\n";
    os << "    normal : " << cm.normal << "\n";
    os << "    contacts # : " << cm.contacts.size() << "\n";
    os << "}\n";
    return os;
}
}
}
