#include "driver.h"
#include "worker.h"
#include "ts_queue.h"

int main(int argc, char ** argv ){
	float iteration = 0;
	float bests = 0;
	float min = -1;
	float max = 0;
	float average = 0;

	int degree = 2;
	int threads = 1;

	for(threads; threads < 5; threads++){
		for(degree; degree < 5; degree++){
			if(degree == 2){
				for(int i = 0; i < 100; i++){
					std::pair<std::pair<int, int>, std::vector<float>> result = Driver::solve(degree, threads);
					iteration += (result.first).first;
					bests += (result.first).second;
					average += (result.second)[2];
					if(min == -1 || min > (result.second)[0])min = (result.second)[0];
					if(max < (result.second)[1])max = (result.second)[1];
				}
				std::cout << "Average Iteration: " << iteration/100 << std::endl;
				std::cout << "Average Number Of Bests" << bests/100 << std::endl;
				std::cout << "Maximum Single Guess Iteration Time: " << max << std::endl;
				std::cout << "Minimum Single Guess Iteration Time: " << min << std::endl;
				std::cout << "Average Guess Iteration Time: " << average / 100 << std::endl;
				std::cout << "Degree: " << degree <<std::endl;
				std::cout << "Thread Num: " << threads << std::endl;
			}

			if(degree == 3){
				for(int i = 0; i < 5; i++){
					std::pair<std::pair<int, int>, std::vector<float>> result = Driver::solve(degree, threads);
					iteration += (result.first).first;
					bests += (result.first).second;
					average += (result.second)[2];
					if(min == -1 || min > (result.second)[0])min = (result.second)[0];
					if(max < (result.second)[1])max = (result.second)[1];
				}
				std::cout << "Average Iteration: " << iteration/100 << std::endl;
				std::cout << "Average Number Of Bests" << bests/100 << std::endl;
				std::cout << "Maximum Single Guess Iteration Time: " << max << std::endl;
				std::cout << "Minimum Single Guess Iteration Time: " << min << std::endl;
				std::cout << "Average Guess Iteration Time: " << average / 100 << std::endl;
				std::cout << "Degree: " << degree <<std::endl;
				std::cout << "Thread Num: " << threads << std::endl;
			}
			if(degree > 3){
				for(int i = 0; i < 1; i++){
					std::pair<std::pair<int, int>, std::vector<float>> result = Driver::solve(degree, threads);
					iteration += (result.first).first;
					bests += (result.first).second;
					average += (result.second)[2];
					if(min == -1 || min > (result.second)[0])min = (result.second)[0];
					if(max < (result.second)[1])max = (result.second)[1];
				}
				std::cout << "Average Iteration: " << iteration/100 << std::endl;
				std::cout << "Average Number Of Bests" << bests/100 << std::endl;
				std::cout << "Maximum Single Guess Iteration Time: " << max << std::endl;
				std::cout << "Minimum Single Guess Iteration Time: " << min << std::endl;
				std::cout << "Average Guess Iteration Time: " << average / 100 << std::endl;
				std::cout << "Degree: " << degree <<std::endl;
				std::cout << "Thread Num: " << threads << std::endl;
			}
		}
		degree = 2;
	}
	return 0;
}