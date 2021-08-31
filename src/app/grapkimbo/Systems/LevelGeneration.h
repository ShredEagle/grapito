#pragma once


#include "Input.h"

#include "Configuration.h"

#include "../Components/CameraGuide.h"
#include "../Components/Position.h"

#include <aunteater/EntityManager.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>

#include <math/Vector.h>

#include <functional>
#include <map>


namespace ad {


typedef aunteater::Archetype<CameraGuide, Position> Guide;


using TileIndex = math::Vec<2, int>;


} // namespace ad 


namespace std
{
    template<> struct less<ad::TileIndex>
    {
        bool operator() (const ad::TileIndex a, const ad::TileIndex b) const
        {
            return (a.y() < b.y()) || (a.y() == b.y() && a.x() < b.x());
        }
    };
} // namespace std


namespace ad {


struct IndexDifferences
{
    std::vector<TileIndex> removed;
    std::vector<TileIndex> added;
};


class LevelGeneration : public aunteater::System<GameInputState>
{
public:
    LevelGeneration(aunteater::EntityManager & aEntityManager);

    void update(const aunteater::Timer aTimer, const GameInputState &) override;

private:
    static std::vector<TileIndex> listConnected(TileIndex aIndex);

    IndexDifferences listDifferences(TileIndex aIndex) const;

    void pruneTiles(TileIndex aIndex);

    void generateTiles(TileIndex aIndex);

    // Abstracting away the detail of getting the camera
    math::Position<2, double> getCameraPosition() const;

    static constexpr math::Size<2, int> gTileDimension{
        (int)(3*render::gViewedHeight),
        (int)(3*render::gViewedHeight)
    };

    aunteater::EntityManager & mEntityManager; 

    // TODO find a good way to actally get the camera world position
    // without relying on the guides
    const aunteater::FamilyHelp<Guide> mGuides;

    TileIndex mCurrentTile{0, 0};
    std::map<TileIndex, std::vector<aunteater::weak_entity>> mTiles;
};


} // namespace ad
