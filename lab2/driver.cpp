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

	std::string tLC = "-t"; std::string tUC = "-T";
	std::string dLC = "-d"; std::string dUC = "-D";

	for(int i = 1; i < argc; i++){
		if(dLC.compare(argv[i]) == 0 || dUC.compare(argv[i]) == 0){
			DEGREE = std::stoi(argv[i+1]);
		}
		else if(tLC.compare(argv[i]) == 0 || tUC.compare(argv[i]) == 0){
			wThreadNum = std::stoi(argv[i+1]);
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

	//Generate the points that the polynomial will be bound to
	for(int i = 0; i < DEGREE + 1; i++){
		float x = distr(eng);
		float y = distr(eng);
		std::pair<float, float> point (x, y);
		INIT_POINTS.push_back(point);
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
			float coef = distr(eng);
			seeds.push_back(coef);
		}
		worker_queue.push(seeds);
		coefficients = seeds;
		workerThreads[i] = std::thread(Worker::work, &worker_queue, &driver_queue, &INIT_POINTS);
	}

	int iteration_count = 0;
	float diffFit = 0;

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
				float tempFit = Worker::fitness(coefficients, INIT_POINTS);
				diffFit = currentFitness - tempFit;
				currentFitness = tempFit;
				std::cout << currentFitness << std::endl;
			}
			else if(iteration_count > 1500 && diffFit < .002){
				std::vector<float> newSeeds;
				for(int i = 0; i < DEGREE + 1; i++){
					//Prefer to explicitly cast
					float coef = distr(eng);
					newSeeds.push_back(coef);
				}
				iteration_count = 0;
				worker_queue.push(newSeeds);
				coefficients = newSeeds;
				currentFitness = Worker::fitness(coefficients, INIT_POINTS);
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
	for(int i = 0; i < coefficients.size(); i++){
		if(i == 0)std::cout << coefficients[i];
		else if(i == 1) std::cout << coefficients[i] << "x";
		else{
			std::cout << coefficients[i] << "x^" << i;
		}
		if(i != coefficients.size())std::cout << " + ";
	}
	std::cout << std::endl;

	std::cout << "Fitness: " << Worker::fitness(coefficients, INIT_POINTS);

	return 0;
}