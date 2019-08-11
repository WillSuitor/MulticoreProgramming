#ifndef THREADSAFEMULTIMAP_H_
#define THREADSAFEMULTIMAP_H_

#include <map>
#include <utility>
#include <pthread.h>

template<class K, class V>
class ThreadSafeMultimap{
	protected:
	//Internal hashmap used to store key-value pairs
	std::multimap<K, V> map;
	pthread_rwlock_t mutliMutex;
	public:
	/*
	Inserts key-value pair into the map if the the value does not already exist.
	If the key already exists in the map, the value is changed to the new value. Returns
	true on successful insertion.

	@param key, value: the key-value pair to be inserted
	@return true when the key-value pair is inserted/updated
	*/
	bool insert(const K key, const V value);

	/*
	Returns true if the key is present in the map and false if not. If it is present, the
	associated value is stored in the value parameter.

	@param key: key to be searched for
	@param value: where the found value is stored
	@return true if the value is present and false if not
	*/
	bool lookup(const K key, V& value);

	/*
	Removes the key-value pair from the map if it is present in the map. Does nothing if not.
	Returns true assuming no errors/exceptions.

	@param key: key element to be removed from the map
	@return true if there are no exceptions
	*/
	bool remove(const K key);

	/*
	Accesses the index-th element in the multimap. If the index is above the range, then the last
	element is returned. If it is below, then a null value is returned.
	
	@param index - the index of the element to be accessed
	@return the associated value of the index
	*/
	V operator [](int index);

	/*
	Reduces the size of the map to newSize. The highest value elements are cut off.

	@param newSize - the size to trim the map to
	*/
	void truncate(int newSize);

	/*
	Returns the number of elements in the map.

	@return the size of the map
	*/
	int size();

	//Default constructor
	ThreadSafeMultimap();
};

//Default constructor
template<class K, class V>
 ThreadSafeMultimap<K,V>::ThreadSafeMultimap(){
		pthread_rwlock_init(&mutliMutex, NULL);
}


/*
Inserts key-value pair into the map if the the value does not already exist.
If the key already exists in the map, the value is changed to the new value. Returns
true on successful insertion.

@param key, value: the key-value pair to be inserted
@return true when the key-value pair is inserted/updated
*/
template<class K, class V>
bool  ThreadSafeMultimap<K,V>::insert(const K key, const V value){
	pthread_rwlock_wrlock(&mutliMutex);
	map.insert(std::pair<K,V> (key, value));
	pthread_rwlock_unlock(&mutliMutex);
	return true;
}

/*
Returns true if the key is present in the map and false if not. If it is present, the
associated value is stored in the value parameter.

@param key: key to be searched for
@param value: where the found value is stored
@return true if the value is present and false if not
*/
template<class K, class V>
bool  ThreadSafeMultimap<K,V>::lookup(const K key, V& value){
	pthread_rwlock_rdlock(&mutliMutex);
	auto ele = map.find(key);
	if(ele == map.end())return false;
	value = ele->second;
	pthread_rwlock_unlock(&mutliMutex);
	return true;
}


/*
Removes the key-value pair from the map if it is present in the map. Does nothing if not.
Returns true assuming no errors/exceptions.

@param key: key element to be removed from the map
@return true if there are no exceptions
*/
template<class K, class V>
bool  ThreadSafeMultimap<K,V>::remove(const K key){
	pthread_rwlock_wrlock(&mutliMutex);
	map.erase(key);
	pthread_rwlock_unlock(&mutliMutex);
	return true;
}

/*
Accesses the index-th element in the multimap. If the index is above the range, then the last
element is returned. If it is below, then a null value is returned.

@param index - the index of the element to be accessed
@return the associated value of the index
*/
template<class K, class V>
V  ThreadSafeMultimap<K,V>::operator [](int index){
	V val;
	pthread_rwlock_rdlock(&mutliMutex);
	int size = map.size();
	if(size == 0){
		pthread_rwlock_unlock(&mutliMutex);
		return val;
	}
	if(index >= size){
		val = map.rbegin()->second;
	}
	else{
		auto it = map.begin();
		for(int i = 0; i < index; i++){
			++it;
		}
		val = it->second;
	}
	pthread_rwlock_unlock(&mutliMutex);
	return val;
}

/*
Reduces the size of the map to newSize. The highest value elements are cut off.

@param newSize - the size to trim the map to
*/
template<class K, class V>
void  ThreadSafeMultimap<K,V>::truncate(int newSize){
	std::multimap<K, V> newMap;
	pthread_rwlock_wrlock(&mutliMutex);
	if(map.size() < newSize){
		pthread_rwlock_unlock(&mutliMutex);
		return;
	}
	int count = 0;
	for(auto it = map.begin(); it != map.end(); ++it){
		if(count < newSize)newMap.insert(*it);
		count++;
	}
	map = newMap;
	pthread_rwlock_unlock(&mutliMutex);
	return;
}

/*
Returns the number of elements in the map.

@return the size of the map
*/
template<class K, class V>
int ThreadSafeMultimap<K,V>::size(){
	int length;
	pthread_rwlock_rdlock(&mutliMutex);
	length = map.size();
	pthread_rwlock_unlock(&mutliMutex);
	return length;
}
#endif