#pragma once


#include <aunteater/Component.h>


namespace ad {
namespace grapito {


struct Text : public aunteater::Component<Text>
{
    Text(std::string aMessage, math::sdr::Rgba aColor = math::sdr::gWhite) :
        message{std::move(aMessage)},
        color{aColor}
    {}

    std::string message;
    math::sdr::Rgba color;
};


} // namespace grapito
} // namespace ad
