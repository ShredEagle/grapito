#pragma once


#include "VisualSprite.h"

#include "../commons.h"

#include <aunteater/Component.h>

#include <functional>


namespace ad {
namespace grapito {


struct AnimatedSprite : public aunteater::Component<AnimatedSprite>
{
    AnimatedSprite() = default;

    template <class T_parameterAnimation>
    AnimatedSprite(StringId aAnimation, T_parameterAnimation aParameterAnimation, bool aHorizontalMirroring = false) :
        animation{aAnimation},
        parameter{[paramAnimation = std::move(aParameterAnimation)](float aDelta) mutable
            {
                return paramAnimation.advance(aDelta);
            }},
        horizontalMirroring{aHorizontalMirroring}
    {}

    StringId animation = StringId::Null();  
    // This provides type erasure for the parameter animation
    std::function<float(float)> parameter;
    bool horizontalMirroring{false};
};


} // namespace grapito
} // namespace ad 
