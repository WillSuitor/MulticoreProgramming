#include <vector>
using namespace std;

template<typename K, typename V>
class multimap{
	public:
	vector<K> keys;
	vector<V> values;
	/*
	Will add the key-value pair to the multimap. As duplication of keys is allowed, there
	is no need to check if the key already exists in the multimap.

	@return true if the operation is successfull, false if not
	@param key, value - the values to be inserted into the multimap
	*/
	bool insert(const K& key, const V& value){
		if(key == NULL || value == NULL)return false;
		
	}
	/*
	Will return true if the key exists in the multimap. Will return false if not.

	@return true if key is in the multimap, false if not
	@param key - the search value to be compared to
	*/
	bool find(const K& key){

	}
	/*
	Will return true if the key exists in the multimap and false if not. Anytime the key is
	encountered, the associated value pair will be added to the list parameter. The values will
	overwrite whatever is in the list not be appended.

	@return true if the key exists in the multimap
	@param key - the search value
	@param values - where any associated values will be placed
	*/
	bool find(const K& key, std::list<V>& values){

	}
	/*
	Will go through the multimap and remove and key-value pairs with the key as the parameter. The
	function will return the number of key-value pairs removed. Will return -1 if there's an error.

	@return the number of key-value pairs removed (-1 if an error)
	@param key - the value to be removed from the multimap
	*/
	int remove(const K& key){

	}
};
int main(){

}