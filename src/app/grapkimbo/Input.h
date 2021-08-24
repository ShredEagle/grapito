#pragma once

#include <engine/Application.h>

#include <array>
#include <variant>

#include <map>
#include <vector>


namespace ad {


enum Command {
    Up, 
    Down,
    Left, 
    Right, 
    A,
    B,

    // Always leave that as last element, until we have reflection for enums
    EndCommand,
};


enum GamepadNature
{
    Button,
    Axis,
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


struct InputState
{
    std::variant<int, float> state;

    operator bool() const
    {
        return std::get<int>(state) == 1;
    }
};


// The index is the command
using ControllerInputState = std::array<InputState, EndCommand>;


enum class Controller
{
    Keyboard,
    Gamepad_0,

    // Always leave that as last element, until we have reflection for enums
    End,
};


struct GameInputState
{
    void readAll(Application & aApplication);

    std::array<ControllerInputState, static_cast<std::size_t>(Controller::End)> controllerState;
};


extern const KeyboardInputConfig gKeyboardConfig;
extern const GamepadInputConfig gGamepadConfig;


} // namespace ad
