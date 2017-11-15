CC = g++ -std=c++11
CFLAGS = -g -Wall
SRCS = Player.cpp main.cpp
PROG = player

OPENCV = `pkg-config opencv`
FFMPEG_1 = `pkg-config --cflags libavformat libavcodec libswresample libswscale libavutil`
SDL2 = `pkg-config --libs libavformat libavcodec libswresample libswscale libavutil sdl2`

LIBS = $(OPENCV)
LIBS := $(FFMPEG_1)
LIBS := $(SDL2)

$(PROG):$(SRCS) 
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)