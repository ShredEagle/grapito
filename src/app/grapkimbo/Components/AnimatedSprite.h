#pragma once


#include <aunteater/Component.h>

#include <functional>


namespace ad {

//TODO
using StringId = std::string;


struct AnimatedSprite : public aunteater::Component<AnimatedSprite>
{
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


} // namespace ad
