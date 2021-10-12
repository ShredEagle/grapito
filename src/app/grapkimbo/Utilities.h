#pragma once

#include "Components/Position.h"
#include "commons.h"

#include <aunteater/FamilyHelp.h>

#include <math/Vector.h>

#include <optional>


namespace ad {
namespace grapito
{

//using AnchorWrap = aunteater::FamilyHelp<AnchorElement>::const_Wrap;


template <class T_archetype>
struct ClosestResult
{
    typename aunteater::FamilyHelp<T_archetype>::const_Wrap entity;
    Position2 testedPosition;
    float distance;
};


template <class T_archetype>
bool isCloser(const typename aunteater::FamilyHelp<T_archetype>::const_Wrap &,
              Position2 aCandidate,
              Position2 aBasePosition,
              float aNormSquared)
{
    return (aCandidate - aBasePosition).getNormSquared() < aNormSquared;
}


template <class T_archetype, class T_positionProvider, class T_filter = decltype(& isCloser<T_archetype>)> 
std::optional<ClosestResult<T_archetype>>
getClosest(
        const aunteater::FamilyHelp<T_archetype> & aEntities,
        const Position2 aPosition,
        T_positionProvider && aProvider,
        T_filter && aFilter = & isCloser<T_archetype>)
{
    std::optional<typename aunteater::FamilyHelp<T_archetype>::const_Wrap> closest;
    Position2 position = Position2::Zero();
    float normSquared = std::numeric_limits<float>::max();

    for (const auto entity : aEntities) 
    {
        const Position & geometry =  entity->template get<Position>();
        Position2 candidate = aProvider(geometry.rectangle());

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

} // namespace grapito
} // namespace ad
