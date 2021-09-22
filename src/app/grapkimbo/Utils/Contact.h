#pragma once

#include "Utils/CollisionBox.h"
#include "commons.h"

#include "aunteater/globals.h"
#include "math/Vector.h"
#include "Utils/DrawDebugStuff.h"

#include <vector>

namespace ad
{
namespace grapito
{

enum ReferenceFace
{
    FACEA,
    FACEB,
};


struct Contact
{
    Position2 point;
    double impulse = 0.;
    double tangentImpulse = 0.;
};

struct ContactQuery
{
    ContactQuery(
        const aunteater::weak_entity aEntity
    ) :
        entity{aEntity}
    {};

#ifdef KIMBO_DEBUG
    void debugRender(Color vecColor);
    Position2 origin = Position2::Zero();
    Position2 end = Position2::Zero();
    Position2 point = Position2::Zero();
#endif

    aunteater::weak_entity entity;
    double distance = 0.;
    Vec2 normal = Vec2::Zero();

    int index = -1;
    int incEdgeIndex = -1;
    ReferenceFace face;

    std::vector<Contact> contacts;
};

} //namespace grapito
} //namespace ad
