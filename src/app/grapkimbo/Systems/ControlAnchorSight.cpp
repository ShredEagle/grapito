#include "ControlAnchorSight.h"

#include <math/VectorUtilities.h>


namespace ad {


ControlAnchorSight::ControlAnchorSight(aunteater::EntityManager & aEntityManager) :
    mAnchorSights{aEntityManager},
    mAnchorables{aEntityManager}
{}


using AnchorWrap = aunteater::FamilyHelp<AnchorElement>::const_Wrap;

template <class T_positionProvider, class T_filter> 
std::optional<std::pair<AnchorWrap, double>> getClosest(
        const aunteater::FamilyHelp<AnchorElement> & aAnchorables,
        const math::Position<2, double> aPosition,
        T_positionProvider && aProvider,
        T_filter && aFilter = 
            [](const AnchorWrap &, math::Vec<2, double> aVec, double aNormSquared)
            {
                return aVec.getNormSquared() < aNormSquared;
            })
{
    std::optional<AnchorWrap> closestAnchorable;
    double normSquared = std::numeric_limits<double>::max();

    for (const auto anchorable : aAnchorables) 
    {
        const auto [geometry, _discard] =  anchorable;
        math::Rectangle<double> anchorableRect{geometry.position, geometry.dimension};

        math::Position<2, double> candidate = aProvider(anchorableRect);

        if (aFilter(anchorable, candidate - aPosition, normSquared))
        {
            normSquared = (candidate - aPosition).getNormSquared();
            closestAnchorable = anchorable;
        }
    }

    if (closestAnchorable)
    {
        return {{*closestAnchorable, std::sqrt(normSquared)}};
    }
    else
    {
        return {};
    }
}



void ControlAnchorSight::update(const aunteater::Timer aTimer, const GameInputState & aInputState)
{
    for (auto & [selector, controllable, geometry] : mAnchorSights)
    {
        math::Vec<2, double> inputDirection{aInputState.getRightDirection(controllable.controller, 0.6f)};

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

                selector.anchorToCommit = nextAnchor ? nextAnchor->first : (aunteater::weak_entity)nullptr;
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
