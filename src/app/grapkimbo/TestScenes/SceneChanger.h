#pragma once

#include "Game.h"
#include "Game_pendulum.h"
#include "TestScenes/Collision.h"
#include "aunteater/EntityManager.h"
#include "engine/Application.h"
namespace ad {
namespace grapito {

static std::unique_ptr<Game> currentGame;

enum GameList
{
    CollisionTest,
    GamePendulum,
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

static void ChangeScene(GameList aGameType, Application & aApplication)
{
    switch (aGameType) {
    case GameList::CollisionTest:
        currentGame = std::make_unique<class CollisionTest>(aApplication);
        break;
    case GameList::GamePendulum:
        currentGame = std::make_unique<Game_pendulum>(aApplication);
        break;
    }
}

}
}
