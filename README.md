# Kalah Games

Artificial intelligence!!

Implementation of the Kalah game making use of MINMAX-A-B (Rich &amp; Knight) and ALPHA-BETA-SEARCH (Russell &amp; Norvig) to select players moves at various depths. 

In artificial intelligence, game theory is a major yardstick to use to test the problem-solving capabilities of intelligent systems. 

This program is an implementation of the ancient Kalah game using use of two algorithms to play the game to check the optimality of both algorithms.

Depth, evaluation metrices and time complexity are used as factors. 

# To run the program 

On Linux:
  - Make use of the [make file](/https://github.com/RotimiOlarry/Kalah_Games/blob/master/Makefile) 
 
On Windows:
  - Step1:
    g++ -Wall player.cpp player.h game.h -o player.o

  - Step 2:
    g++ -Wall game.cpp game.h -o game.o

  - Step 3: 
    g++ -Wall kalah.cpp game.h player.h -o kalah.0

  - Step 4: 
    g++ kalah.o game.o player.o -o kalah

  - Final Step:
    ./kalah
