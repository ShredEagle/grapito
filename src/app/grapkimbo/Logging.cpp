#include "Logging.h"

#include "spdlog/sinks/stdout_color_sinks.h"

#include <sstream>


namespace ad {
namespace grapito {


void initializeLogging()
{
    auto grapito = spdlog::stdout_color_mt("grapito");
    grapito->set_level(spdlog::level::debug);
}


void log(const TimeSystemsUpdater<GameInputState> & aUpdater)
{
    std::ostringstream oss;
    aUpdater.outputTimings(oss);
    spdlog::get("grapito")->trace(oss.str());
}




} // namespace grapito
} // namespace ad
