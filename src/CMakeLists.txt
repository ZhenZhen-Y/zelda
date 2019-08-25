cmake_minimum_required(VERSION 3.14)
set(EXECUTABLE_NAME "zelda")

#Box2D
include_directories(./ ../thirdparty ${Boost_INCLUDE_DIRS})
file(GLOB_RECURSE BOX2D_SOURCES  ../thirdparty/Box2D/*.cpp)

add_executable(${EXECUTABLE_NAME} ${BOX2D_SOURCES} main.cpp input.cpp input.h components.h utils.cpp utils.h
        sprite.cpp sprite.h common.h resource.cpp resource.h ../thirdparty/misc/SFMLDebugDraw.cpp ../thirdparty/misc/SFMLDebugDraw.h VertexArray.h tilemap.cpp tilemap.h combat.cpp combat.h action.cpp action.h ContactListener.h ui/ui.cpp ui/ui.h ../thirdparty/misc/SFMLRoundedRectangle.cpp ../thirdparty/misc/SFMLRoundedRectangle.h ui/UIState.h ui/DialogueUI.cpp ui/DialogueUI.h ui/style.h ui/MessageUI.cpp ui/MessageUI.h ui/PackUI.cpp ui/PackUI.h scripts.cpp scripts.h config.h graphic.cpp graphic.h)
add_definitions(-std=c++17)
target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${LUA_LIBRARIES})
