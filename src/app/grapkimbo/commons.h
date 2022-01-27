#pragma once


#include <handy/StringId.h>
#include <sounds/SoundManager.h>

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

struct Context;

class GrapitoTimer;

using StringId = handy::StringId;
using SoundManager = sounds::SoundManager;
using OggSoundData = sounds::OggSoundData;

using AtlasIndex = std::size_t;


} // namespace grapito
} // namespace ad 
