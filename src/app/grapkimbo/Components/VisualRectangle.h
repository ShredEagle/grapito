#pragma once

#include <aunteater/Component.h>


namespace ad {


struct VisualRectangle : public aunteater::Component<VisualRectangle>
{
    explicit VisualRectangle(Color aColor = math::sdr::gWhite) :
        color{aColor}
    {}

    Color color;
};



} // namespace ad
