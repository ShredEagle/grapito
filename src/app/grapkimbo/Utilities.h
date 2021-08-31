#pragma once


#include "Components/Position.h"

#include <aunteater/FamilyHelp.h>

#include <math/Vector.h>

#include <optional>


namespace ad {

//using AnchorWrap = aunteater::FamilyHelp<AnchorElement>::const_Wrap;


template <class T_archetype>
struct ClosestResult
{
    typename aunteater::FamilyHelp<T_archetype>::const_Wrap entity;
    math::Position<2, double> testedPosition;
    double distance;
};


template <class T_archetype>
bool isCloser(const typename aunteater::FamilyHelp<T_archetype>::const_Wrap &,
              math::Position<2, double> aCandidate,
              math::Position<2, double> aBasePosition,
              double aNormSquared)
{
    return (aCandidate - aBasePosition).getNormSquared() < aNormSquared;
}


template <class T_archetype, class T_positionProvider, class T_filter = decltype(& isCloser<T_archetype>)> 
std::optional<ClosestResult<T_archetype>>
getClosest(
        const aunteater::FamilyHelp<T_archetype> & aEntities,
        const math::Position<2, double> aPosition,
        T_positionProvider && aProvider,
        T_filter && aFilter = & isCloser<T_archetype>)
{
    std::optional<typename aunteater::FamilyHelp<T_archetype>::const_Wrap> closest;
    math::Position<2, double> position = math::Position<2, double>::Zero();
    double normSquared = std::numeric_limits<double>::max();

    for (const auto entity : aEntities) 
    {
        const Position & geometry =  entity->template get<Position>();
        math::Position<2, double> candidate = aProvider(geometry.rectangle());

        if (aFilter(entity, candidate, aPosition, normSquared))
        {
            normSquared = (candidate - aPosition).getNormSquared();
            position = candidate;
            closest = entity;
        }
    }

    if (closest)
    {
        return {{*closest, position, std::sqrt(normSquared)}};
    }
    else
    {
        return {};
    }
}

} // namespace ad
