set(TARGET_NAME grapkimbo)

set(${TARGET_NAME}_HEADERS
    Game.h

    Components/Position.h
    Components/Controllable.h
    Systems/Render.h
    Systems/Control.h
)

set(${TARGET_NAME}_SOURCES
    Game.cpp
    Systems/Render.cpp
    Systems/Control.cpp
)

find_package(Aunteater CONFIG REQUIRED COMPONENTS aunteater)
find_package(Graphics CONFIG REQUIRED COMPONENTS engine)
find_package(Math CONFIG REQUIRED COMPONENTS math)
find_package(Websocket CONFIG REQUIRED COMPONENTS websocket)

find_package(glad REQUIRED)


add_executable(${TARGET_NAME}
    main.cpp
    ${${TARGET_NAME}_SOURCES}
    ${${TARGET_NAME}_HEADERS}
)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_options(${TARGET_NAME}
        PRIVATE "-fno-omit-frame-pointer" "-fsanitize=address" "-fsanitize=undefined")
    target_link_options(${TARGET_NAME}
        PRIVATE "-fsanitize=address" "-fsanitize=undefined")
endif()

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
