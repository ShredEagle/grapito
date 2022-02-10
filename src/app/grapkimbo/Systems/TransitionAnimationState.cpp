#include "TransitionAnimationState.h"

#include "../Configuration.h"
#include "../Timer.h"

#include "../Utils/Player.h"

#include <handy/StringId_Interning.h>

#include <math/Interpolation/ParameterAnimation.h>


namespace ad {
namespace grapito {


const StringId idleSid = handy::internalizeString("idle");
const StringId runSid = handy::internalizeString("run");
const StringId jumpSid = handy::internalizeString("jump");
const StringId walledSid = handy::internalizeString("walled");


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

    // Note Ad 2022/01/04: Now, AnimatedSprite can be mutated by the AnimationState::update(), in order to control the animation
    // playback speed.

    AnimationStateMachine result;

    // The initial state allows to actually assign a non-default AnimatedSprite, thanks to the initial unconditionnal transition.
    // (Otherwise, if the player was already in an Idle state, the AnimatedSprite would not be populated accordingly.)
    result.at(PlayerAnimation::Initial) = {
        {},
        [](AnimatedSprite & aAnimation, const AccelAndSpeed & aAccelAndSpeed, const PlayerData & aPlayerData)
        {
            return PlayerAnimation::IdleRight;
        }
    };


    result.at(PlayerAnimation::IdleLeft) = {
        makeLoopingAnimation(idleSid, aAnimator, Mirroring::Horizontal),
        [](AnimatedSprite & aAnimation, const AccelAndSpeed & aAccelAndSpeed, const PlayerData & aPlayerData) 
        -> std::optional<PlayerAnimation>
        {
            //if (!isIdle(aPlayerData, aAccelAndSpeed)) 
            //{
            if (isJumping(aPlayerData))
            {
                return PlayerAnimation::JumpLeft;
            }
            else if (isGoingRight(aAccelAndSpeed))
            {
                return PlayerAnimation::RunRight;
            }
            else if (isGoingLeft(aAccelAndSpeed))
            {
                return PlayerAnimation::RunLeft;
            }

            return std::nullopt;
        }
    };

    result.at(PlayerAnimation::IdleRight) = {
        makeLoopingAnimation(idleSid, aAnimator, Mirroring::None),
        [](AnimatedSprite & aAnimation, const AccelAndSpeed & aAccelAndSpeed, const PlayerData & aPlayerData) 
        -> std::optional<PlayerAnimation>
        {
            //if (!isIdle(aPlayerData, aAccelAndSpeed)) 
            //{
            if (isJumping(aPlayerData))
            {
                return PlayerAnimation::JumpRight;
            }
            else if (isGoingRight(aAccelAndSpeed))
            {
                return PlayerAnimation::RunRight;
            }
            else if (isGoingLeft(aAccelAndSpeed))
            {
                return PlayerAnimation::RunLeft;
            }

            return std::nullopt;
        }
    };

    result.at(PlayerAnimation::RunLeft) = {
        makeLoopingAnimation(runSid, aAnimator, Mirroring::Horizontal),
        [](AnimatedSprite & aAnimation, const AccelAndSpeed & aAccelAndSpeed, const PlayerData & aPlayerData)
        -> std::optional<PlayerAnimation>
        {
            if (isIdle(aPlayerData, aAccelAndSpeed)) 
            {
                return PlayerAnimation::IdleLeft;
            }
            else if (isJumping(aPlayerData))
            {
                return PlayerAnimation::JumpLeft;
            }
            else if (isGoingRight(aAccelAndSpeed))
            {
                return PlayerAnimation::RunRight;
            }

            aAnimation.parameterAdvanceSpeed = std::abs(aAccelAndSpeed.speed.x()) / player::gGroundSpeed;
            return std::nullopt;
        }
    };

    result.at(PlayerAnimation::RunRight) = {
        makeLoopingAnimation(runSid, aAnimator, Mirroring::None),
        [](AnimatedSprite & aAnimation, const AccelAndSpeed & aAccelAndSpeed, const PlayerData & aPlayerData)
        -> std::optional<PlayerAnimation>
        {
            if (isIdle(aPlayerData, aAccelAndSpeed)) 
            {
                return PlayerAnimation::IdleRight;
            }
            else if (isJumping(aPlayerData))
            {
                return PlayerAnimation::JumpRight;
            }
            else if (isGoingLeft(aAccelAndSpeed))
            {
                return PlayerAnimation::RunLeft;
            }

            aAnimation.parameterAdvanceSpeed = std::abs(aAccelAndSpeed.speed.x()) / player::gGroundSpeed;
            return std::nullopt;
        }
    };

    result.at(PlayerAnimation::JumpLeft) = {
        makeLoopingAnimation(jumpSid, aAnimator, Mirroring::Horizontal),
        [](AnimatedSprite & aAnimation, const AccelAndSpeed & aAccelAndSpeed, const PlayerData & aPlayerData) 
        -> std::optional<PlayerAnimation>
        {
            if (isGrounded(aPlayerData)) 
            {
                if (isGoingLeft(aAccelAndSpeed))
                {
                    return PlayerAnimation::RunLeft;
                }
                else
                {
                    return PlayerAnimation::IdleLeft;
                }
            }
            else if(isGoingRight(aAccelAndSpeed))
            {
                return PlayerAnimation::JumpRight;
            }
            else if(isWalledLeft(aPlayerData))
            {
                return PlayerAnimation::WalledLeft;
            }
            else if(isWalledRight(aPlayerData))
            {
                return PlayerAnimation::WalledRight;
            }

            return std::nullopt;
        }
    };

    result.at(PlayerAnimation::JumpRight) = {
        makeLoopingAnimation(jumpSid, aAnimator, Mirroring::None),
        [](AnimatedSprite & aAnimation, const AccelAndSpeed & aAccelAndSpeed, const PlayerData & aPlayerData) 
        -> std::optional<PlayerAnimation>
        {
            if (isGrounded(aPlayerData)) 
            {
                if (isGoingRight(aAccelAndSpeed))
                {
                    return PlayerAnimation::RunRight;
                }
                else
                {
                    return PlayerAnimation::IdleRight;
                }
            }
            else if(isGoingLeft(aAccelAndSpeed))
            {
                return PlayerAnimation::JumpLeft;
            }
            else if(isWalledLeft(aPlayerData))
            {
                return PlayerAnimation::WalledLeft;
            }
            else if(isWalledRight(aPlayerData))
            {
                return PlayerAnimation::WalledRight;
            }

            return std::nullopt;
        }
    };

    result.at(PlayerAnimation::WalledLeft) = {
        makeLoopingAnimation(walledSid, aAnimator, Mirroring::Horizontal),
        [](AnimatedSprite & aAnimation, const AccelAndSpeed & aAccelAndSpeed, const PlayerData & aPlayerData) 
        -> std::optional<PlayerAnimation>
        {
            if (isGrounded(aPlayerData)) 
            {
                return PlayerAnimation::IdleLeft;
            }
            else if (!isWalledLeft(aPlayerData))
            {
                return PlayerAnimation::JumpLeft;
            }

            return std::nullopt;
        }
    };

    result.at(PlayerAnimation::WalledRight) = {
        makeLoopingAnimation(walledSid, aAnimator, Mirroring::None),
        [](AnimatedSprite & aAnimation, const AccelAndSpeed & aAccelAndSpeed, const PlayerData & aPlayerData) 
        -> std::optional<PlayerAnimation>
        {
            if (isGrounded(aPlayerData)) 
            {
                return PlayerAnimation::IdleRight;
            }
            else if (!isWalledRight(aPlayerData))
            {
                return PlayerAnimation::JumpRight;
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
                                           animatedSprite.advance(aTimer.delta()));
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
