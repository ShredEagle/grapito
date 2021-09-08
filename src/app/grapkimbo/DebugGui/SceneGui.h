#pragma once

#include "TestScenes/SceneChanger.h"
#include "engine/Application.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

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

static void drawImGui(Application & aApplication)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    NewFrame();
    Begin("Hello world");
    if(ImGui::Button("Collision test"))
        ChangeScene(GameList::CollisionTest, aApplication);
    if(ImGui::Button("Game_pendulum"))
        ChangeScene(GameList::GamePendulum, aApplication);
    End();
}

static void renderImGui()
{
    Render();
    ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
}

}
}