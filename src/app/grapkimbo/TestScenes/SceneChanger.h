#pragma once

#include "Game.h"
#include "TestScenes/Game_pendulum.h"
#include "TestScenes/CollisionTest.h"
#include "TestScenes/FrictionTest.h"
#include "TestScenes/PivotTest.h"
#include "TestScenes/SimpleCollisionTest.h"
#include "TestScenes/SetPositionTest.h"
#include "aunteater/EntityManager.h"

#include <graphics/ApplicationGlfw.h>

namespace ad {
namespace grapito {

static std::unique_ptr<Game> currentGame;
static bool pause = false;

enum GameList
{
    CollisionTest,
    GamePendulum,
    FrictionTest,
    SimpleCollisionTest,
    PivotTest,
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
    case GameList::GamePendulum:
        currentGame = std::make_unique<Game_pendulum>(aApplication, aUI);
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
    case GameList::SetPositionTest:
        currentGame = std::make_unique<class SetPositionTest>(aApplication, aUI);
        break;
    }
}

}
}
