#include "LevelGeneration.h"

#include "Entities.h"


namespace ad {


LevelGeneration::LevelGeneration(aunteater::EntityManager & aEntityManager) :
    mEntityManager{aEntityManager},
    mGuides{aEntityManager}
{
    for(const TileIndex index : listConnected(mCurrentTile))
    {
        generateTiles(index);
    }
}


void LevelGeneration::update(const aunteater::Timer aTimer, const GameInputState &)
{
    math::Vec<2, int> cameraPosition = static_cast<math::Vec<2, int>>(getCameraPosition());
    TileIndex newIndex = cameraPosition.cwDiv(gTileDimension.as<math::Vec>());
    // The cpp division rounds towards zero, whereas we need flooring division 
    // (which changes the result when one operand is negative)
    if (cameraPosition.x() < 0) 
    {
        newIndex.x() -= 1;
    }
    if (cameraPosition.y() < 0) 
    {
        newIndex.y() -= 1;
    }

    if (newIndex != mCurrentTile)
    {
        IndexDifferences differences = listDifferences(newIndex);
        for (TileIndex removedId : differences.removed)
        {
            pruneTiles(removedId);
        }
        for (TileIndex addedId : differences.added)
        {
            generateTiles(addedId);
        }
        mCurrentTile = newIndex;
    }
}


std::vector<TileIndex> LevelGeneration::listConnected(TileIndex aIndex)
{
    std::vector<TileIndex> result;
    for (int j = -1; j != 2; ++j)
    {
        for (int i = -1; i != 2; ++i)
        {
            result.push_back(aIndex + TileIndex{i, j});
        }
    }
    return result;
}


IndexDifferences LevelGeneration::listDifferences(TileIndex aIndex) const
{
    auto oldSet = listConnected(mCurrentTile);
    auto newSet = listConnected(aIndex);

    IndexDifferences result{
        oldSet
    };

    for (const TileIndex id : newSet)
    {
        if (std::find(oldSet.begin(), oldSet.end(), id) == oldSet.end())
        {
            result.added.push_back(id);
        }
        
        auto removedIt = std::find(result.removed.begin(), result.removed.end(), id);
        if (removedIt != result.removed.end())
        {
            result.removed.erase(removedIt);
        }
    }
    
    return result; 
}


void LevelGeneration::pruneTiles(TileIndex aIndex)
{
    for(aunteater::weak_entity anchor : mTiles.at(aIndex))
    {
        anchor->markToRemove(); 
    }
    mTiles.erase(aIndex);
}


void LevelGeneration::generateTiles(TileIndex aIndex)
{
    std::vector<aunteater::weak_entity> anchors;

    static constexpr int count = 5;
    for (int y = 0; y != count; ++y)
    {
        for (int x = 0; x != count; ++x)
        {
            auto offset = static_cast<math::Vec<2, double>>(gTileDimension) / count;
            math::Position<2, double> position = 
                static_cast<math::Position<2, double>>(gTileDimension.cwMul(aIndex.as<math::Size>()))
                + offset.cwMul({(double)x, (double)y});
                ;
            math::Size<2, double> size{2., 2.};
            anchors.push_back(mEntityManager.addEntity(makeAnchor(position, size)));
        }
    }

    mTiles.emplace(aIndex, std::move(anchors));
}


math::Position<2, double> LevelGeneration::getCameraPosition() const
{
    for(const auto & [cameraGuide, geometry] : mGuides)
    {
        return geometry.position;
    }
    return {0., 0.};
}


} // namespace ad
