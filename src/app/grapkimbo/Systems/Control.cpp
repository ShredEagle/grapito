#include "Control.h"

#include "Gravity.h"

#include <GLFW/glfw3.h>

namespace ad {


Control::Control(aunteater::Engine & aEngine) :
    mEngine{aEngine},
    mCartesianControllables{mEngine},
    mPolarControllables{mEngine}
{}


void Control::update(const aunteater::Timer aTimer)
{
    for(auto controllable : mCartesianControllables)
    {
        ForceAndSpeed & fas = controllable->get<ForceAndSpeed>();
        const Weight weight = controllable->get<Weight>();

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
                        fas.forces.emplace_back(- gAirControlAcceleration * weight.mass, 0.);
                        //fas.speeds[0].x() = std::max(10., fas.speeds[0].x());
                        break;
                    }
                    case COMMAND::RIGHT:
                    {
                        fas.forces.emplace_back(+ gAirControlAcceleration * weight.mass, 0.);
                        //fas.speeds[0].x() = std::min(-10., fas.speeds[0].x());
                        break;
                    }
                }
            }
        }
    }

    for(auto & entity : mPolarControllables)
    {
        auto & [_discard, geometry, pendular, weight] = entity;

        pendular.angularAccelerationControl = math::Radian<double>{0.};
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
                        pendular.angularAccelerationControl = 
                            math::Radian<double>{- Gravity::gAcceleration / pendular.length 
                                                 * gPendularControlAccelerationFactor};
                        break;
                    }
                    case COMMAND::RIGHT:
                    {
                        pendular.angularAccelerationControl =
                            math::Radian<double>{+ Gravity::gAcceleration / pendular.length 
                                                 * gPendularControlAccelerationFactor};
                        break;
                    }
                    case COMMAND::A:
                    {
                        break;
                    }
                    case COMMAND::B:
                    {
                        // TODO if we edit the components on the live entity, everything crashes because the 
                        // family are instantly edited to reflect the changes (invalidating iterators)
                        // this shoud be reworked!
                        mEngine.markToRemove(entity);
                        mEngine.addEntity(
                           aunteater::Entity()
                            .add<Position>(geometry)
                            .add<Controllable>()
                            .add<Weight>(weight.mass)
                            .add<ForceAndSpeed>(math::Vec<2>{
                                cos(pendular.angle) * pendular.length * pendular.angularSpeed.value(),
                                sin(pendular.angle) * pendular.length * pendular.angularSpeed.value()
                            })
                        );
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
