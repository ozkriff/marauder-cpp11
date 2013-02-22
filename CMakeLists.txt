cmake_minimum_required(VERSION 2.6)
project(marauder)
find_package(OpenGL REQUIRED)
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
include_directories(.)
set(CMAKE_BUILD_TYPE Debug)
set(WARNINGS "-Wold-style-cast -Wall -Wextra --pedantic")
if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNINGS} -stdlib=libc++ -std=c++11")
endif()
include_directories("/usr/include/jsoncpp") # TODO: fix
configure_file(confCore.json . COPYONLY)
configure_file(unitTypes.json . COPYONLY)
configure_file(confUI.json . COPYONLY)
configure_file(unitResources.json . COPYONLY)
include_directories(${SDL_INCLUDE_DIR})
add_library(marauderLib
  ui/game.cpp
  ui/objModel.cpp
  ui/vertexArray.cpp
  ui/v2f.cpp
  ui/v3f.cpp
  ui/gl.cpp
  ui/camera.cpp
  ui/eventVisualizer.cpp
  ui/math.cpp
  ui/event/eventAttackVisualizer.cpp
  ui/event/eventMoveVisualizer.cpp
  ui/event/eventEndTurnVisualizer.cpp
  core/misc.cpp
  core/v2i.cpp
  core/pathfinder.cpp
  core/dir.cpp
  core/los.cpp
  core/unit.cpp
  core/unitType.cpp
  core/event.cpp
  core/core.cpp
  core/math.cpp
  core/map.cpp
  core/eventManager.cpp
  core/event/eventAttack.cpp
  core/event/eventMove.cpp
  core/event/eventEndTurn.cpp
  core/jsonHelpers.cpp
)
add_executable(marauder ui/main.cpp)
set(LIBS
  jsoncpp # TODO: fix
  ${OPENGL_LIBRARIES}
  ${SDL_LIBRARY}
  ${SDLIMAGE_LIBRARY}
)
target_link_libraries(marauder marauderLib ${LIBS})

add_executable(testMarauder
  tests/runner.cpp
  tests/testCoreDir.cpp
  tests/testCoreLos.cpp
)
target_link_libraries(testMarauder unittest++ marauderLib ${LIBS}) # TODO: fix
enable_testing()
add_test(testMarauder testMarauder)
add_custom_target(runUnitTests ALL COMMAND testMarauder)
