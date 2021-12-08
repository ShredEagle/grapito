#pragma once

#include "Utils/TimeSystemsUpdater.h"

#include <spdlog/spdlog.h>


// Usage example: AGLOG(trace)("This is a trace message with an int: {}", 4);
#define ADLOG(severity) spdlog::get("grapito")->severity

namespace ad {
namespace grapito {


void initializeLogging();


void log(const TimeSystemsUpdater & aUpdater);


} // namespace grapito
} // namespace ad
