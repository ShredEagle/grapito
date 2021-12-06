#pragma once

#include "CameraGuidedControl.h" // for Camera archetype
#include "RopeCreation.h" // for RopeCreator archetype

#include "../Components/AnimatedSprite.h"
#include "../Components/Body.h"
#include "../Components/CameraTag.h"
#include "../Components/PlayerData.h"
#include "../Components/Position.h"
#include "../Components/VisualOutline.h"
#include "../Components/VisualRectangle.h"
#include "../Components/VisualPolygon.h"

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
typedef aunteater::Archetype<Position, AnimatedSprite> RenderedSprite;

class Render : public aunteater::System<GrapitoTimer, GameInputState>
{

public:
    Render(aunteater::EntityManager & aEntityManager,
           std::shared_ptr<graphics::AppInterface> aAppInterface);

    void render() const;

    void update(const GrapitoTimer aTimer, const GameInputState &) override;

    //void loadSpriteAnimations(std::initializer_list<
    void loadSpriteAnimation(const arte::AnimationSpriteSheet & aSpriteSheet);

private:
    aunteater::EntityManager & mEntityManager;
    const aunteater::FamilyHelp<RenderedRectangle> mRectangles;
    const aunteater::FamilyHelp<RenderedPolygon> mPolygons;
    const aunteater::FamilyHelp<RenderedBodyRectangle> mBodyRectangles;
    const aunteater::FamilyHelp<RenderedBodyPolygon> mBodyPolygons;
    const aunteater::FamilyHelp<RenderedOutline> mOutlines;
    const aunteater::FamilyHelp<RopeCreatorType> mRopes;
    const aunteater::FamilyHelp<AimVector> mCrosshairs;
    const aunteater::FamilyHelp<RenderedSprite> mSprites;
    const aunteater::FamilyHelp<Camera> mCameras;

    std::shared_ptr<graphics::AppInterface> mAppInterface;
    graphics::TrivialShaping mTrivialShaping;
    graphics::TrivialLineStrip mTrivialLineStrip;
    graphics::TrivialPolygon mTrivialPolygon;
    graphics::Curving mCurving;

    graphics::sprite::Animator mAnimator;
    graphics::Spriting mSpriting;

    Spline mBeziers;
};

} // namespace grapito
} // namespace ad
