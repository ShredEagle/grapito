#pragma once

#include <aunteater/EntityManager.h>
#include <aunteater/Component.h>

#include <al.h>

#include <string>
#include <vector>

namespace ad {
namespace grapito
{

struct Sound
{
    explicit Sound(std::string aFilename, bool aLooping, bool aStreaming, bool aStopOnDelete) :
        mFilename{aFilename},
        mLooping{aLooping},
        mStreaming{aStreaming},
        mStopOnDelete{aStopOnDelete}
    {};

    //mPlaying should never be initialize to anything other than false
    Sound(std::string aFilename, bool aLooping, bool aStreaming, bool aStopOnDelete, bool aPlaying) = delete;

    std::string mFilename;
    bool mLooping;
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

static void addSoundToEntity(aunteater::weak_entity aEntity, std::string aFilename, bool aLooping = false, bool aStreaming = false, bool aStopOnDelete = false)
{
    if (!aEntity->has<SoundPlayer>())
    {
        aEntity->add<SoundPlayer>();
    }

    SoundPlayer & sp = aEntity->get<SoundPlayer>();
    sp.mSounds.emplace_back(Sound{aFilename, aLooping, aStreaming, aStopOnDelete});
};

} // namespace grapito
} // namespace ad
