#pragma once


#include "StateMachine.h"

#include <graphics/AppInterface.h>
#include <graphics/TrivialShaping.h>

#include <memory>


namespace ad {
namespace grapito {


class MenuScene : public State
{
public:
    MenuScene(std::shared_ptr<graphics::AppInterface> & aAppInterface);

    UpdateStatus update(
        GrapitoTimer & aTimer,
        const GameInputState & aInputs,
        StateMachine & aStateMachine) override;

private:
    graphics::TrivialShaping mShaping;
}; 


} // namespace grapito
} // namespace ad
