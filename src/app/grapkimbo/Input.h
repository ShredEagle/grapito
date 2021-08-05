#pragma once

#include <array>

namespace ad {

enum COMMAND {
    UP, DOWN, LEFT, RIGHT, A,
};

struct Input
{
    COMMAND command;
    int GLKeyCode;
    int GLGamePadCode;
};

struct InputState
{
    COMMAND command;
    int state;
};

typedef std::array<Input, 5> gameInputConfig;
typedef std::array<InputState, 5> gameInputState;

}
