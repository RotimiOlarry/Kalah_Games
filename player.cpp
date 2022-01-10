
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
#include "game.h"
#include "player.h"

using namespace std;

Moves Player::record;			// Player move record

// Player initialization
Player::Player ( char *file, int p )
{
	algorithm = ALGORITHMS[0];	// Default algorithm: minimax
	evalfunc = EVALFUNCS[0];	// Default evaluation function
	player = p;			// Player id

	ifstream playfile(file);	// Open player description
	if (!playfile) return;

	string cat, val;		// category value
	playfile >> cat >> val;
	while (!playfile.eof()) {	// Read player factors
		if (cat == "algorithm") {
			if (!((val == ALGORITHMS[0]) || (val == ALGORITHMS[1]))) {
				cerr << cat << " " << val << endl;
			} else {
				algorithm = val;
			}

		} else if (cat == "evalfunc") {
			if (!((val == EVALFUNCS[0]) || (val == EVALFUNCS[1]))) {
				cerr << cat << " " << val << endl;
			} else {
				evalfunc = val;
			}

		} else {
			cerr << "Unknown category: " << cat << " " << val << endl;
		}
		playfile >> cat >> val;
	}
	cout << "Player " << player << ": algorithm: " << algorithm << " evalfunc: " << evalfunc << endl;
}

// Initialize Player
Player::Player ( string a, string f, int p )
{
	player = p;
	algorithm = a;
	evalfunc = f;
	cout << "Player " << player << ": algorithm: " << algorithm << " evalfunc: " << evalfunc << endl;
}

// Take a player's turn
int Player::TakeTurn ( Game &game, int depth )
{
	Moves movements;
	size_t m;

	MOVE_GEN (game, depth, movements);
	size_t moves = movements.size();
	for (m = 0; m < moves; m++) {
		RecordMove(movements[m]);
		cout << "Player " << player << ": moves " << movements[m] << endl;
		game.KalahMove (player, ChooseHole (player, movements[m]));
	}

	if (moves > 0) return(0);	// If moved, then game not won

	int opp = Opponent(player);
	game.Score(opp, game.Gather(opp));  // Opponent gets seeds

	int myscore = game.Score(player);  // Who won?
	int oppscore = game.Score(opp);
	return (myscore >= oppscore ? player : -player);
}

// Plan player's move
void Player::MOVE_GEN ( Game game, int depth, Moves &movements )
{
	if (algorithm == ALGORITHMS[0]) {	// Minimax 
		MINMAX_AB (game, depth, player, movements);
	} else if (algorithm == ALGORITHMS[1]) {  // Alpha-beta
		int inf = game.InfiniteScore();
		ALPHA_BETA_SEARCH (game, depth, -inf, inf, player, movements);
	}
}

// Plan player's move using Minimax strategy
int Player::MINMAX_AB ( Game &game, int depth, int playing, Moves &movements )
{
	char allowed[6];		// Moves allowed
	size_t moves = game.MovesAllowed (allowed, playing);
	size_t b, m;

	if (DEEP_ENOUGH(depth, moves)) {	// Search depth done
		return(EvaluateGame (game, playing));
	}

	if (playing == player) {	// Maximizing player
		int minimax = -game.InfiniteScore();
		for (m = 0; m < moves; m++) {
			Game moved(game);	// Game with move
			Moves trymoves;
			trymoves.push_back(allowed[m]);
			int choice = ChooseHole (playing, allowed[m]);  // Try move

			int endpt = moved.KalahMove (playing, choice);
			if (endpt == 6) {	// Bonus moves
				BonusMove (moved, playing, trymoves);
				for (b = 1; b < trymoves.size(); b++) {
					int bonchoice = ChooseHole (playing, trymoves[b]);
					moved.KalahMove (playing, bonchoice);
				}
			}

			Moves oppmove;		// Opponent's move
			int moveval = MINMAX_AB (moved, depth-1, Opponent(playing), oppmove);
			if (moveval > minimax) {  // Best move
				minimax = moveval;
				movements = trymoves;
			}
		}
		return(minimax);

	} else {			// Minimizing (other) player
		int minimax = +game.InfiniteScore();
		for (m = 0; m < moves; m++) {
			Game moved(game);	// Game with move
			Moves trymoves;
			trymoves.push_back(allowed[m]);
			int choice = ChooseHole (playing, allowed[m]);  // Try move

			int endpt = moved.KalahMove (playing, choice);
			if (endpt == 6) {	// Bonus moves
				BonusMove (moved, playing, trymoves);
				for (b = 1; b < trymoves.size(); b++) {
					int bonchoice = ChooseHole (playing, trymoves[b]);
					moved.KalahMove (playing, bonchoice);
				}
			}

			Moves oppmove;		// Opponent's move
			int moveval = MINMAX_AB (moved, depth-1, Opponent(playing), oppmove);
			if (moveval < minimax) {  // Best defence
				minimax = moveval;
				movements = trymoves;
			}
		}
		return(minimax);
	}
}

// Select bonus move for player
void Player::BonusMove ( Game &game, int playing, Moves &trymoves )
{
	char allowed[6];		// Moves allowed
	size_t moves = game.MovesAllowed (allowed, playing);
	Moves bonus;
	size_t b, m;

	if (moves == 0) return;		// No bonus moves

	if (playing == player ) {
		int bonval = -game.InfiniteScore();

		for (m = 0; m < moves; m++) {
			Game bongame(game);
			Moves bonmoves;
			bonmoves.push_back(allowed[m]);
			int choice = ChooseHole (playing, allowed[m]);  // Try move

			int endpt = bongame.KalahMove (playing, choice);
			if (endpt == 6) {	// Bonus move
				BonusMove (bongame, playing, bonmoves);
				for (b = 1; b < bonmoves.size(); b++) {
					int bonchoice = ChooseHole (playing, bonmoves[b]);
					bongame.KalahMove (playing, bonchoice);
				}
			}

			int evalgame = EvaluateGame (bongame, playing);
			if (evalgame > bonval) {  // Best move
				bonval = evalgame;
				bonus = bonmoves;
			}
		}

	} else {
		int bonval = +game.InfiniteScore();

		for (m = 0; m < moves; m++) {
			Game bongame(game);
			Moves bonmoves;
			bonmoves.push_back(allowed[m]);
			int choice = ChooseHole (playing, allowed[m]);  // Try move

			int endpt = bongame.KalahMove (playing, choice);
			if (endpt == 6) {	// Bonus move
				BonusMove (bongame, playing, bonmoves);
				for (b = 1; b < bonmoves.size(); b++) {
					int bonchoice = ChooseHole (playing, bonmoves[b]);
					bongame.KalahMove (playing, bonchoice);
				}
			}

			int evalgame = EvaluateGame (bongame, playing);
			if (evalgame < bonval) {  // Best move
				bonval = evalgame;
				bonus = bonmoves;
			}
		}
	}

	for (b = 0; b < bonus.size(); b++) {
		trymoves.push_back(bonus[b]);
	}
}

// Plan player's move using Alpha-Beta strategy
int Player::ALPHA_BETA_SEARCH ( Game &game, int depth, int alpha, int beta, int playing, Moves &movements )
{
	char allowed[6];		// Moves allowed
	size_t moves = game.MovesAllowed (allowed, playing);
	size_t b, m;

	if (DEEP_ENOUGH (depth, moves)) {	// Search depth done
		return(EvaluateGame (game, playing));
	}

	if (playing == player) {	// Maximizing player
		int alphabeta = -game.InfiniteScore();
		for (m = 0; m < moves; m++) {
			Game moved(game);	// Game with move
			Moves trymoves;
			trymoves.push_back(allowed[m]);
			int choice = ChooseHole (playing, allowed[m]);  // Try move

			int endpt = moved.KalahMove (playing, choice);
			if (endpt == 6) {	// Bonus moves
				BonusMove (moved, playing, trymoves);
				for (b = 1; b < trymoves.size(); b++) {
					int bonchoice = ChooseHole (playing, trymoves[b]);
					moved.KalahMove (playing, bonchoice);
				}
			}

			Moves oppmove;		// Opponent's move
			int moveval = ALPHA_BETA_SEARCH (moved, depth-1, alpha, beta, Opponent(playing), oppmove);
			if (moveval > alphabeta) {  // Best move
				alphabeta = moveval;
				movements = trymoves;
			}
			if (moveval > alpha) {	// Max(alpha, moveval)
				alpha = moveval;
			}
			if (moveval >= beta) break;  // Cutoff
		}
		return(alphabeta);

	} else {			// Minimizing (other) player
		int alphabeta = +game.InfiniteScore();
		for (m = 0; m < moves; m++) {
			Game moved(game);	// Game with move
			Moves trymoves;
			trymoves.push_back(allowed[m]);
			int choice = ChooseHole (playing, allowed[m]);  // Try move

			int endpt = moved.KalahMove (playing, choice);
			if (endpt == 6) {	// Bonus moves
				BonusMove (moved, playing, trymoves);
				for (b = 1; b < trymoves.size(); b++) {
					int bonchoice = ChooseHole (playing, trymoves[b]);
					moved.KalahMove (playing, bonchoice);
				}
			}

			Moves oppmove;		// Opponent's move
			int moveval = ALPHA_BETA_SEARCH (moved, depth-1, alpha, beta, Opponent(playing), oppmove);
			if (moveval < alphabeta) {  // Best defence
				alphabeta = moveval;
				movements = trymoves;
			}
			if (moveval < beta) {	// Min(beta, moveval)
				beta = moveval;
			}
			if (moveval <= alpha) break;  // Cutoff
		}
		return(alphabeta);
	}
}

// Ask player to move
int Player::AskPlayer ( Game &game )
{
	char allowed[6];		// Moves allowed
	int moves = game.MovesAllowed (allowed, player);

	if (moves <= 0) return(-1);	// No moves

	int choice = -1;
	do {
		cout << "Player " << player << ": Select hole to move:";
		int m;
		for (m = 0; m < moves; m++) {
			cout << " " << allowed[m];
		}
		cout << " -> ";

		char hole;		// Hole to move
		cin >> hole;
		for (m = 0; m < moves; m++) {
			if (allowed[m] == hole) {
				choice = ChooseHole (player, hole);
				break;
			}
		}
	} while (choice < 0);
	return(choice);			// Move chosen
}

// Evaluate position in game
int Player::EvaluateGame ( Game &game, int playing )
{
	if (evalfunc == EVALFUNCS[0]) {	// Evaluate by net score
		return(game.NetScore(playing));
	} else if (evalfunc == EVALFUNCS[1]) {  // Evaluate by player's score
		return(game.MyScore(playing));
	}
	return(-1);
}

// Replay game moves
void Player::Replay ( )
{
	int p;
        cout << "Game moves:" << endl;
	for (p = 1; p <= 2; p++) {
		Replay(p);
	}
}

// Show char or skip with ' '
char RecChar ( int player, char rech )
{
	if (player == 1) {
		return(islower(rech) ? rech : ' ');
	} else {
		return(isupper(rech) ? rech : ' ');
	}
}

void Player::Replay ( int player )
{
        size_t movements = record.size();
	size_t r;

	cout << player << ": ";
	for (r = 0; r < movements; r++) {
		cout << RecChar(player,record[r]);
	}
	cout << endl;
}
