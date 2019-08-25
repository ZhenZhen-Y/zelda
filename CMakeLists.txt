cmake_minimum_required(VERSION 3.12)
project(zelda)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 REQUIRED network audio graphics window system)
if(SFML_FOUND)
    set(SFML_INCLUDE_DIR /usr/local/include)
    set(SFML_LIBRARIES sfml-network sfml-graphics sfml-window sfml-system sfml-audio)
    include_directories(${SFML_INCLUDE_DIR})
endif()

find_package(Boost)
find_package(Boost COMPONENTS system filesystem REQUIRED)
find_package(Lua REQUIRED)
add_subdirectory(src)
