#pragma once

#include "../Input.h"
#include "commons.h"

#include "../Utils/SoundUtilities.h"

#include "../Components/Sound.h"

#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>

namespace ad {
namespace grapito
{

typedef aunteater::Archetype<Sound> SoundType;

class SoundSystem : public aunteater::System<GrapitoTimer, GameInputState>
{

public:
    SoundSystem(aunteater::EntityManager & aEntityManager);

    ~SoundSystem();

    void update(const GrapitoTimer aTimer, const GameInputState &) override;

private:
    const aunteater::FamilyHelp<SoundType> mSounds;
    ALCdevice * mOpenALDevice;
    ALCcontext * mOpenALContext;
    ALCboolean contextIsCurrent;
};


} // namespace grapito
} // namespace ad
