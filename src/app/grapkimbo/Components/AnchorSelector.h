#pragma once

#include <aunteater/Component.h>

#include <math/Angle.h>


namespace ad {


struct AnchorSelector : public aunteater::Component<AnchorSelector>
{
    AnchorSelector(double aReachDistance,
                   math::Degree<double> aTolerance,
                   aunteater::weak_entity aPlayer, 
                   aunteater::weak_entity aAnchor = nullptr) :
        reachDistanceSquared{std::pow(aReachDistance, 2)},
        tolerance{aTolerance},
        player{aPlayer},
        anchor{aAnchor}
    {}

    aunteater::weak_entity currentAnchor() const
    {
        return anchorToCommit ? anchorToCommit : anchor;
    }

    void commit()
    {
        anchor = anchorToCommit;
        anchorToCommit = nullptr;
    }

    double reachDistanceSquared;
    math::Degree<double> tolerance;
    
    // TODO need a much better way to store reference to entities across update steps.
    aunteater::weak_entity player{nullptr};
    aunteater::weak_entity anchor{nullptr};
    aunteater::weak_entity anchorToCommit{nullptr};
};



} // namespace ad
