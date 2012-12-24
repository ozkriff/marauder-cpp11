cmake_minimum_required(VERSION 2.6)
project(rusty_armies)
find_package(OpenGL REQUIRED)
find_package(SDL REQUIRED)
find_package(SDL_ttf REQUIRED)
find_package(SDL_image REQUIRED)
include_directories(.)
add_executable(rusty_armies
	ui_opengl/widgets.cpp
	ui_opengl/game.cpp
	ui_opengl/obj.cpp
	ui_opengl/vertex_array.cpp
	ui_opengl/v2f.cpp
	ui_opengl/v3f.cpp
	ui_opengl/gl.cpp
	ui_opengl/camera.cpp
	ui_opengl/event.cpp
	ui_opengl/math.cpp
	ui_opengl/event/move.cpp
	core/list.cpp
	core/misc.cpp
	core/v2i.cpp
	core/path.cpp
	core/dir.cpp
	core/los.cpp
	core/unit_type.cpp
	core/event.cpp
	core/core.cpp
	core/math.cpp
	core/event/move.cpp
	core/event/end_turn.cpp
)
target_link_libraries(
	rusty_armies
	${OPENGL_LIBRARIES}
	${SDL_LIBRARY}
	${SDLTTF_LIBRARY}
	${SDLIMAGE_LIBRARY}
)
