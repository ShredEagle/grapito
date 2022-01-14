#include "SegmentStacker.h"

#include "../Entities.h"
#include "../LevelStacks.h"
#include "../Logging.h"
#include "../Timer.h"


namespace ad {
namespace grapito {


// Two different spans, otherwise when "surfing" the limit continuously add and remove the same segments.
constexpr int gCandidatesSpan = 1;
constexpr int gKeptAliveSpan = 2;

// Note Ad 2022/01/14: Well for sure we cannot use views with current MSVC in /std:c++20 mode,
// That would require the c++experimental flag, because fme right? 
// see: https://github.com/microsoft/STL/issues/1814#issuecomment-845572895
auto listConnected(SegmentStacker::SegmentIndex aIndex, int aSpan)
{

    //return std::ranges::iota_view(aIndex - gSpan, aIndex + gSpan);
    std::vector<SegmentStacker::SegmentIndex> indices;
    for(SegmentStacker::SegmentIndex id = aIndex - aSpan; id <= aIndex + aSpan; ++id)
    {
        indices.push_back(id);
    }
    return indices;
}


SegmentStacker::SegmentStacker(aunteater::EntityManager & aEntityManager, float aStartingHeight) :
    mEntityManager{aEntityManager},
    mCameras{aEntityManager},
    mStartingIndex{(SegmentIndex)std::floor(aStartingHeight / gStackHeight) + 1},
    mRandomFunctionIndex{0, gStackCandidateFunctions.size() - 1}
{
    updateAvailableSegments(mCurrentIndex);
}


void SegmentStacker::update(const GrapitoTimer, const GameInputState &)
{
    Position2 camera = getCameraPosition(mCameras);
    SegmentIndex newIndex = std::floor(camera.y() / gStackHeight);

    if (newIndex != mCurrentIndex)
    {
        updateAvailableSegments(newIndex);
        mCurrentIndex = newIndex;
    }
}


void SegmentStacker::updateAvailableSegments(SegmentIndex aIndex)
{
    for(auto segmentIt = mSegments.begin(); segmentIt != mSegments.end(); /*in body*/)
    {
        auto keptAlive = listConnected(aIndex, gKeptAliveSpan);
        if(std::find(keptAlive.begin(), keptAlive.end(), segmentIt->first) == keptAlive.end())
        {
            ADLOG(info)("Removing segment {}", segmentIt->first);
            for(aunteater::weak_entity entity : segmentIt->second)
            {
                entity->markToRemove(); 
            }
            segmentIt = mSegments.erase(segmentIt);
        }
        else
        {
            ++segmentIt;
        }
    }
    for (SegmentIndex addedId : listConnected(aIndex, gCandidatesSpan))
    {
        if (addedId >= mStartingIndex && !mSegments.contains(addedId))
        {
            ADLOG(info)("Adding segment {}", addedId);
            generateSegment(addedId);
        }
    }
}


void SegmentStacker::generateSegment(SegmentIndex aIndex)
{
    mSegments.emplace(
        aIndex,
        gStackCandidateFunctions[mRandomFunctionIndex()](mEntityManager, aIndex * gStackHeight));
}


} //namespace grapito
} // namespace ad
