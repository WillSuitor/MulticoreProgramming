#include <utility>
#include <string> 
#include <random>
#include <thread>
#include <iostream>
#include <vector>
#include "worker.h"
#include "ts_queue.h"

int main(int argc, char ** argv){

	//The initial points to fit the polynomial to
	std::vector< std::pair<float, float> > INIT_POINTS;
	
	//the degree of the polynomial
	int DEGREE = -1;

	//ideal fit setting
	float IDEAL_FIT = 1;

	//the number of worker threads spawned
	int wThreadNum = -1;

	//Determines whether the polynomial is defined by x or y
	bool xNotY = true;

	std::string tLC = "-t"; std::string tUC = "-T";
	std::string dLC = "-d"; std::string dUC = "-D";

	for(int i = 1; i < argc-1; i++){
		std::string next = argv[i+1];
		if(dLC.compare(argv[i]) == 0 || dUC.compare(argv[i]) == 0){
			if(next.find_first_not_of("0123456789") == std::string::npos) DEGREE = std::stoi(argv[i+1]);
		}
		else if(tLC.compare(argv[i]) == 0 || tUC.compare(argv[i]) == 0){
			if(next.find_first_not_of("0123456789") == std::string::npos) wThreadNum = std::stoi(argv[i+1]);
		}
	}

	if(wThreadNum == -1 || DEGREE == -1){
		std::cout << "Arguments must be formatted as: " << std::endl;
		std::cout << "-d <Degree of Polynomial> -w <Number of worker threads>" << std::endl;
		return 0;
	}

	//array of worker threads
	std::thread workerThreads[wThreadNum];

	//Random generator for the points
	std::random_device rand;
	std::mt19937 eng(rand());
	std::uniform_int_distribution<> distr(-5, 5);

	/*
	There cannot be a set of initial points in which two points share an x and two other points
	share a y. This is because the only way to link them would be disjoint lines. To prove this,
	try solving a quadratic polynomial by hand but starting with points (1,5), (1,2), (0,5).
	If you first try with A + Bx + Cx^2 = y, you'll end up with two equations like:
		A + B + C = 5 and A + B + C = 2
	Then, try with A + By + Cy^2 = x and you'll get two equations like:
		A + 5B + 25C = 1 and A + 5B + 25C = 0
	As the coefficents are constants, you cannot have either of those pairs of equations. This means
	those sets of points would be impossible to solve for and the program would be stuck in an infinite
	loop.
	*/
	bool validPoints = false;
	while(!validPoints){
		std::vector< std::pair<float, float> > fresh;
		INIT_POINTS = fresh;
		//Generate the points that the polynomial will be bound to
		for(int i = 0; i < DEGREE + 1; i++){
			float x = distr(eng);
			float y = distr(eng);
			std::pair<float, float> point (x, y);
			INIT_POINTS.push_back(point);
		}
		for(int i = 0; i < INIT_POINTS.size(); i++){
			for(int j = i+1; j < INIT_POINTS.size(); j++){
				if(INIT_POINTS[i].first == INIT_POINTS[j].first)xNotY = false;
			}
		}
		if(xNotY)validPoints = true;
		if(!xNotY){
			for(int i = 0; i < INIT_POINTS.size(); i++){
				float tempFirst = INIT_POINTS[i].first;
				INIT_POINTS[i].first = INIT_POINTS[i].second;
				INIT_POINTS[i].second = tempFirst;
			}
		}
		bool yVal = true;
		for(int i = 0; i < INIT_POINTS.size(); i++){
			for(int j = i+1; j < INIT_POINTS.size(); j++){
				if(INIT_POINTS[i].first == INIT_POINTS[j].first)yVal = false;
			}
		}
		if(yVal)validPoints = true;
	}

	//where the current best coefficients will be stored in driver thread
	std::vector<float> coefficients;

	//Queue containing coefficients to be mutated by worker threads
	ThreadSafeListenerQueue< std::vector<float> > worker_queue;
	//Queue containing newly mutated coefficients to be processed by driver thread
	ThreadSafeListenerQueue< std::vector<float> > driver_queue;

	//Start the worker threads and populate wQueue with the initial coefficients
	for(int i = 0; i < wThreadNum; i++){
		std::vector<float> seeds;
		//Generate the starting coefficients
		for(int j = 0; j < DEGREE + 1; j++){
			//Prefer to explicitly cast
			float coef = distr(eng)*5;
			if(i!=0)coef = coef/i;
			seeds.push_back(coef);
		}
		worker_queue.push(seeds);
		coefficients = seeds;
		workerThreads[i] = std::thread(Worker::work, &worker_queue, &driver_queue, &INIT_POINTS);
	}

	int iteration_count = 0;

	float currentFitness = Worker::fitness(coefficients, INIT_POINTS);
	std::cout << currentFitness << std::endl;
	//Driver loop while worker threads are producing new coefficients
	while(currentFitness > IDEAL_FIT){
		//Get new coefficients w/computed fitness from queue
		std::vector<float> newCoeffs;
		if(driver_queue.listen(newCoeffs)){
			//compare the fitness of two coefficients
			if(currentFitness > Worker::fitness(newCoeffs, INIT_POINTS)){
				//if the newCoeffs has better fitness, replace the old one
				worker_queue.push(newCoeffs);
				for(int i = 0; i < coefficients.size(); i++){
					coefficients[i] = newCoeffs[i];
				}
				currentFitness = Worker::fitness(coefficients, INIT_POINTS);
				std::cout << currentFitness << std::endl;
			}
			else if(iteration_count > (30000*DEGREE)){
				//This is hit because the program has hit a wall
				//Reseed with new starting coefficients
				std::vector<float> bs;
				while(worker_queue.pop(bs)){}
				while(driver_queue.pop(bs)){}
				for(int j = 0; j <wThreadNum; j++){
					std::vector<float> newSeeds;
					for(int i = 0; i < DEGREE + 1; i++){
						//Prefer to explicitly cast
						float coef = distr(eng)*15;
						if(i!=0)coef = coef /i;
						newSeeds.push_back(coef);
					}
					worker_queue.push(newSeeds);
					coefficients = newSeeds;
				}
				currentFitness = Worker::fitness(coefficients, INIT_POINTS);
				iteration_count = 0;
			}
			else{
				worker_queue.push(coefficients);
			}
			iteration_count++;
		}
	}

	//Make sure all the threads know the best fit has been found
	for(int i = 0; i < 2*wThreadNum; i++){
		std::vector<float> empty;
		worker_queue.push(empty);
	}

	//Work done -> join the threads
	for(int i = 0; i < wThreadNum; i++){
		workerThreads[i].join();
	}

	//Get points back in the right orientation
	if(!xNotY){
		for(int i = 0; i < INIT_POINTS.size(); i++){
			float tempFirst = INIT_POINTS[i].first;
			INIT_POINTS[i].first = INIT_POINTS[i].second;
			INIT_POINTS[i].second = tempFirst;
		}
	}

	char charX = 'x';
	char charY = 'y';

	//Print result
	std::cout << "Initial Points: " << std::endl;
	for(int i = 0; i < INIT_POINTS.size(); i++){
		std::cout << "(" << INIT_POINTS[i].first << ", ";
		std::cout << INIT_POINTS[i].second;
		if(i == INIT_POINTS.size()-1)std::cout << ")";
		else{
			std::cout << "), ";
		}
	}
	std::cout << std::endl;

	std::cout << "Polynomial Degree: " << DEGREE << std::endl;

	std::cout << "Polynomial: " << std::endl;
	char print = charY;
	for(int i = 0; i < coefficients.size(); i++){
		if(xNotY)print = charX;
		if(i == 0)std::cout << coefficients[i];
		else if(i == 1) std::cout << coefficients[i] << print;
		else{
			std::cout << coefficients[i] << print << "^" << i;
		}
		if(i != coefficients.size())std::cout << " + ";
	}
	std::cout << std::endl;

	std::cout << "Fitness: " << currentFitness << std::endl;

	return 0;
}