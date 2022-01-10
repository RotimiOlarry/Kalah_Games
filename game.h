#ifndef GAME_H
#define GAME_H

/*
 * Compile:
 *    make
 *
 * Usage:
 *    ./kalah -d depth -1 player1.txt -2 player2.txt
 */

using namespace std;

int Opponent ( int p );
int ChooseHole ( int playing, char hole );
char MoveHole ( int playing, int movement );

const int INITSEEDS = 6;		// Seeds to start in holes

// Kalah game position
class Game {
public:
	// Init Kalah game
	Game ( );
	Game ( Game &b4 );

	// How many games?
	inline static int Nodes ( ) { return(nodes); }

	inline int InfiniteScore ( ) { return((2*6*6)+1); }

	// Player's score well
	inline int Score ( int p ) { return(Seeds(p,6)); }
	inline int Score ( int p, int s ) { return(DropSeeds(p,6,s)); }

	// Moves allowed for player at this stage of the game
	int MovesAllowed ( char allowed[], int playing );

	// Player moves seeds from hole
	int KalahMove ( int playing, int movement );

	// Seeds in player's hole
	inline int Seeds ( int p, int h ) { return(holes[p-1][h]); }

	// Empty player's hole
	inline int EmptySeeds ( int p, int hole ) { 
		int dump = holes[p-1][hole];	// Save seeds
		holes[p-1][hole] = 0;		// No seeds left
		return(dump);			// Return seeds
	}

	// Drop seeds in player's hole
	inline int DropSeeds ( int p, int hole, int seeds ) {
		return(holes[p-1][hole] += seeds);	// Add seeds
	}

	// Evaluate game as net score of player p
	inline int NetScore ( int p ) { return(Score(p) - Score(Opponent(p))); }

	// Evaluate game as player's score
	inline int MyScore ( int player ) { return(Score(player)); }

	// Gather all player's seeds at end
	int Gather ( int player );

	// Display game board
	void Display ( );

private:
	int holes[2][7];		// Game holes for each player

	static int nodes;		// Game nodes generated
};

#endif
