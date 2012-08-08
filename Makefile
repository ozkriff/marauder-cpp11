# TODO make libcore.o and libui_opengl.o
CC = gcc
#CC = tcc
#CC = clang
CFLAGS += -I.
CFLAGS += -g
CFLAGS += -std=c89
CFLAGS += -Wall -Wextra --pedantic
#CFLAGS += -Werror
#CFLAGS += -pg
#CFLAGS += -DNDEBUG
#CFLAGS += -O3
LDFLAGS += -lSDL -lSDL_image -lSDL_ttf
LDFLAGS += -lGL -lGLU
LDFLAGS += -lm
binname = rusty_armies
all: $(binname)
ui_opengl_objects = \
  ui_opengl/game.o \
  ui_opengl/obj.o \
  ui_opengl/vertex_array.o \
  ui_opengl/v2f.o \
  ui_opengl/v3f.o \
  ui_opengl/gl.o \
  ui_opengl/camera.o \
  ui_opengl/widgets.o \
  ui_opengl/event.o \
  ui_opengl/math.o \
  ui_opengl/event/move.o
core_objects = \
  core/list.o \
  core/misc.o \
  core/v2i.o \
  core/path.o \
  core/dir.o \
  core/los.o \
  core/unit_type.o \
  core/event.o \
  core/core.o \
  core/math.o \
  core/event/move.o \
  core/event/end_turn.o
libs = $(core_objects) $(ui_opengl_objects)
flags = $(CFLAGS) $(LDFLAGS)
$(binname): $(core_objects) $(ui_opengl_objects)
	$(CC) $(libs) $(flags) -o $(binname)
clean:
	rm $(binname) $(core_objects) $(ui_opengl_objects) tags -f
# Autogenerated:
ui_opengl/game.o: \
  core/bool.h core/list.h core/v2i.h core/math.h core/dir.h \
  core/misc.h core/unit_type.h core/core.h core/event/move.h \
  core/event/end_turn.h core/event.h core/core_private.h \
  core/path.h core/los.h ui_opengl/v3f.h ui_opengl/v2f.h \
  ui_opengl/math.h ui_opengl/vertex_array.h ui_opengl/obj.h \
  ui_opengl/camera.h ui_opengl/game.h ui_opengl/gl.h \
  ui_opengl/widgets.h ui_opengl/event/move.h \
  ui_opengl/event.h ui_opengl/config.h
ui_opengl/obj.o: \
  core/bool.h core/list.h core/v2i.h core/dir.h core/misc.h \
  ui_opengl/v2f.h ui_opengl/v3f.h ui_opengl/math.h \
  ui_opengl/vertex_array.h ui_opengl/obj.h
ui_opengl/vertex_array.o: \
  ui_opengl/vertex_array.h
ui_opengl/v2f.o: \
  ui_opengl/v2f.h
ui_opengl/v3f.o: \
  ui_opengl/v3f.h
ui_opengl/gl.o: \
  core/bool.h core/list.h core/misc.h ui_opengl/gl.h
ui_opengl/camera.o: \
  core/bool.h core/list.h core/v2i.h core/misc.h core/dir.h \
  ui_opengl/v2f.h ui_opengl/math.h ui_opengl/camera.h
ui_opengl/widgets.o: \
  core/bool.h core/list.h core/v2i.h core/dir.h core/misc.h \
  ui_opengl/v2f.h ui_opengl/math.h ui_opengl/gl.h \
  ui_opengl/vertex_array.h ui_opengl/widgets.h \
  ui_opengl/config.h
ui_opengl/event.o: \
  core/bool.h core/list.h core/v2i.h core/dir.h core/core.h \
  core/event/move.h core/event/end_turn.h core/misc.h \
  core/event.h core/core_private.h ui_opengl/v2f.h \
  ui_opengl/v3f.h ui_opengl/math.h ui_opengl/vertex_array.h \
  ui_opengl/event/move.h
ui_opengl/math.o: \
  core/bool.h core/v2i.h core/dir.h ui_opengl/v2f.h \
  ui_opengl/math.h
ui_opengl/event/move.o: \
  core/bool.h core/list.h core/misc.h core/v2i.h core/dir.h \
  core/unit_type.h core/core.h core/event/move.h \
  core/event/end_turn.h core/event.h core/path.h \
  core/core_private.h ui_opengl/v2f.h \
  ui_opengl/vertex_array.h ui_opengl/game_private.h
core/list.o: \
  core/bool.h core/list.h core/misc.h
core/misc.o: \
  core/list.h core/misc.h
core/v2i.o: \
  core/bool.h core/v2i.h
core/path.o: \
  core/bool.h core/list.h core/v2i.h core/misc.h core/dir.h \
  core/core.h core/unit_type.h
core/dir.o: \
  core/bool.h core/list.h core/misc.h core/v2i.h core/dir.h
core/los.o: \
  core/bool.h core/v2i.h core/los.h core/math.h
core/unit_type.o: \
  core/unit_type.h
core/event.o: \
  core/bool.h core/list.h core/v2i.h core/dir.h core/misc.h \
  core/unit_type.h core/core.h core/event/move.h \
  core/event/end_turn.h core/event.h core/path.h core/los.h \
  core/core_private.h
core/core.o: \
  core/bool.h core/list.h core/v2i.h core/math.h core/dir.h \
  core/misc.h core/unit_type.h core/core.h core/event/move.h \
  core/event/end_turn.h core/event.h core/path.h core/los.h \
  core/core_private.h
core/math.o: \
  core/bool.h core/v2i.h core/math.h
core/event/move.o: \
  core/bool.h core/list.h core/misc.h core/v2i.h core/dir.h \
  core/unit_type.h core/core.h core/event/move.h \
  core/event/end_turn.h core/event.h core/path.h \
  core/core_private.h
core/event/end_turn.o: \
  core/bool.h core/list.h core/misc.h core/v2i.h core/dir.h \
  core/unit_type.h core/core.h core/event/end_turn.h \
  core/event/move.h core/event.h core/path.h \
  core/core_private.h
