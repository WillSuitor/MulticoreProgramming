#ifndef DRIVER_H_
#define DRIVER_H_
#include "ts_queue.h"
#include <vector>

class Driver{
public:
	static std::pair<std::pair<int, int>, std::vector<float>> solve(int degree, int numOfThreads);
};

#endif