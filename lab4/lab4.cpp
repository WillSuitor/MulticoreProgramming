#include "ts_queue.h"
#include "genome.h"
#include "ThreadSafeMultimap.h"
#include "futilityCounter.h"
#include "maze.hpp"
#include <random>
#include <iostream>
#include <ctime>
#include <thread>
#include <string>


void Mixer(ThreadSafeMultimap<int, Genome> population, ThreadSafeListenerQueue<Genome> * offspring){
	//Random device for choosing genomes to mix
	std::random_device rand;
	std::mt19937 eng(rand());
	std::uniform_int_distribution<> distr(0, population.size()-1);

	//The length of the array of moves for each genome
	int genomeSize = population[0].moves.size();

	//Random device for choosing splice point
	std::random_device randSp;
	std::mt19937 engSp(randSp());
	//Altered distribution to avoid splicing at very beginning or end
	std::uniform_int_distribution<> distrSp(1, genomeSize-2);

	while(1){
		//Get two random genomes from population
		int indA = distr(eng);
		Genome genA = population[indA];
		int indB = distr(eng);
		//Ensure they are two different genomes
		while(indA == indB)indB = distr(eng);
		Genome genB = population[indB];
		
		//Get the split point 
		int spliceIndex = distrSp(engSp);
		Genome spliced = Genome(genomeSize);
		for(int i = 0; i < spliceIndex; i++){
			spliced.moves[i] = genA.moves[i];
		}
		for(int i = spliceIndex; i < genomeSize; i++){
			spliced.moves[i] = genB.moves[i];
		}
		offspring->push(spliced);
	}
}

void Mutator(ThreadSafeMultimap<int, Genome> population, ThreadSafeListenerQueue<Genome> * offspring, FutilityCounter * futility, Maze * problem){
	//Random device for both moves and case probability
	std::random_device rand;
	std::mt19937 eng(rand());
	std::uniform_int_distribution<> distr(0, 4);

	//The length of the array of moves for each genome
	int genomeSize = population[0].moves.size();

	//Random device for mutating one move in a genome
	std::random_device randSp;
	std::mt19937 engSp(randSp());
	//Altered distribution to avoid splicing at very beginning or end
	std::uniform_int_distribution<> distrSp(0, genomeSize-1);


	int popSize = population.size();

	while(1){
		int fit = population[0].fitness(problem);
		Genome child (genomeSize);
		offspring->listen(child);
		//40% chance of further modification -> distr between 0-4
		switch(distr(eng))
		{
			case 0:
			case 1:
				child.moves[distrSp(engSp)] = distr(eng);
				break;
		}
		//add new solution then truncate the worst solution
		population.insert(child.fitness(problem), child);
		population.truncate(popSize);
		//update futility
		if(population[0].fitness(problem) < fit)futility->zero();
		else futility->up();

		//check if futility threshold is reached
		if(futility->get() == futility->threshold){
			//if reached, print maze, solution, finish, and abort
			std::cout << *problem << std::endl;
			std::cout << "Start (" << problem->getStart().col << ", " << problem->getStart().row << ")" << std::endl;
			std::cout << "Finish (" << problem->getFinish().col << ", " << problem->getFinish().row << ")" << std::endl;
			std::cout << "Best Fit: [";
			Genome best = population[0];
			int i = 0;
			for(i; i < genomeSize-1; i++){
				std::cout << best.moves[i] << ", ";
			}
			std::cout << best.moves[i] << "] " << std::endl;
			std::cout << "Fitness: " << best.fitness(problem) << std::endl;
			std::cout << "Reached Finish: " << best.finish(problem) << std::endl;
			std::abort();
		}
	}
}

int main(int argc, char ** argv){
	//Arguments
	int threadNum = -1;
	int threshold = -1;
	int rows = -1;
	int cols = -1;
	int genLength = -1;

	for(int i = 1; i < argc; i++){
		std::string argument = argv[i];
		//ensure arguments are integers
		if(i == 1){
			if(argument.find_first_not_of("0123456789") == std::string::npos) threadNum = std::stoi(argv[i]);
		}
		if(i == 2){
			if(argument.find_first_not_of("0123456789") == std::string::npos) threshold = std::stoi(argv[i]);
		}
		if(i == 3){
			if(argument.find_first_not_of("0123456789") == std::string::npos) rows = std::stoi(argv[i]);
		}
		if(i == 4){
			if(argument.find_first_not_of("0123456789") == std::string::npos) cols = std::stoi(argv[i]);
		}
		if(i == 5){
			if(argument.find_first_not_of("0123456789") == std::string::npos) genLength = std::stoi(argv[i]);
		}
	}

	//Admittedly hideous check to see that all the arguments are fulfilled
	if(threadNum < 2 || threshold == -1 || rows == -1 || cols == -1 || genLength == -1){
		std::cout << "Input must be formatted as: " << std::endl;
		std::cout << "./lab4 <number of total threads> <threshold g for determining completion> <rows> <cols> <genome length>";
		std::cout << std::endl;
		return 0;
	}

	ThreadSafeMultimap<int, Genome> population;

	ThreadSafeListenerQueue<Genome> offspring;

	FutilityCounter futility (threshold);

	std::srand(std::time(0));	
	Maze problem (rows, cols);

	//Random device for generating genomes
	std::random_device rand;
	std::mt19937 eng(rand());
	std::uniform_int_distribution<> distr(0, 4);

	//Start with random genomes
	for(int i = 0; i < 4*threadNum; i++){
		Genome gen (genLength);
		for(int j = 0; j < genLength; j++){
			gen.moves[j] = distr(eng);
		}
		population.insert(gen.fitness(&problem), gen);
	}

	std::thread threads[threadNum];
	for(int i = 0; i < threadNum; i++){
		//Spawn with a 50/50 chance of either being a mixer or mutator
		if(i%2==0){
			threads[i] = std::thread(Mutator, population, &offspring, &futility, &problem);
		}
		else{
			threads[i] = std::thread(Mixer, population, &offspring);
		}
	}

	for(int i = 0; i < threadNum; i++){
		threads[i].join();
	}
	return 0;
}