#include "Gravity.h"

#include <math/Vector.h>

namespace ad {

const math::Vec<2, double> gravityVector = {0., - Gravity::gAcceleration};

Gravity::Gravity(aunteater::EntityManager & aEntityManager) :
    mMassives(aEntityManager)
{}

void Gravity::update(const aunteater::Timer aTimer, const GameInputState &)
{
    for(auto massive : mMassives)
    {
        if (massive->get<Body>().bodyType != BodyType::STATIC)
        {
            massive->get<AccelAndSpeed>().accel += gravityVector;
        }
    }
}
} // namespace ad
