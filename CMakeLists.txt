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
	ui/widgets.cpp
	ui/game.cpp
	ui/obj.cpp
	ui/vertex_array.cpp
	ui/v2f.cpp
	ui/v3f.cpp
	ui/gl.cpp
	ui/camera.cpp
	ui/event.cpp
	ui/math.cpp
	ui/event/move.cpp
	core/misc.cpp
	core/v2i.cpp
	core/path.cpp
	core/pathQueue.cpp
	core/dir.cpp
	core/los.cpp
	core/unit_type.cpp
	core/event.cpp
	core/core.cpp
	core/math.cpp
	core/map.cpp
	core/event/move.cpp
	core/event/end_turn.cpp
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
