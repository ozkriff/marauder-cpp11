CC=gcc
CFLAGS += -g
CFLAGS += -Wall -Wextra --pedantic
CFLAGS += -Werror
#CFLAGS += -pg
#CFLAGS=-O3
LDFLAGS += -lSDL -lSDL_image
LDFLAGS += -lGL -lGLU
LDFLAGS += -lm
all: rusty_armies
objects = game.o list.o obj.o misc.o va.o v2i.o v2f.o \
  v3f.o gl.o camera.o path.o dir.o math.o line2f.o
rusty_armies: $(objects)
	$(CC) $(CFLAGS) $(objects) $(LDFLAGS) -o rusty_armies
va.o: va.h
v2i.o: v2i.h
v2f.o: v2f.h
v3f.o: v3f.h
camera.o: bool.h list.h misc.h v2f.h v2i.h dir.h math.h \
  camera.h
dir.o: bool.h list.h misc.h v2i.h dir.h
game.o: bool.h list.h v2i.h v3f.h v2f.h math.h dir.h \
  line2f.h va.h obj.h misc.h camera.h path.h game.h gl.h
gl.o: bool.h gl.h list.h misc.h
line2f.o: v2f.h line2f.h
list.o: bool.h list.h
math.o: bool.h v2f.h
misc.o: list.h misc.h
obj.o: bool.h list.h v2f.h v3f.h math.h va.h obj.h misc.h
path.o: bool.h list.h v2i.h misc.h dir.h game.h
clean:
	rm rusty_armies $(objects) tags -f
