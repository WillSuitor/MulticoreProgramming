Lab 2 - Will Suitor
Thread Pool:
The thread pool for this program is pretty simple. There is an array of threads in the driver thread (see driver.cpp)
that are all given the work function (see worker.cpp). There are two threadsafe queues shared between all the threads.
One queue (worker_queue) receives coefficients from the driver thread and has all the threads in the pool waiting on it.
The other queue (driver_queue) receives modified coefficients from the worker threads and checks them for correctness.
If they are better, they are put in the worker_queue. Otherwise, the current best is added.

Fitness:
Fitness is the metric by which the coefficients are assessed. This relies on a distance formula. For each point in the
set, a y is calculated from the x-values of the point and the current coefficients. This value is then added to a sum for
all of the points in the set. That sum is the calculated fitness of the coefficients.

Coefficients:
Stored from lowest degree to highest degree in the vector {x^0, x^1, ..., x^n}. The fitness metric is not stored with the
coefficients.
