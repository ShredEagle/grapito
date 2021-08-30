#include "ControlAnchorSight.h"

#include "../Entities.h"
#include "../Utilities.h"

#include <math/VectorUtilities.h>


namespace ad {


ControlAnchorSight::ControlAnchorSight(aunteater::EntityManager & aEntityManager) :
    mAnchorSights{aEntityManager},
    mAnchorables{aEntityManager}
{}


using AnchorWrap = aunteater::FamilyHelp<AnchorElement>::const_Wrap;


void ControlAnchorSight::positionSight(AnchorSelector & aSelector,
                                       Position & aGeometry,
                                       math::Vec<2, double> aInputDirection) const
{
    auto positionProvider = [](const Rectangle<double> aCandidate)
    {
        return aCandidate.center();
    };

    auto filter = [&](const AnchorWrap & anchor, math::Vec<2, double> aVec, double aNormSquared)
    {
        return anchor != aSelector.anchor  // eliminate the currently selected anchor
               && aVec.getNormSquared() < std::min(aNormSquared, aSelector.reachDistanceSquared)
               && abs(math::getOrientedAngle(aVec, aInputDirection)) < aSelector.tolerance
            ;
    };

    math::Position<2, double> basePosition = aSelector.anchor ?
        aSelector.anchor->get<Position>().center()
        : aSelector.player->get<Position>().center();

    // There is a deadzone, so it will be exactly zero when no direction is considered
    if (aInputDirection != math::Vec<2, double>::Zero())
    {
        auto nextAnchor = getClosest(mAnchorables, basePosition, positionProvider, filter);

        aSelector.anchorToCommit = nextAnchor ? nextAnchor->entity : (aunteater::weak_entity)nullptr;
    }
    else if (aSelector.anchorToCommit)
    {
        aSelector.commit();
    }

    if (auto anchor = aSelector.currentAnchor())
    {
        aGeometry = anchor->get<Position>();
    }
    else
    {
        aGeometry = aSelector.player->get<Position>();
    }
}


void ControlAnchorSight::update(const aunteater::Timer aTimer, const GameInputState & aInputState)
{
    for (auto & [selector, controllable, geometry] : mAnchorSights)
    {
        math::Vec<2, double> inputDirection{aInputState.getRightDirection(controllable.controller, 0.8f)};
        positionSight(selector, geometry, inputDirection);

        if (aInputState.get(controllable.controller)[Grapple])
        {
            aunteater::weak_entity anchor = selector.currentAnchor();
            aunteater::weak_entity player = selector.player;
            if (anchor && anchor->has<Position>() && player->has<Position>() && player->has<AccelAndSpeed>())
            {
                math::Position<2, double> grappleOrigin = player->get<Position>().center();
                math::Position<2, double> anchorPoint = 
                    anchor->get<Position>().rectangle().closestPoint(grappleOrigin);

                connectGrapple(player, makePendular(grappleOrigin, anchorPoint, player->get<AccelAndSpeed>().speed));
            }
        }
    }
}


} // namespace ad
