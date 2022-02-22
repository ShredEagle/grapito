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
    Back,
    Pause,
    Step,
    LeftHorizontalAxis,
    LeftVerticalAxis,
    RightHorizontalAxis,
    RightVerticalAxis,
    MouseXPos,
    MouseYPos,

    // Always leave that as last element, until we have reflection for enums
    _EndCommand,
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


struct AxisStatus
{
    float previous{0.f};
    float current{0.f};

    AxisStatus & operator=(float aNewValue)
    {
        previous = current;
        current = aNewValue;
        return *this;
    }

    operator float () const
    {
        return current;
    }
};

struct InputState
{
    // Note: The order is important, GameInputState initialization code assumes the default alternative
    // is a ButtonStatus (and explicitly default initializes the rest).
    std::variant<ButtonStatus, AxisStatus> state;

    operator bool() const
    {
        // By comparing to a variant, it will return false if the current alternative is not a ButtonStatus
        // (instead of throwing bad_variant_access).
        return state >= std::variant<ButtonStatus, AxisStatus>{ButtonStatus::Pressed};
    }

    bool positiveEdge() const
    {
        return std::get<ButtonStatus>(state) == ButtonStatus::PositiveEdge;
    }

    bool negativeEdge() const
    {
        return std::get<ButtonStatus>(state) == ButtonStatus::NegativeEdge;
    }

    operator ButtonStatus() const
    {
        return std::get<ButtonStatus>(state);
    }


    operator float() const
    {
        return std::get<AxisStatus>(state);
    }

    AxisStatus & axis()
    {
        return std::get<AxisStatus>(state);
    }
};


// The index is the command
using ControllerInputState = std::array<InputState, _EndCommand>;


enum class Controller
{
    KeyboardMouse,
    Gamepad_0,
    Gamepad_1,
    Gamepad_2,
    Gamepad_3,

    // Always leave that as last element, until we have reflection for enums
    _End,
};

constexpr std::size_t gControllerCount = static_cast<std::size_t>(Controller::_End);

constexpr std::array<Controller, gControllerCount> gAllControllers{
    Controller::KeyboardMouse,
    Controller::Gamepad_0,
    Controller::Gamepad_1, 
    Controller::Gamepad_2, 
    Controller::Gamepad_3
};

constexpr bool isGamepad(Controller aController)
{
    return aController != Controller::KeyboardMouse;
}

bool isGamepadPresent(Controller aController);

std::vector<Controller> listPresentGamepads();

enum class AxisSign
{
    Positive,
    Negative
};

struct GameInputState
{
    // Default constructor will initialize the ControllerInputState whit a default constructed state of the right alternative.
    GameInputState();

    void readAll(graphics::ApplicationGlfw & aApplication);
    float asAxis(Controller aController, Command aNegativeButton, Command aPositiveButton, Command aGamepadAxis, float aDeadzone = 0.f) const;
    ButtonStatus asButton(Controller aController, Command aButton, Command aGamepadAxis, AxisSign aSign, float aDeadzone) const;
    math::Vec<2, float> asDirection(Controller aController,
                                    Command aHorizontalAxis,
                                    Command aVerticalAxis,
                                    float aDeadzone) const;
    math::Vec<2, float> asDirection(Controller aController,
                                    Command aHorizontalAxis,
                                    Command aVerticalAxis,
                                    float aHorizontalDeadzone,
                                    float aVerticalDeadzone) const;

    const ControllerInputState & get(Controller aController) const
    { return controllerState[(std::size_t)aController]; }

    math::Vec<2, float> getLeftDirection(Controller aController, float aDeadzone = 0.2) const
    { return asDirection(aController, LeftHorizontalAxis, LeftVerticalAxis, aDeadzone); }

    math::Vec<2, float> getRightDirection(Controller aController, float aDeadzone = 0.2) const
    { return asDirection(aController, RightHorizontalAxis, RightVerticalAxis, aDeadzone); }

    std::array<ControllerInputState, gControllerCount> controllerState;
};

extern const KeyboardInputConfig gKeyboardConfig;
extern const GamepadInputConfig gGamepadConfig;


} // namespace grapito
} // namespace ad
