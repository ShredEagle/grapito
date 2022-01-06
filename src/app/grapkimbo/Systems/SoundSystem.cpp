#include "SoundSystem.h"

#include "../Components/SoundPlayer.h"

#include <handy/StringId_Interning.h>

#include <AL/al.h>
#include <AL/alc.h>
#include <spdlog/spdlog.h>

namespace ad {
namespace grapito
{

const StringId soundId_MusicSid = handy::internalizeString("ahouais");

SoundSystem::SoundSystem(aunteater::EntityManager & aEntityManager, SoundManager aSoundManager) :
    mSounds{aEntityManager},
    mSoundManager{std::move(aSoundManager)}
{
    aEntityManager.getFamily<SoundType>().registerObserver(this);
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
            if (!alcCall(alcMakeContextCurrent, mContextIsCurrent, mOpenALDevice, mOpenALContext))
            {
                spdlog::get("grapito")->error("Cannot set OpenAL to current context");
            }
        }
    }
}

SoundSystem::~SoundSystem()
{
    if (mContextIsCurrent)
    {
        if (!alcCall(alcMakeContextCurrent, mContextIsCurrent, mOpenALDevice, nullptr))
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
            spdlog::get("grapito")->error("Device just disappeared and I don't know why");
        }

    }
}

void SoundSystem::addedEntity(aunteater::LiveEntity &)
{}

void SoundSystem::removedEntity(aunteater::LiveEntity & aEntity)
{
    SoundPlayer & player = aEntity.get<SoundPlayer>();

    for (auto sound : player.mSounds)
    {
        if (sound.mStopOnDelete)
        {
            mSoundManager.stopSound(sound.sourceId);
        }
    }
}

void SoundSystem::update(const GrapitoTimer, const GameInputState &)
{
    for (auto & [soundPlayer] : mSounds)
    {
        for (auto soundIterator = soundPlayer.mSounds.begin(); soundIterator != soundPlayer.mSounds.end(); ++soundIterator)
        {
            auto & sound = *soundIterator;
            if (!sound.mPlaying)
            {
                ALuint sourceId = mSoundManager.playSound(sound.mSoundId);
                sound.sourceId = sourceId;
                mPlayingSources.emplace_back(PlayingSource{sourceId, soundIterator, soundPlayer});
            }
        }
    }
    std::vector<ALuint> sourceToDelete;

    for (auto sourceIterator = mPlayingSources.begin(); sourceIterator != mPlayingSources.end();)
    {
        PlayingSource & playingSource = *sourceIterator;
        ALint sourceState;
        alCall(alGetSourcei, playingSource.mSource, AL_SOURCE_STATE, &sourceState);

        if (sourceState == AL_STOPPED)
        {
            sourceToDelete.push_back(playingSource.mSource);
            sourceIterator = mPlayingSources.erase(sourceIterator);
            playingSource.mSoundPlayerReference.mSounds.erase(playingSource.mSoundDataDeleteIterator);
        }
        else
        {
            ++sourceIterator;
        }
    }

    alCall(alDeleteSources, sourceToDelete.size(), sourceToDelete.data());
}

}
}
