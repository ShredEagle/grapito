#pragma once


#include <aunteater/Component.h>


namespace ad {
namespace grapito {


struct Text : public aunteater::Component<Text>
{
    Text(std::string aMessage) :
        message{std::move(aMessage)}
    {}

    std::string message;
};


} // namespace grapito
} // namespace ad
