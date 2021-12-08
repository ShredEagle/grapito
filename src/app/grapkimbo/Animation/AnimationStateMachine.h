#pragma once


#include "PlayerAnimation.h"

#include "../Components/AccelAndSpeed.h"
#include "../Components/AnimatedSprite.h"
#include "../Components/PlayerData.h"

#include <array>
#include <functional>
#include <optional>


namespace ad {
namespace grapito {


struct PlayerData; 


/// \brief Model a logical animation state.
/// Such states is associated with an AnimatedSprite component, and has logic to transition toward other states.
struct AnimationState
{
    // TODO might probably be free functions' pointers
    AnimatedSprite mAnimatedSprite;
    std::function<std::optional<PlayerAnimation>(
        const AnimatedSprite & aAnimation,
        const AccelAndSpeed & aAccelAndSpeed,
        const PlayerData & aPlayerData)> mUpdate;
};


// Note: I am not sure if it is a good idea to keep separate state machine for the logic player state vs the player animation state.
// There is obviously a lot of coupling between the two (usually, changes in logic state will trigger changes in anim state).
// Yet, this could also allow to cleanly encapsulate what only affects animations :
// (e.g. a DAG of animation transitions, different "next animation" depending on the time into current animation, ...).
/// \brief Implement a state machine for animation states transitions.
class AnimationStateMachine
{
public:
    std::optional<AnimatedSprite> update(PlayerAnimation & aState, 
                                         const AnimatedSprite & aAnimation,
                                         const AccelAndSpeed & aAccelAndSpeed,
                                         const PlayerData & aPlayerData);

    AnimationState & at(PlayerAnimation aState);

private:
    std::array<AnimationState, static_cast<std::size_t>(PlayerAnimation::_Count)> mStates;
};


} // namespace grapito
} // namespace ad 