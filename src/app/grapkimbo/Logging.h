#pragma once

#include "Utils/TimeSystemsUpdater.h"

#include <spdlog/spdlog.h>


namespace ad {
namespace grapito {


void initializeLogging();


void log(const TimeSystemsUpdater & aUpdater);


} // namespace grapito
} // namespace ad
