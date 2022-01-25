#pragma once

#include "Input.h"

#include "../Animation/AnimationStateMachine.h"

#include "../Components/AccelAndSpeed.h"
#include "../Components/AnimatedSprite.h"
#include "../Components/PlayerData.h"
#include "../Components/VisualSprite.h"

#include <aunteater/EntityManager.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>

#include <graphics/SpriteAnimator.h>


namespace ad {
namespace grapito {


using AnimatedPlayer = aunteater::Archetype<AccelAndSpeed, AnimatedSprite, PlayerData>;
using SpriteAnimationFrame = aunteater::Archetype<AnimatedSprite, VisualSprite>;


class TransitionAnimationState : public aunteater::System<GrapitoTimer, GameInputState>
{
public:
    TransitionAnimationState(aunteater::EntityManager & aEntityManager);

    void installAnimator(graphics::sprite::Animator aAnimator);

    void update(const GrapitoTimer aTimer, const GameInputState & aInput) override;

private:
    void updateAnimationStates();
    void updateDrawnFrames(const GrapitoTimer aTimer);


    const aunteater::FamilyHelp<AnimatedPlayer> mAnimatedPlayers;
    const aunteater::FamilyHelp<SpriteAnimationFrame> mSpriteAnimationFrames;

    graphics::sprite::Animator mAnimator;
    AnimationStateMachine mPlayerAnimationStateMachine;
};


} // namespace grapito
} // namespace ad
