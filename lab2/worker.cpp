#include "worker.h"
#include <cmath>
#include <vector>
#include "ts_queue.h"
#include <random>

/*
Function that calculates the fitness according to the process defined in the READ_ME file


@param coefficients - the coefficents to be mutated
@param INIT_POINTS - the intitial points that the polynomial is bounded to

@return the calculated fitness of the set of coefficients.
*/
float Worker::fitness(std::vector<float> coefficients, const std::vector< std::pair<float, float> > INIT_POINTS){
	//The calculated fitness (a sum of the difference of y's)
	float fitness = 0;
	//Loop through all the points
	for(int i = 0; i < INIT_POINTS.size(); i++){
		//The calculated Y
		float calcY = 0;
		//Loop to calculate the polynomial
		//explicit cast of int x to float
		float fX = INIT_POINTS[i].first;
		for(int j = 0; j < coefficients.size(); j++){
			calcY += coefficients[j] * std::pow(fX, j);
		}
		//compare the difference between the two y-values
		fitness += std::abs(calcY - INIT_POINTS[i].second); 
	}
	return fitness;
}


/*
Takes the current coefficent set and randomly modifies them. Modification is dependent on the current fitness of the
the coefficients and the degree of the polynomial.

@param coefficients - the coefficents to be mutated
@param random - the random value to be added/subtracted from the coefficient
@param randInd - the random value that determines whether the random value is added, subtracted, or done nothing with
@param INIT_POINTS - the intitial points that the polynomial is bounded to

@return the newly modified coefficients
*/
std::vector<float> Worker::mutate(std::vector<float> coefficients, float random, const std::pair<int, int> randInd, const std::vector< std::pair<float, float> > INIT_POINTS){
	std::vector<float> mutated;
	int coeffSize = coefficients.size();
	mutated = coefficients;
	std::random_device randy;
	std::mt19937 engy(randy());
	std::uniform_int_distribution<> distry(1, 5);
	if(coeffSize == 0)return mutated;
	if(Worker::fitness(coefficients, INIT_POINTS) < 2.5 && coeffSize > 3){
		random = random / 10;
	}
	mutated[0] = Worker::mutOne(mutated[0], random, distry(engy));
	for(int i = 1; i < coeffSize; i++){
		mutated[i] = Worker::mutOne(mutated[i], random/i, distry(engy));
	}
	return mutated;
}

/*
Function that takes a coefficient and performs random modification on it. Returns the modified coefficient.

@param coeff - the coefficient to be modified
@param random - the random value to be added/subtracted from the coefficient
@param randInd - the random value that determines whether the random value is added, subtracted, or done nothing with

@return the modified coefficient
*/
float Worker::mutOne(float coeff, float random, int randInd){
	float mutated = coeff;
	if(randInd == 1){
		mutated = mutated - random;
	}
	else if (randInd == 2){
		mutated = mutated + random;
	}
	else if (randInd == 3){
		mutated = mutated * random;
	}
	else if(randInd == 4){
		mutated = mutated / random;
	}
	return mutated;
}


/*
The main function for each worker thread. Takes the current best coefficient and and modifies it. Then, it returns that value
to the driver thread for processing.

@param wQueue, dQueue - the worker and driver queues respectively where both threads put the modified/current best coefficients
@param INIT_POINTS - the intitial points that the polynomial is bounded to
*/
void Worker::work(ThreadSafeListenerQueue<std::vector<float>> * wQueue, ThreadSafeListenerQueue<std::vector<float>> * dQueue, const std::vector< std::pair<float, float> > * INIT_POINTS){
	std::vector<float> randoms;
	//Random generator for the points
	std::random_device rand;
	std::mt19937 eng(rand());
	std::uniform_int_distribution<> distr(1, 100);

	for(int i = 0; i < 100; i++){
		float j = distr(eng);
		float k = distr(eng);
		randoms.push_back(j/k);
	}

	int counter = 0;
	std::vector<float> coeffs;
	coeffs.push_back(1);

	std::vector< std::pair<float, float> > points = *INIT_POINTS;

	std::random_device randomSwitch;
	std::mt19937 engSwit(randomSwitch());
	std::uniform_int_distribution<> distrSwit(0, 6);

	while(coeffs.size() != 0){
		if(wQueue->listen(coeffs)){
			std::pair<int, int> switches;
			switches.first = distrSwit(engSwit) / 2;
			switches.second = distrSwit(engSwit);
			coeffs = Worker::mutate(coeffs, randoms[counter], switches, points);
			dQueue->push(coeffs);
		}
		else{
			std::vector<float> temp;
			for(int i = 0; i <= points.size(); i++){
				temp.push_back(1.0);
			}
			dQueue->push(temp);
		}
		if(++counter == 100)counter = 0;
	}
	return;
}