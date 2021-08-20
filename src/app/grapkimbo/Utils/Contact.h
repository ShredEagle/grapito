#pragma once

#include "aunteater/globals.h"
#include "math/Vector.h"
#include "Utils/DrawDebugStuff.h"

#include <vector>

namespace ad
{
struct Contact
{
    math::Position<2, double> point;
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
            /*
            debugDrawer->drawPoint(
                {
                    contact.point,
                    2.f,
                    vecColor
                }
            );
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
                    (origin + static_cast<math::Vec<2, double>>(end)) / 2,
                    (origin + static_cast<math::Vec<2, double>>(end)) / 2 + normal * 20,
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
                Color{0, 0, 255}
            });
            */
        }
    }
#endif

#ifdef KIMBO_DEBUG
    math::Position<2, double> origin = math::Position<2, double>::Zero();
    math::Position<2, double> end = math::Position<2, double>::Zero();
    math::Position<2, double> point = math::Position<2, double>::Zero();
#endif

    aunteater::weak_entity entity;
    double distance = 0.;
    math::Vec<2, double> normal = math::Vec<2, double>::Zero();
    int index = -1;
    std::vector<Contact> contacts;
};
}
