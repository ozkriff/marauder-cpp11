cmake_minimum_required(VERSION 2.6)
project(marauder)
find_package(OpenGL REQUIRED)
find_package(SDL REQUIRED)
find_package(SDL_ttf REQUIRED)
find_package(SDL_image REQUIRED)
include_directories(.)
if(UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra --pedantic -std=c++0x")
endif()
add_library(SOURCES
	ui/game.cpp
	ui/objModel.cpp
	ui/vertexArray.cpp
	ui/v2f.cpp
	ui/v3f.cpp
	ui/gl.cpp
	ui/camera.cpp
	ui/eventVisualizer.cpp
	ui/math.cpp
	ui/event/eventMoveVisualizer.cpp
	ui/event/eventEndTurnVisualizer.cpp
	core/misc.cpp
	core/v2i.cpp
	core/pathfinder.cpp
	core/pathQueue.cpp
	core/dir.cpp
	core/los.cpp
	core/unitType.cpp
	core/event.cpp
	core/core.cpp
	core/math.cpp
	core/map.cpp
	core/event/eventMove.cpp
	core/event/eventEndTurn.cpp
)
add_executable(marauder ui/main.cpp)
set(LIBS
	${OPENGL_LIBRARIES}
	${SDL_LIBRARY}
	${SDLTTF_LIBRARY}
	${SDLIMAGE_LIBRARY}
)
target_link_libraries(marauder SOURCES ${LIBS})

add_executable(testMarauder
	tests/runner.cpp
	tests/testCoreDir.cpp
	tests/testCoreLos.cpp
)
target_link_libraries(testMarauder unittest++ SOURCES ${LIBS})
enable_testing()
add_test(testMarauder testMarauder)
add_custom_target(runUnitTests ALL COMMAND testMarauder)
