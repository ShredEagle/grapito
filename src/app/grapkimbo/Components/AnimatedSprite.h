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
    AnimatedSprite(StringId aAnimation, T_parameterAnimation aParameterAnimation, bool aHorizontalFlipping = false) :
        animation{aAnimation},
        horizontalFlipping{aHorizontalFlipping},
        parameter{[paramAnimation = std::move(aParameterAnimation)](float aDelta) mutable
            {
                return paramAnimation.advance(aDelta);
            }}
    {}

    float advance(float aDelta)
    {
        return parameter(aDelta * parameterAdvanceSpeed);
    }

    StringId animation = StringId::Null();  
    float parameterAdvanceSpeed{1.f};
    bool horizontalFlipping{false};
private:
    // This provides type erasure for the parameter animation
    std::function<float(float)> parameter;
};


} // namespace grapito
} // namespace ad 
