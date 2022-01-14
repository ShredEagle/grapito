#pragma once


#include "CameraGuidedControl.h" // for Camera archetype

#include "../Input.h"
#include "../Configuration.h"

#include "../Components/CameraGuide.h"
#include "../Components/Position.h"

#include <aunteater/EntityManager.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>

#include <math/Vector.h>

#include <functional>
#include <map>


namespace ad {
namespace grapito {


class SegmentStacker : public aunteater::System<GrapitoTimer, GameInputState>
{
public:
    using SegmentIndex = int;

    SegmentStacker(aunteater::EntityManager & aEntityManager, float aStartingHeight);

    void update(const GrapitoTimer, const GameInputState &) override;

private:
    void updateAvailableSegments(SegmentIndex aIndex);

    void pruneSegment(SegmentIndex aIndex);
    void generateSegment(SegmentIndex aIndex);

    aunteater::EntityManager & mEntityManager; 
    const aunteater::FamilyHelp<Camera> mCameras;

    SegmentIndex mCurrentIndex{0};
    SegmentIndex mStartingIndex; // The index at which generation should start.
    std::map<SegmentIndex, std::vector<aunteater::weak_entity>> mSegments;
};

} // namespace grapito
} // namespace ad
