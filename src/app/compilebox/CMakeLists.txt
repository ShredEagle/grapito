set(TARGET_NAME grapito_compilebox)


set(${TARGET_NAME}_HEADERS
)

set(${TARGET_NAME}_SOURCES
)

source_group(TREE ${CMAKE_CURRENT_LIST_DIR}
             FILES ${${TARGET_NAME}_HEADERS} ${${TARGET_NAME}_SOURCES}
)


find_package(Aunteater CONFIG REQUIRED COMPONENTS aunteater)
find_package(Graphics CONFIG REQUIRED COMPONENTS arte graphics resource)
find_package(Math CONFIG REQUIRED COMPONENTS math)
find_package(Websocket CONFIG REQUIRED COMPONENTS websocket)

add_executable(${TARGET_NAME}
    ${${TARGET_NAME}_SOURCES}
    ${${TARGET_NAME}_HEADERS}
    main.cpp
)

target_include_directories(${TARGET_NAME}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${TARGET_NAME}
    PRIVATE
        ad::arte
        ad::aunteater
        ad::graphics
        ad::math
        ad::resource
        ad::websocket
)


set_target_properties(${TARGET_NAME} PROPERTIES
                      VERSION "${BUILD_VERSION}"
)


##
## Install
##

install(TARGETS ${TARGET_NAME})
