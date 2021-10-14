#pragma once


#include "../commons.h"
#include "../Input.h"

#include <memory>
#include <optional>
#include <vector>


namespace ad {
namespace grapito {


// Forward
class StateMachine;


enum class UpdateStatus
{
    KeepFrame,
    SwapBuffers,
};


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
    virtual /*std::optional<shared_ptr<State>>*/ UpdateStatus update(
        GrapitoTimer & aTimer,
        const GameInputState & aInputs,
        StateMachine & aStateMachine) = 0;
};


using State_ptr = std::shared_ptr<State>;

class StateMachine
{
public:
    StateMachine(State_ptr aInitialState);

    UpdateStatus update(GrapitoTimer & aTimer, const GameInputState & aInputs);

    State & topState();

    StateMachine & pushState(State_ptr aState);
    State_ptr popState();

    template <class T_state, class... VT_aArgs>
    StateMachine & emplaceState(VT_aArgs &&... vaArgs);

private:
    std::vector<State_ptr> mStates;
};


//
// Implementations
//
inline StateMachine::StateMachine(std::shared_ptr<State> aInitialState) :
    mStates{std::move(aInitialState)}
{}


inline UpdateStatus StateMachine::update(GrapitoTimer & aTimer, const GameInputState & aInputs)
{
    // Make a copy of the shared_ptr. This is wastefull most of the time,
    // yet ensures that a state popping itself will not access its freed memory. 
    State_ptr state{mStates.back()};
    return state->update(aTimer, aInputs, *this);
}


inline State & StateMachine::topState()
{
    return *(mStates.back());
}


inline StateMachine & StateMachine::pushState(std::shared_ptr<State> aState)
{
    mStates.push_back(std::move(aState));
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
