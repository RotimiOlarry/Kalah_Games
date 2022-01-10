
/*
 * Compile:
 *    g++ -Wall -o kalah kalah.cpp
 *
 * Usage:
 *    ./kalah -d depth -1 player1.txt -2 player2.txt
 */

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include <sys/time.h>
#include <sys/resource.h>
#include "game.h"
#include "player.h"

using namespace std;

// Correct usage
void Usage ( char *argv[] )
{
	cerr << "Usage: " << argv[0] << "-d depth -1 player1.txt -2 player2.txt" << endl;
}

typedef struct {
	int depth;			// Depth
	string algorithm1, evalfunc1;	// Player 1 options
	string algorithm2, evalfunc2;	// Player 2 options
} InputArgs;

typedef struct {
	int depth;			// Depth
	char *file1, *file2;		// Player description files
} CommandArgs;

// Play Kalay Game
class Kalah {
public:
	// Initialize Kalah game
	Kalah ( CommandArgs args );
	Kalah ( InputArgs args );

	// Play Kalah game
	void PlayKalah ( );

	// Output Kalah statistics
	void DispStats ( );

private:
	Player p1;			// Player 1
	Player p2;			// Player 2
	Game game;			// Game position
	int depth;			// Search depth

	int turn;			// Players take turns
	int win;			// Who won
	struct timeval starter;		// Start time

	// Complete initialization of Kalah game
	void KalahInit ( int d );
};

Kalah::Kalah ( CommandArgs args )
	: p1(args.file1,1), p2(args.file2,2), game()
{
	KalahInit(args.depth);
}

Kalah::Kalah ( InputArgs args )
	: p1(args.algorithm1,args.evalfunc1,1), p2(args.algorithm2,args.evalfunc2,2), game()
{
	KalahInit(args.depth);
}

// Complete initialization of Kalah game
void Kalah::KalahInit ( int d )
{
	depth = d;

	turn = 0;			// Starting
	win = 0;			// Nobody won

	struct timezone timezone;
	if (gettimeofday (&starter, &timezone)) {
		cerr << "ERROR: Unable to read system time." << endl;
	}
}

// Play Kalah game
void Kalah::PlayKalah ( )
{
	while (win <= 0) {		// Until game won
		cout << "Game before turn " << (turn+1) << endl;
		game.Display();

		Player *pl = ((turn++ % 2) == 0 ? &p1 : &p2);	// Player turn
		win = pl->TakeTurn (game, depth);
		if (win < 0) {		// Take a loss
			win = Opponent(pl->Who());
		}

		cout << "Game after turn " << turn << endl;
		game.Display();
		cout << endl;
	}
}

inline double dbltime ( struct timeval *tim ) {
	return(tim->tv_sec + tim->tv_usec/1000000.0);
}

// Elapsed time since start
double TimeSince ( struct timeval *starter )
{
	struct timeval nowtime;
	struct timezone timezone;

	if (gettimeofday (&nowtime, &timezone) != 0) {
		cerr << "ERROR: Unable to read system time." << endl;
	}
	return(dbltime(&nowtime) - dbltime(starter));
}

// How much memory did program use?
long MaxMemory ( )
{
	struct rusage resources;	// Resource usage report

	if (getrusage (RUSAGE_SELF, &resources) != 0) {
		cerr << "ERROR: Unable to read resources." << endl;
	}
	return(resources.ru_maxrss);	// Max. resident size
}

// Output Kalah statistics
void Kalah::DispStats ( )
{
	int myscore = game.Score(1);	// Watch for ties
	int oppscore = game.Score(2);
	cout << endl << "Game FINAL:" << endl;
	game.Display();

	if (myscore == oppscore) {
		cout << "The game was TIED!" << endl << endl;
	} else {
		cout << "Player " << win << " WINS!" << endl << endl;
	}
	Player::Replay();
	cout << endl;

	cout << "Game played " << turn << " turns." << endl;
	cout << "Game nodes generated: " << Game::Nodes() << endl;
	cout << "Game nodes expanded: " << (Game::Nodes() - (Player::Records()+1)) << endl;
	cout << "Game played in " << TimeSince(&starter) << " secs." << endl;
	cout << "Max. memory usage: " << MaxMemory() << "k" << endl;
}

// Reading arguments input by user
int PromptArguments ( InputArgs *inputs )
{
	do {
		cout << "What depth do you want the Kalah strategy to go? ";
		cin >> inputs->depth;
		if (!((inputs->depth == 2) || (inputs->depth == 4))) {
			cerr << "Depths supported are 2 or 4." << endl;
		}
	} while (!((inputs->depth == 2) || (inputs->depth == 4)));

	do {
		cout << "What is Player 1's search algorithm? " << endl;
		cin >> inputs->algorithm1;
		if (!((inputs->algorithm1 == ALGORITHMS[0]) || (inputs->algorithm1 == ALGORITHMS[1]))) {
			cerr << "Algorithms are: " << ALGORITHMS[0] << " or " << ALGORITHMS[1] << "." << endl;
		};
	} while (!((inputs->algorithm1 == ALGORITHMS[0]) || (inputs->algorithm1 == ALGORITHMS[1])));

	do {
		cout << "What is Player 1's evaluation function? " << endl;
		cin >> inputs->evalfunc1;
		if (!((inputs->evalfunc1 == EVALFUNCS[0]) || (inputs->evalfunc1 == EVALFUNCS[1]))) {
			cerr << "Evaluation functions are: " << EVALFUNCS[0] << " or " << EVALFUNCS[1] << "." << endl;
		};
	} while (!((inputs->evalfunc1 == EVALFUNCS[0]) || (inputs->evalfunc1 == EVALFUNCS[1])));

	do {
		cout << "What is Player 2's search algorithm? " << endl;
		cin >> inputs->algorithm2;
		if (!((inputs->algorithm2 == ALGORITHMS[0]) || (inputs->algorithm2 == ALGORITHMS[1]))) {
			cerr << "Algorithms are: " << ALGORITHMS[0] << " or " << ALGORITHMS[1] << "." << endl;
		};
	} while (!((inputs->algorithm2 == ALGORITHMS[0]) || (inputs->algorithm2 == ALGORITHMS[1])));

	do {
		cout << "What is Player 2's evaluation function? " << endl;
		cin >> inputs->evalfunc2;
		if (!((inputs->evalfunc2 == EVALFUNCS[0]) || (inputs->evalfunc2 == EVALFUNCS[1]))) {
			cerr << "Evaluation functions are: " << EVALFUNCS[0] << " or " << EVALFUNCS[1] << "." << endl;
		};
	} while (!((inputs->evalfunc2 == EVALFUNCS[0]) || (inputs->evalfunc2 == EVALFUNCS[1])));
	return(0);
}

// Reading arguments from the command line
int CommandArguments ( int argc, char *argv[], CommandArgs *cmdargs )
{
	int opt;

	cmdargs->depth = -1;		// No values
	cmdargs->file1 = cmdargs->file2 = NULL;

	while ((opt = getopt (argc, argv, "1:2:d:")) != -1) {
		switch (opt) {
		case '1':		// Player 1
			cmdargs->file1 = optarg;
			break;

		case '2':		// Player 2
			cmdargs->file2 = optarg;
			break;

		case 'd':		// Depth
			cmdargs->depth = atoi(optarg);
			break;

		default:
			cerr << "Unknown option " << opt << endl;
			return(1);
		}
	}

	if (!((cmdargs->depth == 2) || (cmdargs->depth == 4))) {
		cerr << "Depth " << cmdargs->depth << " must be 2 or 4." << endl;
		return(2);
        }

	if ((cmdargs->file1 == NULL) || (cmdargs->file2 == NULL)) {
		cerr << "Missing player description files." << endl;
		return(2);
	}
	return(0);
}

int main ( int argc, char *argv[] )
{
	Kalah *kalah;			// Kalah game

	if (argc == 1) {		// No arguments/
		InputArgs inputs;
		if (PromptArguments (&inputs)) {
			cerr << "ERROR: Illegal arguments input." << endl;
			return(1);
		}
		kalah = new Kalah(inputs);

	} else {			// Command line
		CommandArgs cmdargs;
		if (CommandArguments (argc, argv, &cmdargs)) {
			cerr << "ERROR: Illegal arguments in files." << endl;
			return(2);
		}
		kalah = new Kalah(cmdargs);
	}

	kalah->PlayKalah();		// Play Kalah game
	kalah->DispStats();		// Output statistics	
	return(0);
}
