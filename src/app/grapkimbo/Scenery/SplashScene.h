#pragma once


#include "StateMachine.h"

#include <arte/Image.h>

#include <graphics/Spriting.h>

#include <vector>


namespace ad {
namespace grapito {

struct Splash
{
    template <class T_value>
    using Evaluator = std::function<T_value(GrapitoTimer::Value_t)>;

    arte::ImageRgba mImage;
    typename GrapitoTimer::Value_t mDuration; 
    Evaluator<math::hdr::Rgb> mBufferClearColor = [](auto){return math::hdr::gBlack;};
    Evaluator<GLfloat> mSpriteOpacity = [](auto){return 1.f;};
};


class SplashScene : public State 
{
public:
    SplashScene(math::Size<2, int> aResolution);

    UpdateStatus update(
        GrapitoTimer & aTimer,
        const GameInputState & aInputs,
        StateMachine & aStateMachine) override;

    std::vector<Splash> splashes;

    void onFramebufferResize(math::Size<2, int> aResolution);

private:
    void reset();
    Splash & current();

    math::Size<2, int> mScreenSize_w;
    math::Position<2, int> mSpritePlacement_w{0, 0};

    std::vector<Splash>::size_type mCurrentSplashId{0};
    GrapitoTimer::Value_t mTimeAccumlated{0};
    graphics::Spriting mSpriting;
    std::optional<graphics::LoadedSprite> mLoadedSprite;
};


} // namespace grapito
} // namespace ad
