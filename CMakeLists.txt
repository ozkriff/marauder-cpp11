cmake_minimum_required(VERSION 2.6)
project(marauder)

include_directories(.)

add_definitions(-Wold-style-cast -Wall -Wextra --pedantic)

option(use_cxx0x "Use '-std=c++0x' flag?")
option(use_libcxx "Use '-stdlib=libc++' flag?")

if (use_cxx0x)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

if (use_libcxx)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()


# ---------------------------------  CORE  ------------------------------------

include_directories("/usr/include/jsoncpp") # TODO: fix

add_library(marauderCore
  core/misc.cpp
  core/v2i.cpp
  core/pathfinder.cpp
  core/dir.cpp
  core/los.cpp
  core/unit.cpp
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
target_link_libraries(marauderCore
  jsoncpp # TODO: fix
)

configure_file(confCore.json . COPYONLY)
configure_file(unitTypes.json . COPYONLY)
configure_file(map.json . COPYONLY)
configure_file(scenario.json . COPYONLY)


# ---------------------------------  UI  --------------------------------------

find_package(OpenGL REQUIRED)
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)

include_directories(${SDL_INCLUDE_DIR})

add_library(marauderUi
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
)
target_link_libraries(marauderUi
  marauderCore
  ${OPENGL_LIBRARIES}
  ${SDL_LIBRARY}
  ${SDLIMAGE_LIBRARY}
)

configure_file(confUI.json . COPYONLY)
configure_file(unitResources.json . COPYONLY)


# --------------------------------  TESTS  ------------------------------------

add_executable(marauder ui/main.cpp)
target_link_libraries(marauder marauderUi)

add_executable(testMarauder
  tests/runner.cpp
  tests/testCoreDir.cpp
  tests/testCoreLos.cpp
)
target_link_libraries(testMarauder unittest++ marauderUi) # TODO: fix

enable_testing()
add_test(testMarauder testMarauder)

add_custom_target(runUnitTests ALL COMMAND testMarauder)
