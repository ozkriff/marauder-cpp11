cmake_minimum_required(VERSION 2.6)
project(marauder)

include_directories(.)

add_definitions(-Wold-style-cast -Wall -Wextra --pedantic)

option(use_libcxx "Use '-stdlib=libc++' flag?")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if (use_libcxx)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()


# ---------------------------------  CORE  ------------------------------------

include_directories("/usr/include/jsoncpp") # TODO: fix

add_library(marauderCore
  core/misc.cpp
  core/v2i.cpp
  core/pathfinder.cpp
  core/direction.cpp
  core/lineOfSight.cpp
  core/unit.cpp
  core/event.cpp
  core/eventView.cpp
  core/core.cpp
  core/math.cpp
  core/map.cpp
  core/eventManager.cpp
  core/event/eventAttack.cpp
  core/event/eventMove.cpp
  core/event/eventEndTurn.cpp
  core/jsonHelpers.cpp
  core/command.cpp
  core/command/commandAttack.cpp
  core/command/commandMove.cpp
  core/command/commandEndTurn.cpp
)
target_link_libraries(marauderCore
  jsoncpp # TODO: fix
)

configure_file(confCore.json . COPYONLY)
configure_file(unitTypes.json . COPYONLY)
configure_file(map.json . COPYONLY)
configure_file(scenario.json . COPYONLY)


# -----------------------------  VISUALIZER  ----------------------------------

find_package(OpenGL REQUIRED)
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)

include_directories(${SDL_INCLUDE_DIR})

add_library(marauderVisualizer
  visualizer/color4f.cpp
  visualizer/color3u.cpp
  visualizer/visualizer.cpp
  visualizer/objModel.cpp
  visualizer/mesh.cpp
  visualizer/v2f.cpp
  visualizer/v3f.cpp
  visualizer/texture.cpp
  visualizer/camera.cpp
  visualizer/eventVisualizer.cpp
  visualizer/math.cpp
  visualizer/meshBuilders.cpp
  visualizer/event/eventAttackVisualizer.cpp
  visualizer/event/eventMoveVisualizer.cpp
  visualizer/event/eventEndTurnVisualizer.cpp
  visualizer/sceneManager.cpp
  visualizer/sceneNode.cpp
  visualizer/tilePicker.cpp
)
target_link_libraries(marauderVisualizer
  marauderCore
  ${OPENGL_LIBRARIES}
  ${SDL_LIBRARY}
  ${SDLIMAGE_LIBRARY}
)

configure_file(confVisualizer.json . COPYONLY)
configure_file(unitResources.json . COPYONLY)


# --------------------------------  TESTS  ------------------------------------

add_executable(marauder visualizer/main.cpp)
target_link_libraries(marauder marauderVisualizer)

add_executable(testMarauder
  tests/runner.cpp
  tests/testCoreDir.cpp
  tests/testCoreLineOfSight.cpp
)
target_link_libraries(testMarauder unittest++ marauderVisualizer) # TODO: fix

enable_testing()
add_test(testMarauder testMarauder)

add_custom_target(runUnitTests ALL COMMAND testMarauder)
