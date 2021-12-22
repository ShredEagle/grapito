#include "GameRule.h"

#include "../Configuration.h"
#include "../Entities.h"
#include "../Logging.h"


namespace ad {
namespace grapito {

GameRule::GameRule(aunteater::EntityManager & aEntityManager) :
    mEntityManager{aEntityManager},
    mCompetitors{aEntityManager},
    mCameras{aEntityManager}
{}


void GameRule::update(const GrapitoTimer, const GameInputState &)
{
    // There is the one camera
    Position2 cameraPosition = mCameras.begin()->get<Position>().position;

    for (auto competitor : mCompetitors)
    {
        auto & [playerData, geometry] = competitor;            
        if (geometry.position.y() < (cameraPosition.y() - game::gCompetitorEliminationDistance))
        {
            ADLOG(info)("Player {} eliminated!", playerData.id);
            // TODO This is a good example of unsafe type system with entities:
            // provide a competitor where the archetype of a player is expected.
            kill(competitor, mEntityManager);
        }
    }
}


} // namespace grapitor
} // namespace ad
