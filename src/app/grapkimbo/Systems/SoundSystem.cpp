#include "SoundSystem.h"

#include "../Components/SoundPlayer.h"
#include "../Timer.h"

#include <handy/StringId_Interning.h>

#include <AL/al.h>
#include <AL/alc.h>
#include <spdlog/spdlog.h>

namespace ad {
namespace grapito
{

SoundSystem::SoundSystem(aunteater::EntityManager & aEntityManager, SoundManager & aSoundManager) :
    mSounds{aEntityManager},
    mSoundManager{aSoundManager}
{
    aEntityManager.getFamily<SoundType>().registerObserver(this);
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
                ALuint sourceId = mSoundManager.playSound(sound.mSoundId, sound.mLooping);
                sound.sourceId = sourceId;
                mPlayingSources.emplace_back(PlayingSource{sourceId, soundIterator, soundPlayer});
                sound.mPlaying = true;
            }
        }
    }
    std::vector<ALuint> sourceToDelete;

    for (auto sourceIterator = mPlayingSources.begin(); sourceIterator != mPlayingSources.end();)
    {
        PlayingSource & playingSource = *sourceIterator;
        ALint sourceState = mSoundManager.getSourceState(playingSource.mSource);

        if (sourceState == AL_STOPPED)
        {
            sourceToDelete.push_back(playingSource.mSource);
            playingSource.mSoundPlayerReference.mSounds.erase(playingSource.mSoundDataDeleteIterator);
            sourceIterator = mPlayingSources.erase(sourceIterator);
        }
        else
        {
            ++sourceIterator;
        }
    }

    mSoundManager.deleteSources(sourceToDelete);
}

}
}
