#pragma once


#include "../commons.h"
#include "../Input.h"

#include <memory>
#include <optional>
#include <vector>

#include <cassert>


namespace ad {
namespace grapito {


// Forward
class StateMachine;


enum class UpdateStatus
{
    KeepFrame,
    SwapBuffers,
};


enum class TransitionProgress
{
    None, // The transition phase does not exist. Should not be returned if any work took place!
    Ongoing,
    Complete,
};


inline constexpr bool isTransitionOver(TransitionProgress aProgress)
{
    return aProgress == TransitionProgress::Complete || aProgress == TransitionProgress::None;
}


inline constexpr bool isTransitionPresent(TransitionProgress aProgress)
{
    return aProgress != TransitionProgress::None;
}


class State
{
public:
    State() = default;

    // Disable copying and moving.
    State(const State &) = delete;
    State & operator=(const State &) = delete;

    virtual ~State() = default;

    /// \return True if  the frame buffer should be swapped.
    /// \note Leaves the door open to timer manipulation (e.g. debug stepping).
    virtual UpdateStatus update(
        const GrapitoTimer & aTimer,
        const GameInputState & aInputs,
        StateMachine & aStateMachine) = 0;

    // Note: The default implementions for enter() and exit() do nothing.
    // They simply return that the transition is not present, the update status is disregarded
    // Note: StateMachine argument is made a const reference, to intentionally prevent
    // transition from altering the sequence of states.
    virtual std::pair<TransitionProgress, UpdateStatus> enter(
        const GrapitoTimer &,
        const GameInputState &,
        const StateMachine &)
    { return {TransitionProgress::None, UpdateStatus::KeepFrame}; }

    virtual std::pair<TransitionProgress, UpdateStatus> exit(
        const GrapitoTimer &,
        const GameInputState &,
        const StateMachine &)
    { return {TransitionProgress::None, UpdateStatus::KeepFrame}; }

    // Provided to allow setup of the transition phases
    // TODO Ad 2022/02/01: could actually make use of the timer, for example to mark "phase enter time".
    virtual void beforeEnter()
    {}

    virtual void beforeExit()
    {}
};


/// \brief A dummy state added as the first in a state machine constructed without an explicit initial state.
/// It just allows to make a normal transition to the next state.
class EmptyState : public State
{
    UpdateStatus update(const GrapitoTimer &, const GameInputState &, StateMachine &) override
    {
        return UpdateStatus::KeepFrame;
    }
};


using State_ptr = std::shared_ptr<State>;

// TODO Ad 2012/12/23: This StateMachine class is sometimes used as a generic state machine (e.g. GameRule system)
// Yet the UpdateStatus return type is somewhat coupled to the top-level flow state machine.
class StateMachine
{
public:
    StateMachine(std::shared_ptr<State> aInitialState);
    StateMachine();

    UpdateStatus update(const GrapitoTimer & aTimer, const GameInputState & aInputs);

    State & topState();

    StateMachine & pushState(State_ptr aState);
    /// \brief Add the provided state *under* the current state.
    StateMachine & putNext(State_ptr aState);
    State_ptr popState();

    template <class T_state, class... VT_aArgs>
    StateMachine & emplaceState(VT_aArgs &&... vaArgs);

private:
    void enterNextState();

    enum class Phase
    {
        Entering,
        Updating,
        Exiting,
    };

    std::vector<State_ptr> mStates;
    // Note: Make a copy of the State shared_ptr. This is serves two purposes:
    // * ensures that a state popping itself will not make dangling access before returning.
    // * allow to implement enter() / exit() state member functions, while states can still pop themselves.
    std::pair<State_ptr, Phase> mActiveState;

    // Make a copy of the State shared_ptr when it is discarded (exit completed),
    // so it survives until next call to update(). 
    // This allows the state to be valid during next buffer swap, which is important if the state contained OpenGL objects.
    State_ptr mDiscardedState;
};


//
// Implementations
//

inline StateMachine::StateMachine(std::shared_ptr<State> aInitialState) :
    mStates{std::move(aInitialState)}
{
    enterNextState();
    mActiveState.first->beforeEnter();
}


inline StateMachine::StateMachine() :
    StateMachine{std::make_shared<EmptyState>()}
{
    // Otherwise, the assertion that the active state cannot be "Entering" 
    // when it is not at the top will not hold true.
    mActiveState.second = Phase::Updating;
}

inline void StateMachine::enterNextState()
{
    mActiveState = std::make_pair(mStates.back(), Phase::Entering);
}


inline UpdateStatus StateMachine::update(const GrapitoTimer & aTimer, const GameInputState & aInputs)
{
    // If the active state is not the top of the stack anymore, the active state is being exited.
    // TODO If the same shared_ptr is added on top of itself, the transtion is not taking place!
    if (mActiveState.first.get() != mStates.back().get())
    {
        // Only "Updating" should be able to move to "Exiting".
        assert(mActiveState.second != Phase::Entering);

        // If we just entered the "Exiting" phase, call beforeExit()
        if (mActiveState.second != Phase::Exiting)
        {
            mActiveState.first->beforeExit();
            mActiveState.second = Phase::Exiting;
        }

        auto [progress, updateStatus] = mActiveState.first->exit(aTimer, aInputs, *this);
        if (isTransitionOver(progress))
        {
            mDiscardedState = mActiveState.first; // prolongates lifetime at least until next call to update();
            enterNextState();
            mActiveState.first->beforeEnter();
        }
        if (isTransitionPresent(progress)) return updateStatus;
        // Fallthrough if the transition was not present
    }

    assert(mActiveState.first.get() == mStates.back().get());
    switch(mActiveState.second)
    {
        case Phase::Entering:
        {
            // Note: this might cause issue if the exited state returned TransitionProgress::None but took actions
            // requiring it to be available until buffer swap.
            mDiscardedState = nullptr; // The discarded state is cleared before entering next.
            auto [progress, updateStatus] = mActiveState.first->enter(aTimer, aInputs, *this);
            if (isTransitionOver(progress)) mActiveState.second = Phase::Updating;
            if (isTransitionPresent(progress)) return updateStatus;
            [[fallthrough]];
        }
        // Fallthrough if the transition was not present
        case Phase::Updating:
            return mActiveState.first->update(aTimer, aInputs, *this);
        default:
            throw std::logic_error{"Inconstitent machine state."};
    }
}


inline State & StateMachine::topState()
{
    return *(mStates.back());
}


inline StateMachine & StateMachine::pushState(State_ptr aState)
{
    mStates.push_back(std::move(aState));
    return *this;
}

inline StateMachine & StateMachine::putNext(State_ptr aState)
{
    mStates.insert(mStates.end() - 1, aState);
    return *this;
}


inline State_ptr StateMachine::popState()
{
    State_ptr popped = std::move(mStates.back());
    mStates.pop_back();
    return popped;
}


template <class T_state, class... VT_aArgs>
StateMachine & StateMachine::emplaceState(VT_aArgs &&... vaArgs)
{
    mStates.push_back(
        std::make_shared<T_state>(std::forward<VT_aArgs>(vaArgs)...));
    return *this;
}


} // namespace grapito
} // namespace ad
