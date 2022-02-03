#pragma once

#include "Game.h"
#include "CollisionTest.h"
#include "DistanceTest.h"
#include "FrictionTest.h"
#include "PivotTest.h"
#include "WeldTest.h"
#include "SimpleCollisionTest.h"
#include "SetPositionTest.h"

#include <aunteater/EntityManager.h>

#include <graphics/ApplicationGlfw.h>

namespace ad {
namespace grapito {

static std::unique_ptr<Game> currentGame;

enum GameList
{
    CollisionTest,
    DistanceTest,
    FrictionTest,
    SimpleCollisionTest,
    PivotTest,
    WeldTest,
    SetPositionTest,
};

/*
 * I'm not good at cpp
static std::vector<std::function<Game(ApplicationGlfw & aApplication)>> games;

static void setupSceneChanger()
{
    games[Games::CollisionTest] = createCollisionTestGame;
    games[Games::GamePendulum] = createCollisionTestGame;
}

*/

static void ChangeScene(GameList aGameType, graphics::ApplicationGlfw & aApplication, DebugUI & aUI)
{
    switch (aGameType) {
    case GameList::CollisionTest:
        currentGame = std::make_unique<class CollisionTest>(aApplication, aUI);
        break;
    case GameList::DistanceTest:
        currentGame = std::make_unique<class DistanceTest>(aApplication, aUI);
        break;
    case GameList::FrictionTest:
        currentGame = std::make_unique<class FrictionTest>(aApplication, aUI);
        break;
    case GameList::SimpleCollisionTest:
        currentGame = std::make_unique<class SimpleCollisionTest>(aApplication, aUI);
        break;
    case GameList::PivotTest:
        currentGame = std::make_unique<class PivotTest>(aApplication, aUI);
        break;
    case GameList::WeldTest:
        currentGame = std::make_unique<class WeldTest>(aApplication, aUI);
        break;
    case GameList::SetPositionTest:
        currentGame = std::make_unique<class SetPositionTest>(aApplication, aUI);
        break;
    }
}

}
}
