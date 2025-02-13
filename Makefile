CXX = g++
ROOT_DIR := $(shell pwd)
CFLAGS = -I$(ROOT_DIR)/include -I$(ROOT_DIR)/include/imgui -I$(ROOT_DIR)/include/imgui/backends -I$(ROOT_DIR)/include/SDL -I$(ROOT_DIR)/include/ffmpeg

# CFLAGS = -Iinclude -Iinclude/imgui -Iinclude/imgui/backends -Iinclude/SDL -Iinclude/ffmpeg
LDFLAGS = -Llib -lSDL -lavcodec -lavformat -lswscale -lswresample -lavutil

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    LDFLAGS += -framework OpenGL
else ifeq ($(UNAME_S),Windows_NT)
    LDFLAGS += -lopengl32 -lgdi32 -static-libgcc -static-libstdc++
endif

all: video_player

video_player: main.cpp
    $(CXX) -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
    rm -f video_player