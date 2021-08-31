set(TARGET_NAME grapkimbo)

set(${TARGET_NAME}_HEADERS
    Configuration.h
    DebugUI.h
    Game.h
    Game_pendulum.h
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
    Components/PlayerData.h
    Components/Position.h
    Components/VisualOutline.h
    Components/VisualRectangle.h
    Components/Body.h
    Components/AccelAndSpeed.h

    Systems/CameraGuidedControl.h
    Systems/Control.h
    Systems/ControlAnchorSight.h
    Systems/Control.h
    Systems/ContactConstraintCreation.h
    Systems/Gravity.h
    Systems/LevelGeneration.h
    Systems/PendulumSimulation.h
    Systems/Render.h
    Systems/AccelSolver.h
    Systems/ImpulseSolver.h
    Utils/CollisionBox.h
    Utils/DrawDebugStuff.h
)

set(${TARGET_NAME}_SOURCES
    main.cpp

    DebugUI.cpp
    Game.cpp
    Game_pendulum.cpp
    Input.cpp
    Entities.cpp

    Systems/CameraGuidedControl.cpp
    Systems/Control.cpp
    Systems/ControlAnchorSight.cpp
    Systems/ContactConstraintCreation.cpp
    Systems/Gravity.cpp
    Systems/LevelGeneration.cpp
    Systems/PendulumSimulation.cpp
    Systems/Render.cpp
    Systems/AccelSolver.cpp
    Systems/ImpulseSolver.cpp
    Utils/DrawDebugStuff.cpp
)

source_group(TREE ${CMAKE_CURRENT_LIST_DIR}
             FILES ${${TARGET_NAME}_HEADERS} ${${TARGET_NAME}_SOURCES}
)


find_package(Aunteater CONFIG REQUIRED COMPONENTS aunteater)
find_package(Graphics CONFIG REQUIRED COMPONENTS engine)
find_package(Math CONFIG REQUIRED COMPONENTS math)
find_package(Websocket CONFIG REQUIRED COMPONENTS websocket)

find_package(glad REQUIRED)


add_executable(${TARGET_NAME}
    ${${TARGET_NAME}_SOURCES}
    ${${TARGET_NAME}_HEADERS}
)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_options(${TARGET_NAME}
        PRIVATE "-fno-omit-frame-pointer" "-fsanitize=address" "-fsanitize=undefined" "-D KIMBO_DEBUG")
    target_link_options(${TARGET_NAME}
        PRIVATE "-fsanitize=address" "-fsanitize=undefined")
endif()

#target_compile_definitions(${TARGET_NAME} PRIVATE KIMBO_DEBUG)

target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${TARGET_NAME}
    PRIVATE
        ad::aunteater
        ad::engine
        ad::math
        ad::websocket

        glad::glad
)


set_target_properties(${TARGET_NAME} PROPERTIES
                      VERSION "${${PROJECT_NAME}_VERSION}"
)


##
## Install
##

install(TARGETS ${TARGET_NAME})
