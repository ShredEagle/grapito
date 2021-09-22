#include "Contact.h"

namespace ad {
namespace grapito {


#ifdef KIMBO_DEBUG
void ContactQuery::debugRender(Color vecColor)
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
            Color{0,255,0}
        });
    }
#endif

}
}
}

