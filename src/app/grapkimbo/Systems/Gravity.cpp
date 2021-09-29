#include "Gravity.h"

#include <math/Vector.h>

namespace ad {
namespace grapito
{

const Vec2 gravityVector = {0., - Gravity::gAcceleration};

Gravity::Gravity(aunteater::EntityManager & aEntityManager) :
    mMassives(aEntityManager)
{}

void Gravity::update(const aunteater::Timer aTimer, const GameInputState &)
{
    for(auto massive : mMassives)
    {
        if (massive->get<Body>().bodyType != BodyType_Static)
        {
            massive->get<AccelAndSpeed>().accel += gravityVector;
        }
    }
}
} // namespace grapito
} // namespace ad
