#include "Gravity.h"

#include "../Configuration.h"

#include <math/Vector.h>

namespace ad {
namespace grapito
{

Gravity::Gravity(aunteater::EntityManager & aEntityManager) :
    mMassives(aEntityManager)
{}

void Gravity::update(const GrapitoTimer aTimer, const GameInputState &)
{
    for(auto massive : mMassives)
    {
        if (massive->get<Body>().bodyType != BodyType_Static)
        {
            const Vec2 gravityVector = {0.f, - player::gAcceleration};
            massive->get<AccelAndSpeed>().accel += gravityVector;
        }
    }
}
} // namespace grapito
} // namespace ad
