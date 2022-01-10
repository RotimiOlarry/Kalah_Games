
/*
 * Compile:
 *    make
 *
 * Usage:
 *    ./kalah -d depth -1 player1.txt -2 player2.txt
 */

#include <iostream>
#include <vector>
#include "game.h"

using namespace std;

char Hole1 ( int p ) { return(p == 1 ? 'f' : 'A'); }  // Hole 1 label
int Opponent ( int p ) { return(p == 1 ? 2 : 1); }  // Opponent
int OppHole ( int myhole ) { return(5 - myhole); }  // Opposite hole
int ChooseHole ( int playing, char hole ) {
	if (hole == ' ') return(-1);
	return(playing == 1 ? Hole1(1)-hole : 5-(hole-Hole1(2)));
}
char MoveHole ( int playing, int movement ) {
	return(playing == 1 ? Hole1(1)-movement : Hole1(2)+5-movement);
}

int Game::nodes = 0;			// Game nodes generated

// Init Kalah game
Game::Game ( )
{
	int s;
	for (s = 0; s < 6; s++) {
		holes[0][s] = holes[1][s] = INITSEEDS;  // Start seeds in holes
	}
	holes[0][6] = holes[1][6] = 0;	// No score to start
	nodes++;
}

// Init Kalah game with current state
Game::Game ( Game &b4 )
{
	int s;
	for (s = 0; s < 7; s++) {
		holes[0][s] = b4.Seeds(1,s);
		holes[1][s] = b4.Seeds(2,s);
	}
	nodes++;
}

// Gather all player's seeds at end
int Game::Gather ( int player )
{
	int gather = 0;
	int s;
	for (s = 0; s < 6; s++) {
		gather += EmptySeeds (player, s);  // Gather seeds out of hole
	}
	return(gather);
}

// Moves allowed for player at this stage of the game
int Game::MovesAllowed ( char allowed[], int playing )
{
	int moves = 0;			// Moves allowed
	int m;

	if (playing == 1) {		// Player 1
		for (m = 0; m < 6; m++) {
			if (Seeds(playing,m) > 0) {
				allowed[moves++] = Hole1(1)-m;
			}
		}

	} else {			// Player 2
		for (m = 5; m >= 0; m--) {
			if (Seeds(playing,m) > 0) {
				allowed[moves++] = Hole1(2)+5-m;
			}
		}
	}
	return(moves);
}

// Which is adjacent hole?
int AdjHole ( int hole, int *myside )
{
	int adjhole = hole+1;		// Adjacent shift 1 hole

	if (*myside) {			// If on own side of board
		if (adjhole > 6) {	// Crossing to other side
			adjhole = 0;
			*myside = 0;
		}

	} else {			//  On opponent's side
		if (adjhole > 5) {	// Skip opponent's score hole
			adjhole = 0;
			*myside = 1;
		}
	}
	return(adjhole);
}

// Player moves seeds from hole
int Game::KalahMove ( int playing, int movement )
{
	int seeds;			// Seeds to move
	int endpt = movement;		// Where move ends
	int myside = 1;			// Start on player's side
	int opp = Opponent(playing);

	if (movement < 0) return(endpt);	// No move

	seeds = EmptySeeds(playing, movement);  // Pickup seeds
	while (seeds-- > 0) {		// Until seeds distributed
		endpt = AdjHole (endpt, &myside);
		if (myside) {
			DropSeeds (playing, endpt, 1);
		} else {
			DropSeeds (opp, endpt, 1);
		}
	}

	// If player ends on an empty hole, 
	if (myside && (endpt != 6) && (Seeds(playing,endpt) == 1)) {
		// Then capture any seeds in opponent's opposite hole
		int oppseeds = Seeds(opp,OppHole(endpt));
		if (oppseeds > 0) {
			EmptySeeds(opp,OppHole(endpt));
			EmptySeeds(playing,endpt);
			Score(playing, oppseeds+1);
		}
	}

	//Display();			// Show game after move
	return(endpt);			// Tell player where move ends
}

// Display game board
void Game::Display ( )
{
	char alpha;
	int s;

	cout << "PLAYER 2" << endl;
	cout << Score(2);
	alpha = Hole1(2);
	for (s = 5; s >= 0; s--) { 
		cout << "\t" << alpha++ << "-" << Seeds(2,s);
	}
	cout << endl;

	alpha = Hole1(1);
	for (s = 0; s < 6; s++) { 
		cout << "\t" << alpha-- << "-" << Seeds(1,s);
	}
	cout << "\t" << Score(1) << endl;
	cout << "PLAYER 1" << endl;
}
