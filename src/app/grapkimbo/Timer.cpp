#include "Timer.h"

#include "Logging.h"


namespace ad {
namespace grapito {


void GrapitoTimer::mark(Value_t aMonotonic)
{
    float realDelta = aMonotonic - mWallTime;
    float delta = std::min(realDelta, mMaxDelta);
    ADLOG(trace)("GrapitoTimer delta: {:.2f}ms(wall: {:.2f}, max: {:.2f}).",
                 delta*1000, realDelta*1000, mMaxDelta*1000);
    mWallTime = aMonotonic;
    Base_type::mark(simulationTime() + delta); 
}


} // namespace grapito
} // namespace ad
