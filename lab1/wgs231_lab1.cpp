#include <iostream>
#include <unordered_map>
#include <mutex>
#include <list>
#include <condition_variable>
#include <thread>
#include <random>
#include <iterator>
#include <ctime>

//Mutex used to synchronize ThreadSafeKVStore
std::mutex mMap;
//Mutex and Condition Variables to synchronize ThreadSafeListenerQueue
std::mutex mList;
std::condition_variable cv;

/*
A threadsafe implementation of the standard unordered_map. This class stores key-value
pairs of an undefined type and allows access from multiple threads.
*/
template<class K, class V>
class ThreadSafeKVStore{
	//Internal hashmap used to store key-value pairs
	std::unordered_map<K, V> umap;

	public:
	/*
	Inserts key-value pair into the hashmap if the the value does not already exist.
	If the key already exists in the map, the value is changed to the new value. Returns
	true on successful insertion.

	@param key, value: the key-value pair to be inserted
	@return true when the key-value pair is inserted/updated
	*/
	bool insert(const K key, const V value){
		std::lock_guard<std::mutex> lock(mMap);
		umap[key] = value;
		return true;
	}

	/*
	Inserts the key-value pair if it does not already exist. If the key is already in the
	map, the value is added to the current value. Returns true when it adds/updates the value.

	@param key, value: the key-value pair to be inserted
	@return true when the key-value pair is inserted/updated
	*/
	bool accumulate(const K key, const V value){
		std::lock_guard<std::mutex> lock(mMap);
		if(umap.find(key) == umap.end()){
			umap.insert({key, value});
			return true;
		}
		umap[key] = umap[key] + value;
		return true;
	}

	/*
	Returns true if the value is present in the map and false if not. If it is present, the
	associated value is stored in the value parameter.

	@param key: key to be searched for
	@param value: where the found value is stored
	@return true if the value is present and false if not
	*/
	bool lookup(const K key, V& value){
		std::lock_guard<std::mutex> lock(mMap);
		auto val = umap.find(key);
		if(val == umap.end())return false;
		value = val->second;
		return true;
	}

	/*
	Removes the key-value pair from the map if it is present in the map. Does nothing if not.
	Returns true assuming no errors/exceptions.

	@param key: key element to be removed from the map
	@return true if there are no exceptions
	*/
	bool remove(const K key){
		std::lock_guard<std::mutex> lock(mMap);
		if(umap.find(key) == umap.end())return true;
		umap.erase(key);
		return true;
	}
	typename std::unordered_map<K, V>::iterator begin(){
		return umap.begin();
	}
	typename std::unordered_map<K, V>::iterator end(){
		return umap.end();
	}
};


//A threadsafe implementation of a first-in, first-out queue. 
template<class T>
class ThreadSafeListenerQueue{
	std::list<T> list;

	public:

	/*
	Inserts an item to front of the queue. Returns true if inserted successfully.
	
	@param element: element to be added to the queue
	@returns true when the item is successfully inserted.
	*/
	bool push(const T element){
		std::lock_guard<std::mutex> lock(mList);
		list.push_front(element);
		cv.notify_all();
		return true;
	}

	/*
	Removes the element that has been in the queue the longest and stores it in
	the parameter element. Returns true if an element has been removed and false
	if the list is empty.

	@param element: where the removed element will be stored
	@returns true if an element has been removed and false if the list is empty
	*/
	bool pop(T& element){
		std::lock_guard<std::mutex> lock(mList);
		if(list.empty())return false;
		element = list.back();
		list.pop_back();
		return true;
	}
	/*
	Removes the element that has been in the queue the longest and stores it in
	the parameter element. Returns true if an element has been removed and false
	if the list is empty. If the queue is empty, it puts the thread to sleep until
	there is an element in the queue.

	@param element: where the removed element will be stored
	@returns true when an element is removed
	*/
	bool listen(T& element){
		std::lock_guard<std::mutex> lock(mList);
		if(list.empty()){
			cv.wait(lock);
			element = list.back();
			list.pop_back();
			return true;
		}
		element = list.back();
		list.pop_back();
		return true;
	}
};

void threadTest(ThreadSafeKVStore<std::string, int32_t> *kvStore, ThreadSafeListenerQueue<int32_t> *que){
	std::time_t start = std::time(nullptr);
	//Storage for values and keys
	int32_t valueSum = 0;
	std::vector<std::string> keys;

	//Random Case Generator
	std::random_device rdSwitch;
	std::mt19937 engSwitch(rdSwitch());
	std::uniform_int_distribution<> distrSwitch(0, 4);

	//Random Key Generator
	std::random_device rdKey;
	std::mt19937 engKey(rdKey());
	std::uniform_int_distribution<> distrKey(0,500);

	//Random Value Generator
	std::random_device rdVal;
	std::mt19937 engVal(rdVal());
	std::uniform_int_distribution<> distrVal(-256, 256);

	bool passed = true;
	//Test Loop
	for(int i = 0; i < 10000; i++){
		int swtch = distrSwitch(engSwitch);
		switch(swtch)
		{
			//Case to accumulate new key-value pair
			case 0:{
				//Get Random Key-Val pair
				int key = distrKey(engKey);
				int32_t value = distrVal(engVal);
				std::string fullKey = "user";
				fullKey = fullKey + std::to_string(key);

				//Accumulate Key-val pair
				kvStore->accumulate(fullKey, value);

				//Keep track of keys and values
				keys.push_back(fullKey);
				valueSum += value;
				break;
			}
			//Case to look up existing key-value pair
			default: {
				if(keys.size() == 0)break;
				int randIndex = rand() % keys.size();
				int32_t nonEssential;
				if(!(kvStore->lookup(keys[randIndex], nonEssential))){
					passed = false;
				}
				break;
			}
		}
	}
	if(!passed)std::cout << "Test Failed" <<std::endl;
	que->push(valueSum);
	std::cout << "Time Of Thread: " << std::difftime(time(nullptr), start) << std::endl;
}


int main(int argc, char ** argv){
	ThreadSafeKVStore<std::string, int32_t> kv;
	ThreadSafeListenerQueue<int32_t> q;

	int threadNum = 3;
	if(argc < 3){
		std::cout << "Args need to be formatted as: " <<std::endl;
		std::cout << "lab1 -n <NumberOfThreads>" <<std::endl;
		return 0;
	}

	std::string arg1 = argv[1];
	std::string threadString = argv[2];

	if(arg1.compare("-n") != 0 && arg1.compare("-N") != 0){
		std::cout << "Args need to be formatted as: " <<std::endl;
		std::cout << "lab1 -n <NumberOfThreads>" <<std::endl;
		return 0;
	}

	threadNum = std::stoi(threadString);	

	std::thread threads[threadNum];

	//Time as threads are running
	std::time_t start = std::time(nullptr);
	for(int i = 0; i < threadNum;i++){
		threads[i] = std::thread(threadTest, &kv, &q);
	}

	//Number of threads returned so far
	int threadRet = 0;
	//Sum of Values Produced by Individual Threads
	int32_t totalValSum = 0;
	while(threadRet != threadNum){
		int32_t oneSum;
		q.listen(oneSum);
		totalValSum = totalValSum + oneSum;
		threadRet++;
	}

	//Joining all the threads
	for(int i = 0; i < threadNum;i++){
		threads[i].join();
	}
	//Looping through the actual struct
	int32_t sumStructVal = 0;
	for(auto it = kv.begin(); it!=kv.end(); ++it){
		sumStructVal += it->second;
	}

	//Testing if the two values are the same
	if(sumStructVal != totalValSum){
		std::cout << "Sum of Values in Actual Data: " << sumStructVal <<std::endl;
		std::cout << "Sum of Values Returned By Threads: " << totalValSum <<std::endl;
		std::cout << "Test Failed" << std::endl;
	}
	else{
		std::cout << "Sum of Values in Actual Data: " << sumStructVal <<std::endl;
		std::cout << "Sum of Values Returned By Threads: " << totalValSum <<std::endl;
		std::cout << "Test Passed" << std::endl;
	}

	std::cout << "Total Program RunTime: " << std::difftime(std::time(nullptr), start) << std::endl;

	return 0;
}
