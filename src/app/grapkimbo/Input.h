#pragma once

#include <array>
#include <variant>

#include <vector>


namespace ad {


enum Command {
    Up, 
    Down,
    Left, 
    Right, 
    A,
    B,
};


enum GamepadNature
{
    Button,
    Axis,
};


struct Input
{
    Command command;
    int glKeyCode;
    int glGamePadCode;
    GamepadNature gamepadNature;

    constexpr bool isButton()
    {
        return gamepadNature == Button;
    }
};


struct InputState
{
    Command command;
    GamepadNature nature;
    std::variant<int, float> state;
};


typedef std::vector<Input> GameInputConfig;
typedef std::array<InputState, 6> GameInputState;


} // namespace ad
