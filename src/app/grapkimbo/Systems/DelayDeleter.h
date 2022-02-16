#pragma once

#include "../Input.h"
#include "../commons.h"

#include "../Components/DelayDeletion.h"

#include <aunteater/EntityManager.h>
#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>

namespace ad {
namespace grapito
{

typedef aunteater::Archetype<DelayDeletion> Deletable;

class DelayDeleter : public aunteater::System<GrapitoTimer, GameInputState>
{

public:
    DelayDeleter(aunteater::EntityManager & aEntityManager);

    void update(const GrapitoTimer, const GameInputState &) override;

private:
    const aunteater::FamilyHelp<Deletable> mDeletable;

};

} // namespace grapito
} // namespace ad
