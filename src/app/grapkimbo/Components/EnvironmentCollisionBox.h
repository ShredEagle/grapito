#pragma once


#include <Utils/CollisionBox.h>
#include "Utils/DrawDebugStuff.h"
#include "aunteater/globals.h"
#include "engine/commons.h"
#include "math/Vector.h"

#include <aunteater/Component.h>

#include <cmath>
#include <vector>

namespace ad {

enum EntityType
{
    GROUND,
    PLAYER,
    PHYSICAL,
};

enum CollisionBoxType
{
    AABB,
    type_count,
};

struct ContactPointQuery
{
    ContactPointQuery(
        const double aDistance = NAN,
        const math::Position<2, double> aContactPoint = math::Position<2, double>::Zero(),
        const math::Vec<2, double> aNormal = math::Vec<2, double>::Zero(),
        const math::Position<2, double> aOrigin = math::Position<2, double>::Zero(),
        const math::Position<2, double> aEnd = math::Position<2, double>::Zero(),
        const math::Position<2, double> aPoint = math::Position<2, double>::Zero()
    ) :
        distance{aDistance},
        contactPoint{aContactPoint},
        normal{aNormal},
        origin{aOrigin},
        end{aEnd},
        point{aPoint},
        impulse{0.f}
    {};

    void debugRender(Color vecColor)
    {
            debugDrawer->drawLine(
                {
                    contactPoint + math::Vec<2, double>{3.f, 3.f},
                    contactPoint + math::Vec<2, double>{-3.f, -3.f},
                    2.f,
                    vecColor
                }
            );
            debugDrawer->drawLine(
                {
                    contactPoint + math::Vec<2, double>{3.f, -3.f},
                    contactPoint + math::Vec<2, double>{-3.f, 3.f},
                    2.f,
                    vecColor
                }
            );
            debugDrawer->drawLine(
                {
                    point + math::Vec<2, double>{3.f, 3.f},
                    point + math::Vec<2, double>{-3.f, -3.f},
                    2.f,
                    vecColor
                }
            );
            debugDrawer->drawLine(
                {
                    point + math::Vec<2, double>{3.f, -3.f},
                    point + math::Vec<2, double>{-3.f, 3.f},
                    2.f,
                    vecColor
                }
            );
            debugDrawer->drawLine(
                {
                    origin,
                    end,
                    2.f,
                    vecColor
                }
            );
            debugDrawer->drawLine(
                {
                    contactPoint,
                    contactPoint - normal * distance,
                    2.f,
                    vecColor
                }
            );
            debugDrawer->drawLine(
                {
                    (origin + static_cast<math::Vec<2, double>>(end)) / 2,
                    (origin + static_cast<math::Vec<2, double>>(end)) / 2 + normal * 20,
                    2.f,
                    vecColor
                }
            );
    }

    double distance;
    math::Position<2, double> contactPoint;
    math::Vec<2, double> normal;
    math::Position<2, double> origin;
    math::Position<2, double> end;
    math::Position<2, double> point;
    double impulse;
};

struct ContactWith
{
    ContactPointQuery contact;
    aunteater::weak_entity entity;

    void debugRender(Color vecColor)
    {
        contact.debugRender(vecColor);
    }
};


struct EnvironmentCollisionBox : public aunteater::Component<EnvironmentCollisionBox>
{
    explicit EnvironmentCollisionBox(
        math::Rectangle<double> aBox = math::Rectangle<double>{{0., 0.}, {1. , 1.}},
        const EntityType aEntityType = EntityType::GROUND,
        const CollisionBoxType aEcbType = CollisionBoxType::AABB
    ):
        box{aBox},
        entityType{aEntityType},
        ecbType{aEcbType}
    {}

    CollisionBox box;
    std::vector<ContactWith> collidingWith;
    const EntityType entityType;
    const CollisionBoxType ecbType;
};

} // namespace ad
