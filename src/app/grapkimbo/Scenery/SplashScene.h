#pragma once


#include "StateMachine.h"

#include "../Timer.h"

#include <arte/Image.h>

#include <graphics/SpriteLoading.h>
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


// TODO Ad 2022/02/14: It should be possible to overlay
// several images (e.g. showing the studio name after the logo).
// This would require to either allow composite steps (Splash), or
// to remove the notion of steps 
// (whith each image starting indepently on the global scene timeline).
class SplashScene : public State 
{
public:
    SplashScene(math::Size<2, int> aResolution);

    UpdateStatus update(
        const GrapitoTimer & aTimer,
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
    GrapitoTimer::Value_t mTimeAccumulated{0};
    graphics::Spriting mSpriting;
    graphics::sprite::LoadedAtlas mAtlas;
    std::optional<graphics::LoadedSprite> mLoadedSprite;
};


} // namespace grapito
} // namespace ad
