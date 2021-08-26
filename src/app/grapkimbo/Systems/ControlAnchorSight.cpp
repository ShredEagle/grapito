#include "ControlAnchorSight.h"

#include "../Utilities.h"

#include <math/VectorUtilities.h>


namespace ad {


ControlAnchorSight::ControlAnchorSight(aunteater::EntityManager & aEntityManager) :
    mAnchorSights{aEntityManager},
    mAnchorables{aEntityManager}
{}


using AnchorWrap = aunteater::FamilyHelp<AnchorElement>::const_Wrap;


void ControlAnchorSight::update(const aunteater::Timer aTimer, const GameInputState & aInputState)
{
    for (auto & [selector, controllable, geometry] : mAnchorSights)
    {
        math::Vec<2, double> inputDirection{aInputState.getRightDirection(controllable.controller, 0.8f)};

        auto positionProvider = [](const Rectangle<double> aCandidate)
        {
            return aCandidate.center();
        };

        auto filter = [&](const AnchorWrap & anchor, math::Vec<2, double> aVec, double aNormSquared)
        {
            return anchor != selector.anchor  // eliminate the currently selected anchor
                   && aVec.getNormSquared() < std::min(aNormSquared, selector.reachDistanceSquared)
                   && abs(math::getOrientedAngle(aVec, inputDirection)) < selector.tolerance
                ;
        };

        if (selector.currentAnchor()) 
        {
            // There is a deadzone, so it will be exactly zero when no direction is considered
            if (inputDirection != math::Vec<2, double>::Zero())
            {
                auto nextAnchor = getClosest(mAnchorables,
                                             selector.anchor->get<Position>().center(),
                                             positionProvider,
                                             filter);

                selector.anchorToCommit = nextAnchor ? nextAnchor->entity : (aunteater::weak_entity)nullptr;
            }
            else if (selector.anchorToCommit)
            {
                selector.commit();
            }

            geometry = selector.currentAnchor()->get<Position>();
        }
    }
}


} // namespace ad
