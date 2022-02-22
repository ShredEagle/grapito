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
        else
        {
            mOrphanPlayingSounds.push_back(std::move(sound));
        }
    }
}

void SoundSystem::update(const GrapitoTimer, const GameInputState &)
{
    std::vector<ALuint> sourceToDelete;
    for (auto & [soundPlayer] : mSounds)
    {
        for (auto & sound : soundPlayer.mSounds)
        {
            if (!sound.mPlaying)
            {
                ALuint sourceId = mSoundManager.playSound(sound.mSoundId, sound.mOptions);
                sound.sourceId = sourceId;
                sound.mPlaying = true;
            }
        }
        checkAndPrepareSourceDeletion(sourceToDelete, soundPlayer.mSounds);
    }

    checkAndPrepareSourceDeletion(sourceToDelete, mOrphanPlayingSounds);

    mSoundManager.deleteSources(sourceToDelete);
}

void SoundSystem::checkAndPrepareSourceDeletion(std::vector<ALuint> & outSourceToDelete, std::list<Sound> & aSoundList)
{
    for (auto soundIterator = aSoundList.begin(); soundIterator != aSoundList.end();)
    {
        Sound sound = *soundIterator;
        ALint sourceState = mSoundManager.getSourceState(sound.sourceId);

        if (sourceState == AL_STOPPED)
        {
            outSourceToDelete.push_back(sound.sourceId);
            soundIterator = aSoundList.erase(soundIterator);
        }
        else
        {
            ++soundIterator;
        }
    }
}
}
}
