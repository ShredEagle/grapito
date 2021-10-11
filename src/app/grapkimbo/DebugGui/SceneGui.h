#pragma once

#include "../TestScenes/SceneChanger.h"
#include "../Utils/DrawDebugStuff.h"

#include <engine/Application.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <cstdio>

namespace ad {
namespace grapito {

using namespace ImGui;


static void setupImGui(Application & aApplication)
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

static void drawImGui(Application & aApplication, DebugUI & aUI, ImguiState & aState)
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
