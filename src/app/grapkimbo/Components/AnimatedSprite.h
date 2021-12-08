#pragma once


#include "../commons.h"

#include <aunteater/Component.h>

#include <functional>


namespace ad {
namespace grapito {


struct AnimatedSprite : public aunteater::Component<AnimatedSprite>
{
    AnimatedSprite() = default;

    template <class T_parameterAnimation>
    AnimatedSprite(StringId aAnimation, T_parameterAnimation aParameterAnimation) :
        animation{aAnimation},
        parameter{[paramAnimation = std::move(aParameterAnimation)](GLfloat aDelta) mutable
            {
                return paramAnimation.advance(aDelta);
            }}
    {}

    StringId animation;  
    // This provides type erasure for the parameter animation
    std::function<float(float)> parameter;
};


} // namespace grapito
} // namespace ad 
