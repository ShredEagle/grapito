#include "PhysicsStructs.h"
#include "Components/Body.h"
#include "Components/Position.h"

#include "Utils/CollisionBox.h"
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
    radius{aBody.radius},
    bodyType{aBody.bodyType},
    shapeType{aBody.shapeType},
    collisionType{aBody.collisionType},
    bodyRef{aBody},
    posRef{aPos},
    aasRef{aAas},
    entity{aEntity}
{
}

void ConstructedBody::synchronize(
        std::vector<Velocity> & velocities,
        std::vector<BodyPosition> & bodyPoses,
        std::vector<CollisionBox> & collisionBoxes,
        int index
)
{
    box = &collisionBoxes.at(index);
    velocity = &velocities.at(index);
    bodyPos = &bodyPoses.at(index);
}

void ConstructedBody::updateEntity()
{
    bodyRef.theta = bodyPos->a;
    aasRef.speed = velocity->v;
    aasRef.w = velocity->w;
    posRef.position = bodyPos->p;
}

void ConstructedBody::debugRender()
{
    for (int i = 0; i < box->shape.mFaceCount; ++i)
    {
        auto vertex = box->shape.getVertice(i);
        debugDrawer->drawPoint({
                vertex,
                .05f,
                {255,255,0},
        });
    }
    debugDrawer->drawPoint({
            bodyPos->c,
            .05f,
            {255,100,0},
    });
}

void CollisionPair::debugRender()
{
    debugDrawer->drawLine({
            bodyA.bodyPos->c,
            bodyB.bodyPos->c,
            0.,
            {255,0,255},
    });
}

void VelocityConstraint::debugRender()
{
    debugDrawer->drawPoint({
            cf.contactPoint,
            .05,
            {0,0,255},
    });
    /*
    debugDrawer->drawLine({
            cf.contactPoint,
            static_cast<Position2>(cf.contactPoint.as<math::Vec>() + normal * 3),
            .05,
            {255,0,0},
    });
    debugDrawer->drawLine({
            cf.contactPoint,
            static_cast<Position2>(cf.contactPoint.as<math::Vec>() + cf.normalImpulse * normal * 3),
            .05,
            {150,150,200},
    });

    Vec2 AngVecA = {-rA.y(), rA.x()};
    Vec2 AngVecB = {-rB.y(), rB.x()};
    Vec2 speed = velocityA->v + (AngVecA * velocityA->w) - velocityB->v - (AngVecB * velocityB->w);

    debugDrawer->drawLine({
            cf.contactPoint,
            static_cast<Position2>(cf.contactPoint.as<math::Vec>() + speed.dot(normal) * normal),
            .05,
            {0,200,0},
    });
    */
}

std::ostream &operator<<(std::ostream & os, const VelocityConstraint & vc)
{
    os << "{ VelocityConstraint\n";
    os << "    contact : {\n";
    os << "        x : " << vc.cf.contactPoint.x() << "\n";
    os << "        y : " << vc.cf.contactPoint.y() << "\n";
    os << "        nI : " << vc.cf.normalImpulse << "\n";
    os << "        tI : " << vc.cf.tangentImpulse << "\n";
    os << "    }\n";
    os << "    normal : {\n";
    os << "        x : " << vc.normal.x() << "\n";
    os << "        y : " << vc.normal.y() << "\n";
    os << "    }\n";
    os << "    position A : {\n";
    os << "        x : " << vc.bodyPosA->p.x() << "\n";
    os << "        y : " << vc.bodyPosA->p.y() << "\n";
    os << "    }\n";
    os << "    position B : {\n";
    os << "        x : " << vc.bodyPosB->p.x() << "\n";
    os << "        y : " << vc.bodyPosB->p.y() << "\n";
    os << "    }\n";
    os << "    vA : " << vc.velocityA->v << "\n";
    os << "    vB : " << vc.velocityB->v << "\n";
    os << "    wA : " << vc.velocityA->w << "\n";
    os << "    wB : " << vc.velocityB->w << "\n";
    os << "    invMassA : " << vc.invMassA << "\n";
    os << "    invMassB : " << vc.invMassB << "\n";
    os << "    invMoiA : " << vc.invMoiA << "\n";
    os << "    invMoiB : " << vc.invMoiB << "\n";
    os << "}\n";
    return os;
}

std::ostream &operator<<(std::ostream & os, const ConstructedBody & cb)
{
    os << "{ ConstructedBody\n";
    os << "    x : " << cb.bodyPos->p.x() << "\n";
    os << "    y : " << cb.bodyPos->p.y() << "\n";
    os << "    speed : " << cb.velocity->v << "\n";
    os << "    w : " << cb.velocity->w << "\n";
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

std::ostream &operator<<(std::ostream & os, const ContactFeature & cm)
{
    os << "{ ContactFeature\n";
    os << "    refIndex : " << unsigned(cm.indexReference) << "\n";
    os << "    incIndex : " << unsigned(cm.indexIncident) << "\n";
    os << "    typeReference : " << unsigned(cm.typeReference) << "\n";
    os << "    typeIncident : " << unsigned(cm.typeIncident) << "\n";
    os << "}\n";
    return os;
}
}
}
