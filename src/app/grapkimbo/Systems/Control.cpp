#include "Control.h"

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
        ForceAndSpeed fas = controllable->get<ForceAndSpeed>();

        for (auto input : mInputState)
        {
            if (input.state == 1)
            {
                switch (input.command)
                {
                    case COMMAND::UP:
                    {
                        break;
                    }
                    case COMMAND::DOWN:
                    {
                        break;
                    }
                    case COMMAND::LEFT:
                    {
                        fas.speeds[0].x() = std::max(10., fas.speeds[0].x());
                        break;
                    }
                    case COMMAND::RIGHT:
                    {
                        fas.speeds[0].x() = std::min(-10., fas.speeds[0].x());
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
