cmake_minimum_required(VERSION 3.16)

project(Grapito VERSION "${BUILD_VERSION}")

include(cmake/include.cmake)

cmc_include_conan_configuration()

option(BUILD_Grapito_RELEASEMODE
       "Build Grapito application for public release."
       TRUE)

add_subdirectory(src)
