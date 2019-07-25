#ifndef TS_QUEUE_H_
#define TS_QUEUE_H_
#include <mutex>
#include <list>
#include <condition_variable>
#include <iostream>
#include <unordered_map>
#include <iterator>
#include <chrono>

template<class T>
class ThreadSafeListenerQueue{
	private:
		std::list<T> storage;

		//Mutex and Condition Variables to synchronize ThreadSafeListenerQueue
		std::mutex mList;
		std::condition_variable cv;
	public:
		/*
		Inserts an item to front of the queue. Returns true if inserted successfully.
	
		@param element: element to be added to the queue
		@returns true when the item is successfully inserted.
		*/
		bool push(const T element);

		/*
		Removes the element that has been in the queue the longest and stores it in
		the parameter element. Returns true if an element has been removed and false
		if the list is empty.

		@param element: where the removed element will be stored
		@returns true if an element has been removed and false if the list is empty
		*/
		bool pop(T& element);

		/*
		Removes the element that has been in the queue the longest and stores it in
		the parameter element. Returns true if an element has been removed and false
		if the list is empty. If the queue is empty, it puts the thread to sleep until
		there is an element in the queue.

		@param element: where the removed element will be stored
		@returns true when an element is removed
		*/
		bool listen(T& element);
};
/*
Inserts an item to front of the queue. Returns true if inserted successfully.
@param element: element to be added to the queue
@returns true when the item is successfully inserted.
*/
template<class T>
bool ThreadSafeListenerQueue<T>::push(const T element){
	std::lock_guard<std::mutex> lock(ThreadSafeListenerQueue<T>::mList);
	ThreadSafeListenerQueue<T>::storage.push_front(element);
	ThreadSafeListenerQueue<T>::cv.notify_one();
	return true;
}

/*
Removes the element that has been in the queue the longest and stores it in
the parameter element. Returns true if an element has been removed and false
if the list is empty.
@param element: where the removed element will be stored
@returns true if an element has been removed and false if the list is empty
*/
template<class T>
bool ThreadSafeListenerQueue<T>::pop(T& element){
	std::lock_guard<std::mutex> lock(ThreadSafeListenerQueue<T>::mList);
	if(ThreadSafeListenerQueue<T>::storage.empty())return false;
	element = ThreadSafeListenerQueue<T>::storage.back();
	ThreadSafeListenerQueue<T>::storage.pop_back();
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
template<class T>
bool ThreadSafeListenerQueue<T>::listen(T& element){
	std::chrono::milliseconds mil(1000);
	std::unique_lock<std::mutex> lock(ThreadSafeListenerQueue<T>::mList);
	if(cv.wait_for(lock, mil, [this]{return !(this->storage.empty());})){
		element = ThreadSafeListenerQueue<T>::storage.back();
		ThreadSafeListenerQueue<T>::storage.pop_back();
		return true;
	}
	else{
		return false;
	}
}

#endif