#ifndef FCOUNTER_H_
#define FCOUNTER_H_
#include <mutex>

//A threadsafe counter that can be incremented up or zeroed
class FutilityCounter{
	std::mutex mtx;
	unsigned int count;
public:
	//Specific to this lab -> counter holds the threshold
	int threshold;

	//increments the counter up by one
	void up();

	//resets the counter to 0
	void zero();

	//returns the current value of the counter
	unsigned int get();

	//creates a FC with the threshold set to cap
	FutilityCounter(int cap);
};

FutilityCounter::FutilityCounter(int cap){
	count = 0;
	threshold = cap;
}

void FutilityCounter::up(){
	std::lock_guard<std::mutex> lck(mtx);
	count++;
}

void FutilityCounter::zero(){
	std::lock_guard<std::mutex> lck(mtx);
	count = 0;
}

unsigned int FutilityCounter::get(){
	std::lock_guard<std::mutex> lck(mtx);
	return count;
}

#endif