#include "DelayDeleter.h"
#include "../commons.h"

namespace ad {
namespace grapito
{

DelayDeleter::DelayDeleter(aunteater::EntityManager & aEntityManager) :
    mDeletable(aEntityManager)
{}

void DelayDeleter::update(const GrapitoTimer, const GameInputState &)
{
    // For the accel solver we will use a derive methode
    // from velocity verlet. It assumes a constant acceleration
    // for total precision but error with changing acceleration
    // are really small.
    // see: https://www.youtube.com/watch?v=hG9SzQxaCm8
    for(auto deletable : mDeletable)
    {
        DelayDeletion & delayDeletion = deletable->get<DelayDeletion>();
        if (delayDeletion.mFrameBeforeDelete-- == 0)
        {
            deletable->markToRemove();
        }
    }
}

} // namespace grapito
} // namespace ad
