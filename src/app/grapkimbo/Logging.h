#pragma once

#include "Utils/TimeSystemsUpdater.h"

#include <spdlog/spdlog.h>


namespace ad {
namespace grapito {


void initializeLogging();


struct GameInputState;
void log(const TimeSystemsUpdater<GameInputState> & aUpdater);



} // namespace grapito
} // namespace ad
