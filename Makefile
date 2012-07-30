CC = gcc
#CC = tcc
#CC = clang
CFLAGS += -g
CFLAGS += -std=c89
CFLAGS += -Wall -Wextra --pedantic
CFLAGS += -Werror
#CFLAGS += -pg
#CFLAGS += -DNDEBUG
#CFLAGS += -O3
LDFLAGS += -lSDL -lSDL_image -lSDL_ttf
LDFLAGS += -lGL -lGLU
LDFLAGS += -lm
binname = rusty_armies
all: $(binname)
objects = game.o list.o obj.o misc.o va.o v2i.o v2f.o \
  v3f.o gl.o camera.o path.o dir.o math.o los.o \
  unit_type.o widgets.o
$(binname): $(objects)
	$(CC) $(CFLAGS) $(objects) $(LDFLAGS) -o $(binname)
va.o: va.h
v2i.o: bool.h v2i.h
v2f.o: v2f.h
v3f.o: v3f.h
camera.o: bool.h list.h misc.h v2f.h v2i.h dir.h math.h \
  camera.h
dir.o: bool.h list.h misc.h v2i.h dir.h
game.o: bool.h list.h v2i.h v3f.h v2f.h math.h dir.h \
  va.h obj.h misc.h camera.h game.h path.h gl.h los.h \
  config.h
gl.o: bool.h gl.h list.h misc.h
list.o: bool.h list.h
math.o: bool.h v2f.h
misc.o: list.h misc.h
obj.o: bool.h list.h v2i.h v2f.h v3f.h math.h va.h \
  obj.h misc.h
path.o: bool.h list.h v2i.h misc.h dir.h game.h
los.o: bool.h v2i.h v2f.h los.h math.h
unit_type.o: unit_type.h bool.h
widgets.o: widgets.h bool.h list.h v2i.h v2f.h math.h \
  config.h misc.h gl.h va.h
clean:
	rm $(binname) $(objects) tags -f
