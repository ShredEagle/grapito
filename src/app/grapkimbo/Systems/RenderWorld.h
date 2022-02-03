#pragma once

#include "CameraGuidedControl.h" // for Camera archetype
#include "RopeCreation.h" // for RopeCreator archetype

#include "../Components/Body.h"
#include "../Components/CameraTag.h"
#include "../Components/PlayerData.h"
#include "../Components/Position.h"
#include "../Components/VisualOutline.h"
#include "../Components/VisualRectangle.h"
#include "../Components/VisualPolygon.h"
#include "../Components/VisualSprite.h"

#include "../Utils/RopeUtilities.h"

#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>

#include <graphics/ApplicationGlfw.h>
#include <graphics/Curving.h>
#include <graphics/DrawLine.h>
#include <graphics/SpriteAnimator.h>
#include <graphics/Spriting.h>
#include <graphics/TrivialLineStrip.h>
#include <graphics/TrivialPolygon.h>
#include <graphics/TrivialShaping.h>


namespace ad {
namespace grapito
{

typedef aunteater::Archetype<Position, VisualRectangle> RenderedRectangle;
typedef aunteater::Archetype<Position, VisualPolygon> RenderedPolygon;
typedef aunteater::Archetype<Position, Body, VisualRectangle> RenderedBodyRectangle;
typedef aunteater::Archetype<Position, Body, VisualPolygon> RenderedBodyPolygon;
typedef aunteater::Archetype<Position, VisualOutline> RenderedOutline;
typedef aunteater::Archetype<Position, Body, PlayerData> AimVector;
typedef aunteater::Archetype<Position, VisualSprite> RenderedSprite;

class RenderWorld : public aunteater::System<GrapitoTimer, GameInputState>
{

public:
    RenderWorld(aunteater::EntityManager & aEntityManager,
                std::shared_ptr<graphics::AppInterface> aAppInterface);

    void render() const;

    void update(const GrapitoTimer aTimer, const GameInputState &) override;

    AtlasIndex installAtlas(graphics::sprite::LoadedAtlas aAtlas);

private:
    aunteater::EntityManager & mEntityManager;
    std::shared_ptr<graphics::AppInterface> mAppInterface;

    const aunteater::FamilyHelp<RenderedRectangle> mRectangles;
    const aunteater::FamilyHelp<RenderedPolygon> mPolygons;
    const aunteater::FamilyHelp<RenderedBodyRectangle> mBodyRectangles;
    const aunteater::FamilyHelp<RenderedBodyPolygon> mBodyPolygons;
    const aunteater::FamilyHelp<RenderedOutline> mOutlines;
    const aunteater::FamilyHelp<RopeCreatorType> mRopes;
    const aunteater::FamilyHelp<AimVector> mCrosshairs;
    const aunteater::FamilyHelp<RenderedSprite> mSprites;
    const aunteater::FamilyHelp<Camera> mCameras;

    graphics::TrivialShaping mTrivialShaping;
    graphics::TrivialLineStrip mTrivialLineStrip;
    graphics::TrivialPolygon mTrivialPolygon;
    graphics::Curving mCurving;
    // TODO Ad 2022/01/26: Awfull SMELL
    // I now think that having several atlases (textures) to render on a single Spriting renderer
    // complicates things too much. A big atlas with all color variations, and some better logic in
    // animation system to offset to the expected variations, could simplify the code a lot
    // (no more mapping between atlases and their sprites, etc).
    // Alternatively, we could "externalize" the VAO (out of Spriting renderer), and feed it on ::render().
    // (see Tiling)
    // But for the moment, Spriting is mutable, so ::render() can update instances per atlas...
    mutable graphics::Spriting mSpriting;

    Spline mBeziers;

    std::vector<graphics::sprite::LoadedAtlas> mAtlases;
    // An (implicit) mapping between an AtlasIndex and a vector of Sprites for this atlas.
    std::vector<std::vector<graphics::Spriting::Instance>> mAtlasToSprites;
};


} // namespace grapito
} // namespace ad
