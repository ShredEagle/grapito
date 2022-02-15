#pragma once

#include "commons.h"
#include <aunteater/EntityManager.h>
#include <aunteater/Component.h>

#include <AL/al.h>

#include <string>
#include <vector>

namespace ad {
namespace grapito
{

struct Sound
{
    explicit Sound(StringId aSoundId, sounds::SoundOption aOptions, bool aStreaming, bool aStopOnDelete) :
        mSoundId{aSoundId},
        mOptions{aOptions},
        mStreaming{aStreaming},
        mStopOnDelete{aStopOnDelete}
    {};

    //mPlaying should never be initialize to anything other than false
    Sound(StringId aSoundId, sounds::SoundOption aOptions, bool aStreaming, bool aStopOnDelete, bool aPlaying) = delete;

    StringId mSoundId;
    sounds::SoundOption mOptions;
    bool mStreaming = false;
    bool mStopOnDelete = false;
    bool mPlaying = false;
    ALuint sourceId;
};


struct SoundPlayer : public aunteater::Component<SoundPlayer>
{
    explicit SoundPlayer()
    {}

    std::list<Sound> mSounds;
};

inline void addSoundToEntity(aunteater::weak_entity aEntity, StringId aSoundId, sounds::SoundOption aOptions, bool aStreaming = false, bool aStopOnDelete = false)
{
    if (!aEntity->has<SoundPlayer>())
    {
        aEntity->add<SoundPlayer>();
    }

    SoundPlayer & sp = aEntity->get<SoundPlayer>();
    sp.mSounds.emplace_back(Sound{aSoundId, aOptions, aStreaming, aStopOnDelete});
};

} // namespace grapito
} // namespace ad
