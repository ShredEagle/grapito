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
    virtual /*std::optional<shared_ptr<State>>*/ bool update(
        GrapitoTimer & aTimer,
        const GameInputState & aInputs,
        StateMachine & aStateMachine) = 0;
};


class StateMachine
{
public:
    StateMachine(std::shared_ptr<State> aInitialState);

    bool update(GrapitoTimer & aTimer, const GameInputState & aInputs);

private:
    std::vector<std::shared_ptr<State>> mStates;
};


//
// Implementations
//
inline StateMachine::StateMachine(std::shared_ptr<State> aInitialState) :
    mStates{std::move(aInitialState)}
{}


inline bool StateMachine::update(GrapitoTimer & aTimer, const GameInputState & aInputs)
{
    return mStates.back()->update(aTimer, aInputs, *this);
}


} // namespace grapito
} // namespace ad
