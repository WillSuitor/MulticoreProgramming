#include <vector>
#include <random>
#include <iostream>
#include <string>
#include <list>
#include <iterator>

template<class K, class V>
class Multimap{

	std::vector<K> keys;
	std::vector<V> values;

	public:
	//Multimap();

	//~Multimap();
	/*
	Will add the key-value pair to the multimap. As duplication of keys is allowed, there
	is no need to check if the key already exists in the multimap.

	@return true if the operation is successfull, false if not
	@param key, value - the values to be inserted into the multimap
	*/
	bool insert(const K& key, const V& value);

	/*
	Will return true if the key exists in the multimap. Will return false if not.

	@return true if key is in the multimap, false if not
	@param key - the search value to be compared to
	*/
	bool find(const K& key);

	/*
	Will return true if the key exists in the multimap and false if not. Anytime the key is
	encountered, the associated value pair will be added to the list parameter. The values will
	overwrite whatever is in the list not be appended.

	@return true if the key exists in the multimap
	@param key - the search value
	@param returnVals - where any associated values will be placed
	*/
	bool find(const K& key, std::list<V>& returnVals);

	/*
	Will go through the multimap and remove and key-value pairs with the key as the parameter. The
	function will return the number of key-value pairs removed.

	@return the number of key-value pairs removed
	@param key - the value to be removed from the multimap
	*/
	int remove(const K& key);

	//Print each key in the multimap on a new line (in order of insertion)
	void printKeys();
	//Print each value in the multimap on a new line (in order of insertion)
	void printValues();
};

template <class K, class V>
void Multimap<K, V>::printKeys(){
	for(typename std::vector<K>::iterator it = this->keys.begin(); it != this->keys.end(); it = std::next(it, 1)){
		std::cout << *it << std::endl;
	}
}
template <class K, class V>
void Multimap<K, V>::printValues(){
	for(typename std::vector<K>::iterator it = this->values.begin(); it != this->values.end(); it = std::next(it, 1)){
		std::cout << *it << std::endl;
	}
}

template <class K, class V>
bool Multimap<K, V>::insert(const K& key, const V& value){
	this->keys.push_back(key);
	this->values.push_back(value);
	return true;
}

template <class K, class V>
bool Multimap<K, V>::find(const K& key){
	if(this->keys.empty() || this->values.empty())return false;
	for(typename std::vector<K>::iterator it = this->keys.begin(); it != this->keys.end(); it = std::next(it, 1)){
		if(*it == key)return true;
	}
	return false;
}

template <class K, class V>
bool Multimap<K, V>::find(const K& key, std::list<V>& returnVals){
	if(this->keys.empty() || this->values.empty())return 0;
	bool result = false;
	for(typename std::vector<K>::iterator it = this->keys.begin(); it != this->keys.end(); it = std::next(it, 1)){
		if(*it == key){
			result = true;
			returnVals.push_back(*it);
		}
	}
	return result;
}

template <class K, class V>
int Multimap<K,V>::remove(const K& key){
	if(this->keys.empty() || this->values.empty())return 0;
	int result = 0;
	typename std::vector<V>::iterator vIt = this->values.begin();
	typename std::vector<K>::iterator it = this->keys.begin();
	while(it != this->keys.end()){
		if(*it == key){
			result++;
			this->values.erase(vIt);
			this->keys.erase(it);
		}
		else{
			it = std::next(it, 1);
			vIt = std::next(vIt, 1);
		}
	}
	return result;
}

int main(){
	std::uniform_int_distribution<int> range(0,200);
	std::default_random_engine generator;

	//loop to repeat the test 10 times
	for(int i = 0; i<10; i++){
		bool testPassed = true;
		std::list<int> testKeys;
		std::list<int> testVals;
		Multimap<int, int> testMap = Multimap<int, int>();
		//loop that populates multimap and lists with test values
		for(int j = 0; j < 100; j++){
			testKeys.push_back(range(generator));
			testVals.push_back(range(generator));

			testMap.insert(testKeys.back(), testVals.back());
		}
		//loop that checks the keys exist in the multimap and sums the number of vals
		int numOfVals = 0;
		while(!testKeys.empty()){
			int cur = testKeys.front();
			testPassed = testMap.find(cur);
			numOfVals = numOfVals + testMap.remove(cur);
			testKeys.remove(cur);
		}
		if(testPassed  && numOfVals == 100){
			std::cout << "Test Passed: True" << std::endl;
		}
		else{
			std::cout << "Test Passed: False" << std::endl;
		}
		testKeys.clear();
		testVals.clear();
	}
	return 0;
}
