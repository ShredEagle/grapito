#pragma once

// TODO move to cpp
#include "Logging.h"

#include <aunteater/Timer.h>


namespace ad {
namespace grapito {


class GrapitoTimer : public aunteater::Timer_base<float>
{
    using Base_type = aunteater::Timer_base<float>;

public:
    GrapitoTimer(float aInitialTime, float aMaxDelta) :
        Base_type{aInitialTime},
        mWallTime{aInitialTime},
        mMaxDelta{aMaxDelta}
    {}

    void mark(Value_t aMonotonic)
    {
        float delta = std::min(aMonotonic - mWallTime, mMaxDelta);
        ADLOG(trace)("GrapitoTimer delta: {}(max: {}).", delta, mMaxDelta);
        mWallTime = aMonotonic;
        Base_type::mark(simulationTime() + delta); 
    }

private:
    float mWallTime;
    float mMaxDelta;
};


} // namespace grapito
} // namespace ad
