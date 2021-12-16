#include "AnimationStateMachine.h"


namespace ad {
namespace grapito {


std::optional<AnimatedSprite> AnimationStateMachine::update(PlayerAnimation & aState,
                                                            const AnimatedSprite & aAnimation,
                                                            const AccelAndSpeed & aAccelAndSpeed,
                                                            const PlayerData & aPlayerData)
{
    if(auto nextState = mStates[static_cast<std::size_t>(aState)].mUpdate(aAnimation, aAccelAndSpeed, aPlayerData))
    {
        aState = *nextState;
        return mStates[static_cast<std::size_t>(aState)].mAnimatedSprite;
    }
    return std::nullopt;
}


AnimationState & AnimationStateMachine::at(PlayerAnimation aState)
{
    return mStates.at(static_cast<std::size_t>(aState));
}


} // namespace grapito
} // namespace ad 
