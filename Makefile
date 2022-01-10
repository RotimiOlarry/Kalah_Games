# Copyright Idleswell Software Creations, 2021
#
# 0.00 17Nov2021 AI Initial version
# 0.01 17Nov2021 AI kalah.cpp
# 0.02 17Nov2021 AI game.cpp
# 0.03 17Nov2021 AI player.cpp
# 0.04 23Nov2021 AI CXXFLAGS

CC = g++
#CXXFLAGS = -Wall
CXXFLAGS = -g -Wall

all: kalah

kalah: kalah.o game.o player.o
	$(CC) -o kalah kalah.o game.o player.o

kalah.o: kalah.cpp game.h player.h

game.o: game.cpp game.h

player.o: player.cpp player.h game.h

clean:
	rm -f *.o kalah
