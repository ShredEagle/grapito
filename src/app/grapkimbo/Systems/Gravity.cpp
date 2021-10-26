#include "Gravity.h"

#include "../Configuration.h"

#include <math/Vector.h>

namespace ad {
namespace grapito
{

Gravity::Gravity(aunteater::EntityManager & aEntityManager) :
    mMassives(aEntityManager),
    mPlayers(aEntityManager)
{}

void Gravity::update(const GrapitoTimer aTimer, const GameInputState &)
{
    for(auto massive : mMassives)
    {
        if (massive->get<Body>().bodyType != BodyType_Static)
        {
            const Vec2 gravityVector = {0.f, - player::gAcceleration * massive->get<Body>().gravityScale};
            massive->get<AccelAndSpeed>().accel += gravityVector;
        }
    }

    for (auto player: mPlayers)
    {
        player->get<AccelAndSpeed>().speed.y() = std::max(player::gAirMaxFallSpeed, player->get<AccelAndSpeed>().speed.y());
    }
}
} // namespace grapito
} // namespace ad
