#include "Control.h"

#include "Components/Position.h"

#include <GLFW/glfw3.h>

namespace ad {

Control::Control(aunteater::Engine & aEngine) :
    mEngine(aEngine),
    mControllables(mEngine)
{}


void Control::update(const aunteater::Timer aTimer)
{
    for(auto controllable : mControllables)
    {
        for (auto input : mInputState)
        {
            if (input.state == 1)
            {
                switch (input.command)
                {
                    case COMMAND::UP:
                    {
                        controllable->get<Position>().position.y() += 10;
                        break;
                    }
                    case COMMAND::DOWN:
                    {
                        controllable->get<Position>().position.y() += -10;
                        break;
                    }
                    case COMMAND::LEFT:
                    {
                        controllable->get<Position>().position.x() += -10;
                        break;
                    }
                    case COMMAND::RIGHT:
                    {
                        controllable->get<Position>().position.x() += 10;
                        break;
                    }
                }
            }
        }
    }
}

void Control::loadInputState(const gameInputState & aInputState)
{
    mInputState = aInputState;
}

} // namespace ad
