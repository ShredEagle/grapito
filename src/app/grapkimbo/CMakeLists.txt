set(TARGET_NAME grapkimbo)

set(${TARGET_NAME}_HEADERS
    commons.h
    Configuration.h
    DebugUI.h
    Game.h
    Input.h
    Entities.h
    Utilities.h

    Components/AccelAndSpeed.h
    Components/AnchorSelector.h
    Components/CameraGuide.h
    Components/CameraTag.h
    Components/Controllable.h
    Components/GrappleControl.h
    Components/Mass.h
    Components/Pendular.h
    Components/PivotJoint.h
    Components/PlayerData.h
    Components/Position.h
    Components/VisualOutline.h
    Components/VisualRectangle.h
    Components/Body.h
    Components/AccelAndSpeed.h
    Components/RopeCreator.h

    Systems/CameraGuidedControl.h
    Systems/Control.h
    Systems/ControlAnchorSight.h
    Systems/Control.h
    Systems/Gravity.h
    Systems/LevelGeneration.h
    Systems/PendulumSimulation.h
    Systems/Render.h
    Systems/AccelSolver.h
    Systems/Physics.h
    Systems/RopeCreation.h

    TestScenes/CollisionTest.h
    TestScenes/FrictionTest.h
    TestScenes/PivotTest.h
    TestScenes/SimpleCollisionTest.h
    TestScenes/SetPositionTest.h
    TestScenes/Game_pendulum.h
    TestScenes/SceneChanger.h

    Utils/CollisionBox.h
    Utils/DrawDebugStuff.h
    Utils/HomogeneousTransformation.h
    Utils/Shape.h
    Utils/PhysicsStructs.h

    DebugGui/SceneGui.h
)

set(${TARGET_NAME}_SOURCES
    main.cpp

    DebugUI.cpp
    Input.cpp
    Entities.cpp

    Systems/CameraGuidedControl.cpp
    Systems/Control.cpp
    Systems/ControlAnchorSight.cpp
    Systems/Gravity.cpp
    Systems/LevelGeneration.cpp
    Systems/PendulumSimulation.cpp
    Systems/Render.cpp
    Systems/AccelSolver.cpp
    Systems/Physics.cpp
    Systems/RopeCreation.cpp

    TestScenes/CollisionTest.cpp
    TestScenes/FrictionTest.cpp
    TestScenes/SimpleCollisionTest.cpp
    TestScenes/SetPositionTest.cpp
    TestScenes/PivotTest.cpp
    TestScenes/Game_pendulum.cpp

    Utils/CollisionBox.cpp
    Utils/DrawDebugStuff.cpp
    Utils/Shape.cpp
    Utils/PhysicsStructs.cpp
)

source_group(TREE ${CMAKE_CURRENT_LIST_DIR}
             FILES ${${TARGET_NAME}_HEADERS} ${${TARGET_NAME}_SOURCES}
)


find_package(Aunteater CONFIG REQUIRED COMPONENTS aunteater)
find_package(Graphics CONFIG REQUIRED COMPONENTS engine)
find_package(Math CONFIG REQUIRED COMPONENTS math)
find_package(Websocket CONFIG REQUIRED COMPONENTS websocket)

find_package(glad REQUIRED)
find_package(imgui REQUIRED)


add_executable(${TARGET_NAME}
    ${${TARGET_NAME}_SOURCES}
    ${${TARGET_NAME}_HEADERS}

    ${PROJECT_BINARY_DIR}/imgui_backends/imgui_impl_glfw.cpp
    ${PROJECT_BINARY_DIR}/imgui_backends/imgui_impl_opengl3.cpp
    ${PROJECT_BINARY_DIR}/imgui_backends/imgui_impl_glfw.h
    ${PROJECT_BINARY_DIR}/imgui_backends/imgui_impl_opengl3.h
    ${PROJECT_BINARY_DIR}/imgui_backends/imgui_impl_opengl3_loader.h
)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_options(${TARGET_NAME}
        PRIVATE "-O3")
    # target_link_options(${TARGET_NAME}
    #PRIVATE "-fsanitize=address" "-fsanitize=undefined")
endif()

#target_compile_definitions(${TARGET_NAME} PRIVATE KIMBO_DEBUG)

target_include_directories(${TARGET_NAME}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
            ${PROJECT_BINARY_DIR}/imgui_backends
)

target_link_libraries(${TARGET_NAME}
    PRIVATE
        ad::aunteater
        ad::engine
        ad::math
        ad::websocket

        glad::glad
        imgui::imgui
)


set_target_properties(${TARGET_NAME} PROPERTIES
                      VERSION "${${PROJECT_NAME}_VERSION}"
)


##
## Install
##

install(TARGETS ${TARGET_NAME})
