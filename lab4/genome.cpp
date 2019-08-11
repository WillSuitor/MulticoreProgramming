#include "genome.h"
#include <stdlib.h>

//Intialize a genome to size size
Genome::Genome(int size){
	for(int i = 0; i < size; i++){
		moves.push_back(0);
	}
}

//Default constructor
Genome::Genome(){
	std::vector<int> k;
	moves = k;
}

/*
Calculates the fitness of the genome with respect to the maze. The fitness is determined by
the 'taxicab distance' from the ending point of the genome and the finish as well as the amount
of times it hits the barrier of the maze (ie attempts to move into a wall). The taxicab distance
is weighted more heavily (2:1) as it is more important that it reaches the end.

@param problem - the maze the genome is mapped to
@return the fitness of the genome
*/
int Genome::fitness(Maze * problem){
	int x = problem->getStart().col;
	int y = problem->getStart().row;
	int xEnd = problem->getFinish().col;
	int yEnd = problem->getFinish().row;

	int wallHits = 0;

	for(int i = 0; i < moves.size(); i++){
		int move = moves[i];
		if(move == 1){
			if(problem->get(y+1, x))wallHits++;
			else y++;
		}
		else if(move == 2){
			if(problem->get(y-1, x))wallHits++;
			else y--;
		}
		else if(move == 3){
			if(problem->get(y, x-1))wallHits++;
			else x--;
		}
		else if(move == 4){
			if(problem->get(y, x+1))wallHits++;
			else x++;
		}
	}
	int pointDist = abs(yEnd - y) + abs(xEnd - x);
	return 2*pointDist + wallHits;
}

/*
Tests to see if the genome finishes at the finish of the maze. Returns true if it 
does and false if not.

@returns true if it does reach the finish and false if not.
*/
bool Genome::finish(Maze * problem){
	int x = problem->getStart().col;
	int y = problem->getStart().row;
	int xEnd = problem->getFinish().col;
	int yEnd = problem->getFinish().row;


	for(int i = 0; i < moves.size(); i++){
		int move = moves[i];
		if(move == 1){
			if(!(problem->get(y+1, x))) y++;
		}
		else if(move == 2){
			if(!(problem->get(y-1, x)))y--;
		}
		else if(move == 3){
			if(!(problem->get(y, x-1))) x--;
		}
		else if(move == 4){
			if(!(problem->get(y, x+1)))x++;
		}
	}
	int pointDist = abs(yEnd - y) + abs(xEnd - x);
	return !pointDist;
}