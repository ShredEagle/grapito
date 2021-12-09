#include "TransitionAnimationState.h"

#include "../Configuration.h"
#include "../Utils/Player.h"

#include <math/Interpolation/ParameterAnimation.h>


namespace ad {
namespace grapito {


enum class Mirroring
{
    None,
    Horizontal
};

AnimatedSprite makeLoopingAnimation(StringId aAnimationId, const graphics::sprite::Animator & aAnimator, Mirroring aMirroring)
{
    using namespace math;

    return AnimatedSprite{
        aAnimationId, 
        makeParameterAnimation<FullRange, periodic::Repeat>(aAnimator.get(aAnimationId).totalDuration,
                                                            spriteanimation::gDefaultSpeed),
        aMirroring == Mirroring::Horizontal
    };
}

AnimationStateMachine makePlayerAnimationStateMachine(const graphics::sprite::Animator & aAnimator)
{
    // TODO Ad 21/12/09: Avoid duplicates for mirrored animation states?
    //  Currently, each animation state is duplicated between Left and Right versions (mirroring).
    // Initially, that is done because the mirroring data is kept in AnimatedSprite 
    // (and copied to VisualSprite, which is another problem because of duplication), and I preferred not to allow
    // the AnimationState::update() to mutate the AnimatedSprite, meaning separate AnimationState for each orientation.
    // --
    // On the other hand, if the orientation info was present in some logic-state component (e.g. PlayerData for characters),
    // it could be copied from there directly into the VisualSprite, without being kept in AnimatedSprite.

    AnimationStateMachine result;

    // The initial state allows to actually assign a non-default AnimatedSprite, thanks to the initial unconditionnal transition.
    // (Otherwise, if the player was already in an Idle state, the AnimatedSprite would not be populated accordingly.)
    result.at(PlayerAnimation::Initial) = {
        {},
        [](const AnimatedSprite & aAnimation, const AccelAndSpeed & aAccelAndSpeed, const PlayerData & aPlayerData)
        {
            return PlayerAnimation::IdleRight;
        }
    };


    result.at(PlayerAnimation::IdleLeft) = {
        makeLoopingAnimation("idle", aAnimator, Mirroring::Horizontal),
        [](const AnimatedSprite & aAnimation, const AccelAndSpeed & aAccelAndSpeed, const PlayerData & aPlayerData) 
        -> std::optional<PlayerAnimation>
        {
            if (!isIdle(aPlayerData, aAccelAndSpeed)) 
            {
                if (isGoingRight(aAccelAndSpeed))
                {
                    return PlayerAnimation::RunRight;
                }
                else
                {
                    return PlayerAnimation::RunLeft;
                }
            }

            return std::nullopt;
        }
    };

    result.at(PlayerAnimation::IdleRight) = {
        makeLoopingAnimation("idle", aAnimator, Mirroring::None),
        [](const AnimatedSprite & aAnimation, const AccelAndSpeed & aAccelAndSpeed, const PlayerData & aPlayerData) 
        -> std::optional<PlayerAnimation>
        {
            if (!isIdle(aPlayerData, aAccelAndSpeed)) 
            {
                if (isGoingLeft(aAccelAndSpeed))
                {
                    return PlayerAnimation::RunLeft;
                }
                else
                {
                    return PlayerAnimation::RunRight;
                }
            }

            return std::nullopt;
        }
    };

    result.at(PlayerAnimation::RunLeft) = {
        makeLoopingAnimation("run", aAnimator, Mirroring::Horizontal),
        [](const AnimatedSprite & aAnimation, const AccelAndSpeed & aAccelAndSpeed, const PlayerData & aPlayerData)
        -> std::optional<PlayerAnimation>
        {
            if (isIdle(aPlayerData, aAccelAndSpeed)) 
            {
                return PlayerAnimation::IdleLeft;
            }
            else if (isGoingRight(aAccelAndSpeed))
            {
                return PlayerAnimation::RunRight;
            }

            return std::nullopt;
        }
    };

    result.at(PlayerAnimation::RunRight) = {
        makeLoopingAnimation("run", aAnimator, Mirroring::None),
        [](const AnimatedSprite & aAnimation, const AccelAndSpeed & aAccelAndSpeed, const PlayerData & aPlayerData)
        -> std::optional<PlayerAnimation>
        {
            if (isIdle(aPlayerData, aAccelAndSpeed)) 
            {
                return PlayerAnimation::IdleRight;
            }
            else if (isGoingLeft(aAccelAndSpeed))
            {
                return PlayerAnimation::RunLeft;
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
        if (animatedSprite.horizontalMirroring)
        {
            visualSprite.mirroring.x() = -1;
        }
        else
        {
            visualSprite.mirroring.x() = +1;
        }
    }
}


void TransitionAnimationState::update(const GrapitoTimer aTimer, const GameInputState &)
{
    updateAnimationStates();
    updateDrawnFrames(aTimer);
}


} // namespace grapito
} // namespace ad
