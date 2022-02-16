#pragma once

#include "Input.h"
#include "commons.h"
#include <aunteater/UpdateTiming.h>

#include <websocket/WebSocket.h>

#include <mutex>

namespace ad {
namespace grapito
{

class DebugUI
{
public:
    DebugUI();

    // Non-movable, non-copyable (captures this)
    DebugUI(const DebugUI &) = delete;

    void broadcast(const aunteater::UpdateTiming<GrapitoTimer, GameInputState> & aTimings);

private:
    std::mutex mSessionsMutex;
    std::vector<WebSocket> mSessions;
    NetworkContext mNetworkContext;
};

} // namespace grapito
} // namespace ad

