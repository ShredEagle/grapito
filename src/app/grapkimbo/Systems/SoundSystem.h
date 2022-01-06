#pragma once

#include "../Input.h"
#include "../commons.h"

#include "../Utils/SoundUtilities.h"

#include "../Components/SoundPlayer.h"

#include "../Ressources/SoundManager.h"
#include "aunteater/globals.h"

#include <aunteater/EntityManager.h>
#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>
#include <list>

namespace ad {
namespace grapito
{

struct PlayingSource
{
    ALuint mSource;
    std::list<Sound>::iterator mSoundDataDeleteIterator;
    SoundPlayer & mSoundPlayerReference;
};

typedef aunteater::Archetype<SoundPlayer> SoundType;

class SoundSystem : public aunteater::System<GrapitoTimer, GameInputState>, public aunteater::FamilyObserver
{

public:
    SoundSystem(aunteater::EntityManager & aEntityManager, SoundManager aSoundManager);

    ~SoundSystem();

    void update(const GrapitoTimer aTimer, const GameInputState &) override;

    void addedEntity(aunteater::LiveEntity & aEntity) override;

    void removedEntity(aunteater::LiveEntity & aEntity) override;

    template<class T_iterator>
    void loadSoundData(T_iterator soundDataBegin, T_iterator soundDataEnd);


private:
    const aunteater::FamilyHelp<SoundType> mSounds;
    ALCdevice * mOpenALDevice;
    ALCcontext * mOpenALContext;
    ALCboolean mContextIsCurrent;
    SoundManager mSoundManager;
    //Right now this is here but this should be stored in the scenes
    //later once I understand it better to allow pause and relaunch on scene change
    //TODO(franz): store this in the scenes
    std::list<PlayingSource> mPlayingSources;
};

template<class T_iterator>
void SoundSystem::loadSoundData(T_iterator soundDataBegin, T_iterator soundDataEnd)
{
    for (T_iterator soundIt = soundDataBegin; soundIt != soundDataEnd; soundIt++)
    {
        mSoundManager.storeDataInLoadedSound(*soundIt);
    }
}

} // namespace grapito
} // namespace ad
