#include "SoundSystem.h"

#include <alc.h>
#include <spdlog/spdlog.h>

namespace ad {
namespace grapito
{

SoundSystem::SoundSystem(aunteater::EntityManager & aEntityManager) :
    mSounds(aEntityManager)
{
    mOpenALDevice = alcOpenDevice(nullptr);
    if(!mOpenALDevice)
    {
        /* fail */
        spdlog::get("grapito")->error("Cannot open OpenAL sound device");
    }
    else
    {
        if (!alcCall(alcCreateContext, mOpenALContext, mOpenALDevice, mOpenALDevice, nullptr))
        {
            spdlog::get("grapito")->error("Cannot create OpenAL context");
        }
        else
        {
            if (!alcCall(alcMakeContextCurrent, contextIsCurrent, mOpenALDevice, mOpenALContext));
            {
                spdlog::get("grapito")->error("Cannot set OpenAL to current context");
            }
        }
    }
}

SoundSystem::~SoundSystem()
{
    if (contextIsCurrent)
    {
        if (!alcCall(alcMakeContextCurrent, contextIsCurrent, mOpenALDevice, nullptr))
        {
            spdlog::get("grapito")->error("Well we're leaking audio memory now");
        }

        if (!alcCall(alcDestroyContext, mOpenALDevice, mOpenALContext))
        {
            spdlog::get("grapito")->error("Well we're leaking audio memory now");
        }

        ALCboolean closed;
        if (!alcCall(alcCloseDevice, closed, mOpenALDevice, mOpenALDevice))
        {
            spdlog::get("grapito")->error("Device just dissapeared and I don't know why");
        }

    }
}

void SoundSystem::update(const GrapitoTimer, const GameInputState &)
{
}

} // namespace grapito
} // namespace ad
