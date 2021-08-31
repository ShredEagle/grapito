#pragma once


#include <aunteater/Component.h>

#include <math/Interpolation.h>
#include <math/Vector.h>

#include <optional>


namespace ad {


struct CameraGuide : public aunteater::Component<CameraGuide>
{
    using Interpolation_type = math::Interpolation<double, double, math::ease::SmoothStep<double>>;

    enum class OnCompletion
    {
        Remove,
        Keep,
    };

    CameraGuide(double aInfluence) :
        influence{aInfluence}
    {}

    CameraGuide(Interpolation_type aInterpolation, OnCompletion aCompletionBehaviour = OnCompletion::Keep) :
        influence{},
        influenceInterpolation{std::move(aInterpolation)},
        completionBehaviour{aCompletionBehaviour}
    {}


    double influence;
    std::optional<Interpolation_type> influenceInterpolation;
    OnCompletion completionBehaviour{OnCompletion::Keep};
};


} // namespace ad
