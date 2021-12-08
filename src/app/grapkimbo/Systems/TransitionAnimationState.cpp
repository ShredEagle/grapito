#include "TransitionAnimationState.h"

#include "../Configuration.h"
#include "../Utils/Player.h"

#include <math/Interpolation/ParameterAnimation.h>


namespace ad {
namespace grapito {


AnimatedSprite makeLoopingAnimation(StringId aAnimationId, const graphics::sprite::Animator & aAnimator)
{
    using namespace math;

    return AnimatedSprite{
        aAnimationId, 
        makeParameterAnimation<FullRange, periodic::Repeat>(aAnimator.get(aAnimationId).totalDuration,
                                                            spriteanimation::gDefaultSpeed)
    };
}

AnimationStateMachine makePlayerAnimationStateMachine(const graphics::sprite::Animator & aAnimator)
{
    AnimationStateMachine result;

    result.at(PlayerAnimation::Initial) = {
        {},
        [](const AnimatedSprite & aAnimation, const AccelAndSpeed & aAccelAndSpeed, const PlayerData & aPlayerData)
        {
            return PlayerAnimation::Idle;
        }
    };

    result.at(PlayerAnimation::Idle) = {
        makeLoopingAnimation("idle", aAnimator),
        [](const AnimatedSprite & aAnimation, const AccelAndSpeed & aAccelAndSpeed, const PlayerData & aPlayerData) 
        -> std::optional<PlayerAnimation>
        {
            if (!isIdle(aPlayerData, aAccelAndSpeed)) 
            {
                return PlayerAnimation::Run;
            }

            return std::nullopt;
        }
    };

    result.at(PlayerAnimation::Run) = {
        makeLoopingAnimation("run", aAnimator),
        [](const AnimatedSprite & aAnimation, const AccelAndSpeed & aAccelAndSpeed, const PlayerData & aPlayerData)
        -> std::optional<PlayerAnimation>
        {
            if (isIdle(aPlayerData, aAccelAndSpeed)) 
            {
                return PlayerAnimation::Idle;
            }

            return std::nullopt;
        }
    };

    return result;
}


TransitionAnimationState::TransitionAnimationState(aunteater::EntityManager & aEntityManager) :
    mAnimatedPlayers{aEntityManager},
    mSpriteAnimationFrames{aEntityManager}
{}


void TransitionAnimationState::installAnimator(graphics::sprite::Animator aAnimator)
{
    mAnimator = std::move(aAnimator);
    mPlayerAnimationStateMachine = makePlayerAnimationStateMachine(mAnimator);
}


void TransitionAnimationState::updateAnimationStates()
{
    for (auto & [accelAndSpeed, animatedSprite, playerData] : mAnimatedPlayers)
    {
        if (auto nextAnimatedSprite =
                mPlayerAnimationStateMachine.update(playerData.mCurrentAnimation, animatedSprite, accelAndSpeed, playerData))
        {
            animatedSprite = *nextAnimatedSprite;
        }
    }
}


void TransitionAnimationState::updateDrawnFrames(const GrapitoTimer aTimer)
{
    for (auto & [animatedSprite, visualSprite] : mSpriteAnimationFrames)
    {
        visualSprite.sprite = mAnimator.at(animatedSprite.animation,
                                           animatedSprite.parameter(aTimer.delta()));
    }
}


void TransitionAnimationState::update(const GrapitoTimer aTimer, const GameInputState &)
{
    updateAnimationStates();
    updateDrawnFrames(aTimer);
}


} // namespace grapito
} // namespace ad
