#pragma once

#include <aunteater/Timer.h>
#include <math/Vector.h>

namespace ad
{
namespace grapito
{

typedef math::Vec<2, float> Vec2;
typedef math::Vec<3, float> Vec3;
typedef math::Position<2, float> Position2;
typedef math::Position<3, double> Position3;
using GrapitoTimer = aunteater::Timer_base<float>;

}
}
