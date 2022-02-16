#pragma once


#include <aunteater/Component.h>


namespace ad {
namespace grapito {


struct Text : public aunteater::Component<Text>
{
    enum Size
    {
        Small,
        Large,

        _End/*
    */};


    Text(std::string aMessage,
         math::sdr::Rgba aColor = math::sdr::gWhite,
         Size aSize = Large) :
        message{std::move(aMessage)},
        color{aColor},
        size{aSize}
    {}

    std::string message;
    math::sdr::Rgba color;
    Size size;
};


} // namespace grapito
} // namespace ad
