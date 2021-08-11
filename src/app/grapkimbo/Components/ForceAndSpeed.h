#pragma once

#include <aunteater/Component.h>
#include <math/Vector.h>

#include <vector>

namespace ad {

struct ForceAndSpeed : public aunteater::Component<ForceAndSpeed>
{
    explicit ForceAndSpeed(math::Vec<2, double> baseSpeed = math::Vec<2, double>::Zero())
    {
        speeds.push_back(baseSpeed);
    }

    std::vector<math::Vec<2, double>> forces;
    std::vector<math::Vec<2, double>> speeds;
};


} // namespace ad
