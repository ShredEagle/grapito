#pragma once

#include <graphics/ApplicationGlfw.h>

#include <array>
#include <variant>

#include <map>
#include <vector>


namespace ad {
namespace grapito {


enum Command {
    Up, 
    Down,
    Left, 
    Right, 
    Jump,
    Grapple,
    ChangeMode, // intended to change the grappling mode (see GrappleControl component)
    R3,
    Start,
    Pause,
    Step,
    LeftHorizontalAxis,
    LeftVerticalAxis,
    RightHorizontalAxis,
    RightVerticalAxis,
    MouseXPos,
    MouseYPos,

    // Always leave that as last element, until we have reflection for enums
    EndCommand,
};


enum GamepadNature
{
    Button,
    Axis,
    AxisInverted,
};


struct KeyboardInputMapping
{
    Command command;
    int glKeyCode;
};


struct GamepadInputMapping
{
    Command command;
    int glGamePadCode;
    GamepadNature nature;
};


using KeyboardInputConfig = std::vector<KeyboardInputMapping> ;
using GamepadInputConfig = std::vector<GamepadInputMapping> ;


enum ButtonStatus
{
    Released,
    NegativeEdge, // just released
    Pressed,
    PositiveEdge, // just pressed
};

struct InputState
{
    std::variant<ButtonStatus, float> state;

    operator bool() const
    {
        return std::get<ButtonStatus>(state) >= ButtonStatus::Pressed;
    }

    bool positiveEdge() const
    {
        return std::get<ButtonStatus>(state) == ButtonStatus::PositiveEdge;
    }

    bool negativeEdge() const
    {
        return std::get<ButtonStatus>(state) == ButtonStatus::NegativeEdge;
    }

    operator float() const
    {
        return std::get<float>(state);
    }
};


// The index is the command
using ControllerInputState = std::array<InputState, EndCommand>;


enum class Controller
{
    KeyboardMouse,
    Gamepad_0,
    Gamepad_1,
    Gamepad_2,
    Gamepad_3,

    // Always leave that as last element, until we have reflection for enums
    End,
};


constexpr bool isGamepad(Controller aController);

bool isGamepadPresent(Controller aController);


struct GameInputState
{
    void readAll(graphics::ApplicationGlfw & aApplication);
    float asAxis(Controller aController, Command aNegativeButton, Command aPositiveButton, Command aGamepadAxis) const;
    math::Vec<2, float> asDirection(Controller aController,
                                    Command aHorizontalAxis,
                                    Command aVerticalAxis,
                                    float aDeadZone) const;
    math::Vec<2, float> asDirection(Controller aController,
                                    Command aHorizontalAxis,
                                    Command aVerticalAxis,
                                    float aHorizontalDeadZone,
                                    float aVerticalDeadZone) const;

    const ControllerInputState & get(Controller aController) const
    { return controllerState[(std::size_t)aController]; }

    math::Vec<2, float> getLeftDirection(Controller aController, float aDeadZone = 0.2) const
    { return asDirection(aController, LeftHorizontalAxis, LeftVerticalAxis, aDeadZone); }

    math::Vec<2, float> getRightDirection(Controller aController, float aDeadZone = 0.2) const
    { return asDirection(aController, RightHorizontalAxis, RightVerticalAxis, aDeadZone); }

    std::array<ControllerInputState, static_cast<std::size_t>(Controller::End)> controllerState;
};

extern const KeyboardInputConfig gKeyboardConfig;
extern const GamepadInputConfig gGamepadConfig;


} // namespace grapito
} // namespace ad
