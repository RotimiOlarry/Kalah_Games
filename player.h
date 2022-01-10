#ifndef PLAYER_H
#define PLAYER_H

/*
 * Compile:
 *    make
 *
 * Usage:
 *    ./kalah -d depth -1 player1.txt -2 player2.txt
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

const string ALGORITHMS[2] = { "minimax", "alphabeta" };
const string EVALFUNCS[2]  = { "netscore", "myscore" };

typedef vector<char> Moves;

// Kalah Player 
class Player {
public:
	// Initialize Player
	Player ( char *f, int p );
	Player ( string a, string f, int p );

	// Who is playing?
	inline int Who ( ) { return(player); }

	// Take a player's turn
	int TakeTurn ( Game &game, int depth );

	// Record player move
	inline void RecordMove ( char mv ) { record.push_back(mv); }
	static inline int Records ( ) { return(record.size()); }
	static void Replay ( );
	static void Replay ( int player );

private:
	string algorithm;		// Strategy algorithm
	string evalfunc;		// Evaluation function
	int player;			// Player id

	static Moves record;		// Record of moves

	// Search into planning move is deep enough?
	inline int DEEP_ENOUGH ( int depth, int moves ) {
		// Step search if depth done or no moves available
		return((depth <= 0) || (moves == 0));
	}

	// Plan player's move
	void MOVE_GEN ( Game game, int depth, Moves &movements );

	// Plan player's move using Minimax strategy
	int MINMAX_AB ( Game &game, int depth, int player, Moves &movements );

	// Player has bonus move
	void BonusMove ( Game &game, int playing, Moves &trymoves );

	// Plan player's move using Alpha-Beta strategy
	int ALPHA_BETA_SEARCH ( Game &game, int depth, int alpha, int beta, int player, Moves &movements );

	// Ask player to move
	int AskPlayer ( Game &game );

	// Evaluate position in game
	int EvaluateGame ( Game &game, int playing );
};

#endif
