#pragma once

#include "CameraGuidedControl.h" // for Camera archetype
#include "RopeCreation.h" // for RopeCreator archetype

#include "../Components/CameraTag.h"
#include "../Components/PlayerData.h"
#include "../Components/Position.h"
#include "../Components/VisualOutline.h"
#include "../Components/VisualRectangle.h"
#include "../Components/VisualPolygon.h"
#include "../Components/Body.h"

#include <aunteater/Archetype.h>
#include <aunteater/FamilyHelp.h>
#include <aunteater/System.h>

#include <graphics/ApplicationGlfw.h>
#include <graphics/Curving.h>
#include <graphics/DrawLine.h>
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

class Render : public aunteater::System<GrapitoTimer, GameInputState>
{

public:
    Render(aunteater::EntityManager & aEntityManager, graphics::ApplicationGlfw & aApplication);

    void update(const GrapitoTimer, const GameInputState &) override;

private:
    aunteater::EntityManager & mEntityManager;
    const aunteater::FamilyHelp<RenderedRectangle> mRectangles;
    const aunteater::FamilyHelp<RenderedPolygon> mPolygons;
    const aunteater::FamilyHelp<RenderedBodyRectangle> mBodyRectangles;
    const aunteater::FamilyHelp<RenderedBodyPolygon> mBodyPolygons;
    const aunteater::FamilyHelp<RenderedOutline> mOutlines;
    const aunteater::FamilyHelp<RopeCreatorType> mRopes;
    const aunteater::FamilyHelp<AimVector> mCrosshairs;
    const aunteater::FamilyHelp<Camera> mCameras;

    std::shared_ptr<graphics::AppInterface> mAppInterface;
    graphics::TrivialShaping mTrivialShaping;
    graphics::TrivialLineStrip mTrivialLineStrip;
    graphics::TrivialPolygon mTrivialPolygon;
    graphics::Curving mCurving;

};

} // namespace grapito
} // namespace ad
