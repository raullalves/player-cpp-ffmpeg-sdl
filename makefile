CC = g++ -std=c++11
CFLAGS = -g -Wall
SRCS = Player.cpp main.cpp
PROG = player

LIBS_CONFIG = `pkg-config --libs libavformat libavcodec libswresample libswscale libavutil sdl2`

LIBS := $(LIBS_CONFIG)

$(PROG):$(SRCS) 
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)