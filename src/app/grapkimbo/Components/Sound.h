#pragma once

#include <aunteater/Component.h>
#include <string>

namespace ad {
namespace grapito
{


struct Sound : public aunteater::Component<Sound>
{
    explicit Sound(std::string aFilename) :
        mFilename{aFilename}
    {}

    std::string mFilename;
};


} // namespace grapito
} // namespace ad
