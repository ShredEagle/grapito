#pragma once


#include <aunteater/Component.h>

#include <math/Interpolation/Interpolation.h>
#include <math/Vector.h>

#include <optional>


namespace ad {


struct CameraGuide : public aunteater::Component<CameraGuide>
{
    using Interpolation_type = math::Interpolation<float, float, math::None, math::ease::SmoothStep>;

    enum class OnCompletion
    {
        Remove,
        Keep,
    };

    CameraGuide(float aInfluence) :
        influence{aInfluence}
    {}

    CameraGuide(Interpolation_type aInterpolation, OnCompletion aCompletionBehaviour = OnCompletion::Keep) :
        influence{},
        influenceInterpolation{std::move(aInterpolation)},
        completionBehaviour{aCompletionBehaviour}
    {}


    float influence;
    std::optional<Interpolation_type> influenceInterpolation;
    OnCompletion completionBehaviour{OnCompletion::Keep};
};


} // namespace ad
