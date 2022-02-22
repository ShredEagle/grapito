#include "Logging.h"

#include "spdlog/sinks/stdout_color_sinks.h"

#include <graphics/detail/Logging.h>

#include <sstream>


namespace ad {
namespace grapito {


void initializeLogging()
{
    auto grapito = spdlog::stdout_color_mt("grapito");
    // TODO default levels should be controllable by config
    grapito->set_level(spdlog::level::debug);
    spdlog::get("opengl")->set_level(spdlog::level::warn);
}


void log(const TimeSystemsUpdater & aUpdater)
{
    std::ostringstream oss;
    aUpdater.outputTimings(oss);
    spdlog::get("grapito")->trace(oss.str());
}




} // namespace grapito
} // namespace ad
