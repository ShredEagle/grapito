#pragma once


#include "../commons.h"

#include <aunteater/Component.h>

#include <math/Interpolation/Interpolation.h>
#include <math/Vector.h>

#include <optional>


namespace ad {
namespace grapito {


struct CameraGuide : public aunteater::Component<CameraGuide>
{
    using Interpolation_type = math::Interpolation<float, float, math::None, math::ease::SmoothStep>;

    enum class OnCompletion
    {
        RemoveComponent,
        RemoveEntity,
        Keep,
    };

    CameraGuide(float aInfluence, Vec2 aOffset = {0.f, 0.f}) :
        influence{aInfluence},
        offset{aOffset}
    {}

    CameraGuide(Interpolation_type aInterpolation, OnCompletion aCompletionBehaviour = OnCompletion::Keep) :
        influence{},
        influenceInterpolation{std::move(aInterpolation)},
        completionBehaviour{aCompletionBehaviour}
    {}


    float influence;
    Vec2 offset{0.f, 0.f}; // to offset the guide position relative to the entity position
    std::optional<Interpolation_type> influenceInterpolation;
    OnCompletion completionBehaviour{OnCompletion::Keep};
};


} // namespace grapito
} // namespace ad
