#pragma once


#include <aunteater/Timer.h>

#include <handy/StringId.h>

#include <math/Rectangle.h>
#include <math/Vector.h>


namespace ad {
namespace grapito {


using Vec2 = math::Vec<2, float>;
using Vec3 = math::Vec<3, float>;
using Position2 = math::Position<2, float>;
using Position3 = math::Position<3, float>;
using Size2 = math::Size<2, float>;
using Size3 = math::Size<3, float>;
using Rectangle = math::Rectangle<float>;

using GrapitoTimer = aunteater::Timer_base<float>;


using StringId = handy::StringId;


} // namespace grapito
} // namespace ad 
