#ifndef GENOME_H_
#define GENOME_H_
#include "maze.hpp"
#include <vector>

class Genome{
public:
	//The container for the list of moves (0-4 represents no movement, up, down, left, right respectively)
	std::vector<int> moves;

	//Default constructor
	Genome();

	//Intialize a genome to size size	
	Genome(int size);

	/*
	Calculates the fitness of the genome with respect to the maze. The fitness is determined by
	the 'taxicab distance' from the ending point of the genome and the finish as well as the amount
	of times it hits the barrier of the maze (ie attempts to move into a wall). The taxicab distance
	is weighted more heavily (2:1) as it is more important that it reaches the end.

	@param problem - the maze the genome is mapped tailor
	@return the fitness of the genome
	*/
	int fitness(Maze * problem);

	/*
	Tests to see if the genome finishes at the finish of the maze. Returns true if it 
	does and false if not.

	@returns true if it does reach the finish and false if not.
	*/
	bool finish(Maze * problem);
};

#endif