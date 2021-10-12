#include "Scene.h"

#include <graphics/Application.h>

#include <math/Color.h>

int main(int argc, const char * argv[])
{
    try
    {
        ad::Application application("Grapito splines", 1600, 900,
                                    ad::Application::Window_Keep_Ratio);

        ad::Scene scene{application.getAppInterface()->getFramebufferSize()};

        scene.mPoints = {
            {{-100.f,  -60.f}, ad::math::sdr::gGreen},
            {{ -20.f,   20.f}, ad::math::sdr::gGreen},
            {{  20.f,   20.f}, ad::math::sdr::gGreen},
            {{ 100.f,  -60.f}, ad::math::sdr::gGreen},
            {{ 200.f,    0.f}, ad::math::sdr::gGreen},
            {{ 300.f,    0.f}, ad::math::sdr::gGreen},
            {{ 350.f, -100.f}, ad::math::sdr::gGreen},
        };

        while(application.handleEvents())
        {
            scene.step();
            scene.render();
            application.swapBuffers();
        }
    }
    catch(const std::exception & e)
    {
        std::cerr << "Exception:\n"
                  << e.what()
                  << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::exit(EXIT_SUCCESS);
}

