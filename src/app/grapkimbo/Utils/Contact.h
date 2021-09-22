#pragma once

#include "Utils/CollisionBox.h"
#include "commons.h"

#include "aunteater/globals.h"
#include "math/Vector.h"
#include "Utils/DrawDebugStuff.h"

#include <vector>

namespace ad
{
namespace grapito
{


struct ContactDepecrated
{
    Position2 point;
    double impulse = 0.;
    double tangentImpulse = 0.;
};
} //namespace grapito
} //namespace ad
