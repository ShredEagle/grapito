#pragma once

#include "../TestScenes/SceneChanger.h"
#include "../Utils/DrawDebugStuff.h"

#include <graphics/ApplicationGlfw.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <cstdio>

namespace ad {
namespace grapito {

using namespace ImGui;


static void setupImGui(graphics::ApplicationGlfw & aApplication)
{
    IMGUI_CHECKVERSION();
    CreateContext();

    ImGuiIO & io = GetIO(); (void)io;

    StyleColorsDark();

    aApplication.loadWindowIntoImGui(ImGui_ImplGlfw_InitForOpenGL);
    ImGui_ImplOpenGL3_Init("#version 400");
}

static bool showDebugWindow = false;

struct ImguiState
{
    bool showRopeStructure{false};
};

static void drawImGui(graphics::ApplicationGlfw & aApplication, DebugUI & aUI, ImguiState & aState)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    NewFrame();

    const ImGuiViewport * main_viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 20, main_viewport->WorkPos.y + 10), ImGuiCond_Once);
    Begin("Hello");
    if(ImGui::Button("Debug scenes"))
    {
        showDebugWindow = !showDebugWindow;
    }
    if(ImGui::Button("Game_pendulum"))
    {
        ChangeScene(GameList::GamePendulum, aApplication, aUI);
    }

    ImGui::Checkbox("Debug draw", &debugDrawer->mShown);

    ImGui::Checkbox("Show rope structure (not implemented)", &aState.showRopeStructure);
    End();

    Begin("Logging");
    {
        static const char* items[] = {
            "trace",
            "debug",
            "info",
            "warn",
            "err",
            "critical",
            "off",
        };

        spdlog::apply_all([&](std::shared_ptr<spdlog::logger> logger) 
            {
                // I suspect this is the default logger which we are not using.
                if (logger->name() == "") return;

                ImGui::Text(logger->name().c_str());

                // Let's just hope that this fmt::string_view does terminate data() with a null character.
                const char * current_item = spdlog::level::to_string_view(logger->level()).data();
                if (ImGui::BeginCombo(("##combo" + logger->name()).c_str(), current_item)) // The second parameter is the label previewed before opening the combo.
                {
                    for (int n = 0; n < IM_ARRAYSIZE(items); n++)
                    {
                        bool is_selected = (current_item == items[n]); // You can store your selection however you want, outside or inside your objects
                        if (ImGui::Selectable(items[n], is_selected))
                            // TODO should we check whether the level actually changed instead of setting it each frame?
                            // I suspect it would be trading memory barriers for conditional branching
                            logger->set_level(spdlog::level::from_str(items[n]));
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
                    }
                    ImGui::EndCombo();
                }
            });
    }
    End();

    if (showDebugWindow)
    {
        ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 20, main_viewport->WorkPos.y + 120), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(200.f, 300.f), ImGuiCond_Once);
        Begin("Debug scenes");
        if(ImGui::Button("Collision test"))
        {
            ChangeScene(GameList::CollisionTest, aApplication, aUI);
        }
        if(ImGui::Button("FrictionTest"))
        {
            ChangeScene(GameList::FrictionTest, aApplication, aUI);
        }
        if(ImGui::Button("SimpleCollisionTest"))
        {
            ChangeScene(GameList::SimpleCollisionTest, aApplication, aUI);
        }
        if(ImGui::Button("PivotTest"))
        {
            ChangeScene(GameList::PivotTest, aApplication, aUI);
        }
        if(ImGui::Button("SetPositionTest"))
        {
            ChangeScene(GameList::SetPositionTest, aApplication, aUI);
        }
        End();
    }
}

static void renderImGui()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
}

}
}
