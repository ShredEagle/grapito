#pragma once

#include "../Input.h"

#include <aunteater/Component.h>

namespace ad {
namespace grapito
{

struct DelayDeletion : public aunteater::Component<DelayDeletion>
{
    explicit DelayDeletion(int aFrameBeforeDelete) :
        mFrameBeforeDelete{aFrameBeforeDelete}
    {}

    int mFrameBeforeDelete;
};


} // namespace grapito
} // namespace ad
