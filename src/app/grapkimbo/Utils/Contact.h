#pragma once

#include "commons.h"

#include "aunteater/globals.h"
#include "math/Vector.h"
#include "Utils/DrawDebugStuff.h"

#include <vector>

namespace ad
{
namespace grapito
{

struct Contact
{
    Position2 point;
    double impulse = 0.;
};

struct ContactQuery
{
    ContactQuery(
        const aunteater::weak_entity aEntity
    ) :
        entity{aEntity}
    {};


#ifdef KIMBO_DEBUG
    void debugRender(Color vecColor)
    {
        for (auto contact : contacts)
        {
            debugDrawer->drawPoint(
                {
                    contact.point,
                    .05,
                    Color{0, 0, 255}
                }
            );
            /*
            debugDrawer->drawPoint(
                {
                    point,
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
                    contact.point,
                    contact.point - normal * distance,
                    2.f,
                    vecColor
                }
            );
            debugDrawer->drawLine(
                {
                    (origin + static_cast<Vec2>(end)) / 2,
                    (origin + static_cast<Vec2>(end)) / 2 + normal * 20,
                    2.f,
                    vecColor
                }
            );
            debugDrawer->drawPoint({
                end,
                2.,
                Color{0,255,0}
            });
            debugDrawer->drawPoint({
                origin,
                2.,
            });
            */
        }
    }
#endif

#ifdef KIMBO_DEBUG
    Position2 origin = Position2::Zero();
    Position2 end = Position2::Zero();
    Position2 point = Position2::Zero();
#endif

    aunteater::weak_entity entity;
    double distance = 0.;
    Vec2 normal = Vec2::Zero();
    int index = -1;
    std::vector<Contact> contacts;
};
} //namespace grapito
} //namespace ad
