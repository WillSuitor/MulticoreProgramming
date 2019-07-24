#ifndef WORKER_H_
#define WORKER_H_
#include "ts_queue.h"
#include <vector>

class Worker{
	public:

		/*
		Function that calculates the fitness according to the process defined in the READ_ME file


		@param coefficients - the coefficents to be mutated
		@param INIT_POINTS - the intitial points that the polynomial is bounded to

		@return the calculated fitness of the set of coefficients.
		*/
		static float fitness(std::vector<float> coefficients, const std::vector< std::pair<float, float> > INIT_POINTS);

		/*
		Takes the current coefficent set and randomly modifies them. Modification is dependent on the current fitness of the
		the coefficients and the degree of the polynomial.

		@param coefficients - the coefficents to be mutated
		@param random - the random value to be added/subtracted from the coefficient
		@param randInd - the random value that determines whether the random value is added, subtracted, or done nothing with
		@param INIT_POINTS - the intitial points that the polynomial is bounded to

		@return the newly modified coefficients
		*/
		static std::vector<float> mutate(std::vector<float> coefficients, float random, const std::pair<int, int> randInd, const std::vector< std::pair<float, float> > INIT_POINTS);

		/*
		Function that takes a coefficient and performs random modification on it. Returns the modified coefficient.

		@param coeff - the coefficient to be modified
		@param random - the random value to be added/subtracted from the coefficient
		@param randInd - the random value that determines whether the random value is added, subtracted, or done nothing with

		@return the modified coefficient
		*/
		static float mutOne(float coeff, float random, int randInd);

		/*
		The main function for each worker thread. Takes the current best coefficient and and modifies it. Then, it returns that value
		to the driver thread for processing.

		@param wQueue, dQueue - the worker and driver queues respectively where both threads put the modified/current best coefficients
		@param INIT_POINTS - the intitial points that the polynomial is bounded to
		*/
		static void work(ThreadSafeListenerQueue<std::vector<float>> * wQueue, ThreadSafeListenerQueue<std::vector<float>> * dQueue, const std::vector< std::pair<float, float> > * INIT_POINTS);
};

#endif