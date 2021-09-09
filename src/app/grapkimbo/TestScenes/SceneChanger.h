#pragma once

#include "Game.h"
#include "TestScenes/Game_pendulum.h"
#include "TestScenes/CollisionTest.h"
#include "TestScenes/FrictionTest.h"
#include "aunteater/EntityManager.h"
#include "engine/Application.h"
namespace ad {
namespace grapito {

static std::unique_ptr<Game> currentGame;
static bool pause = false;

enum GameList
{
    CollisionTest,
    GamePendulum,
    FrictionTest
};

/*
 * I'm not good at cpp
static std::vector<std::function<Game(Application & aApplication)>> games;

static void setupSceneChanger()
{
    games[Games::CollisionTest] = createCollisionTestGame;
    games[Games::GamePendulum] = createCollisionTestGame;
}

*/

static void ChangeScene(GameList aGameType, Application & aApplication, DebugUI & aUI)
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
    }
}

}
}
