#pragma once


#include <array>
#include <stdexcept>
#include <string>


namespace ad {
namespace grapito {


enum class PlayerAnimation
{
    Initial,
    IdleLeft,
    IdleRight,
    RunLeft,
    RunRight,

    // Keep me last
    _Count,
};


enum class AnimationVariant
{
    Default,
    Blue,
    Green,
    Red,
};


// TODO Forget to update that when Franz design the 4th color variant
constexpr std::array<AnimationVariant, 3> gAllColorVariants{
    AnimationVariant::Blue,
    AnimationVariant::Green,
    AnimationVariant::Red,
};


inline std::string toString(AnimationVariant aVariant)
{
    switch(aVariant)
    {
    default:
        throw std::invalid_argument(
            // MSVC, making you feel like it is v6.0 all over again.
            //std::format(
            //    "Invalid AnimatonVariant value: {}.",
            //    (std::underlying_type_t<AnimationVariant>)aVariant));
            "Invalid AnimatonVariant value: " 
            + std::to_string((std::underlying_type_t<AnimationVariant>)aVariant));
    case AnimationVariant::Default:
        return "";
    case AnimationVariant::Blue:
        return "blue";
    case AnimationVariant::Green:
        return "green";
    case AnimationVariant::Red:
        return "red";
    }
}


} // namespace grapito
} // namespace ad 
