#include "RenderToScreen.h"

#include "../Timer.h"

#include "../Scenery/GameScene.h"


namespace ad {
namespace grapito {


RenderToScreen::RenderToScreen(aunteater::EntityManager & aEntityManager,
                               std::shared_ptr<graphics::AppInterface> aAppInterface,
                               const GameScene & aGameScene) :
    mEntityManager{aEntityManager},
    mAppInterface{aAppInterface},
    mRenderedScene{aGameScene},
    mEffects{mAppInterface}
{}


void RenderToScreen::update(const GrapitoTimer, const GameInputState &)
{
    if(mFadeToColor)
    {
        mEffects.fadeTo(mRenderedScene,         
                        graphics::FrameBuffer::Default(),
                        *mFadeToColor);
    }
    else
    {
        mRenderedScene.render();
    }
}


} // namespace grapito
} // namespace ad
