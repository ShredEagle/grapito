#pragma once

#include "Utils/Contact.h"
#include "Utils/CollisionBox.h"

#include <aunteater/Component.h>

namespace ad {

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
        float aMass = 1,
        float theta = 0.
    ) :
        box{std::move(aBox), theta},
        bodyType{aBodyType},
        shapeType{aShapeType}
    {
        if (bodyType != BodyType::DYNAMIC)
        {
            mass = 0.;
            invMass = 0.;
        }
        else
        {
            mass = aMass;
            invMass = 1 / aMass;
        }
    }

    double mass;
    double invMass;
    BodyType bodyType;
    CollisionBox box;
    ShapeType shapeType;
    std::vector<ContactQuery> collidingWith;
};


} // namespace ad
