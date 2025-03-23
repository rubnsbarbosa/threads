# Threads

OS's thread scheduling decisions are made by the kernel itself. Therefore, thread execution is nondeterministic and can lead to some issues. This project highlights the race conditions issue with threads from the nondeterministic timing of execution. Then demonstrates how to handle race conditions with mutex and dispatch semaphores in C++ on macOS to control access to a shared resource among multiple concurrent threads. 

## Compile the codes

```shell
g++ race.cpp -o race -lpthread
g++ mutex.cpp -o mutex -lpthread
g++ semaphore.cpp -o semaphore -lpthread
```

## Run the compiled codes 

```shell
./race
./mutex
./semaphore
```

## Race Condition

A race condition occurs when two or more concurrent threads can access a shared resource and try to change it at the same time. Basically they compete for the shared resource.  

```cpp
#define THREADS 5
// shared resource
unsigned long int shared = 0;

for (int x = 0; x < 10000000; x++)
{
    shared = shared + 1;
}
```

In [race.cpp](https://github.com/rubnsbarbosa/threads/blob/main/race.cpp), we have 5 threads executing this code at once, the value of `shared` won't end up being `50000000` as it should be. In fact, for each execution, the result of the shared resource will vary. Why the threads cannot increment the value of `shared` to `50000000`?

Because the OS's thread-scheduler algorithm can swap between threads at any time, we don't know the order in which the threads will attempt to access the shared data. So, before the execution on a given state is completed, another execution starts and the new thread’s initial state for a given operation is wrong because the previous execution has not completed. The state of `shared` can be changed by another thread even when the early execution has not completed because the threads are "racing" to change the resource (in this example by increment it by 1).

Race conditions can be avoided within a locking mechanism.

```cpp
for ( int i = 0; i < 10000000; i++ )
{
   //lock shared
   shared = shared + 1; 
   //unlock shared
}
```

Using this approach, the answer will always be `50000000`.

For locking mechanism we will see: mutex and semaphore.

## Mutex

The lack of synchronization between the threads in the example above while using the shared resource caused the issue. A common way of achieving thread synchronization is by using mutexes. 

* a mutex is a lock that we set before using a shared resource and release after using it.
* when the lock is set, no other thread can access the locked region of code.
* so this ensures synchronized access of shared resources in the code.

```cpp
#define THREADS 5
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// shared resource
unsigned long int shared = 0;

for (int x = 0; x < 10000000; x++)
{
    pthread_mutex_lock(&mutex); // critical section entry
    shared = shared + 1;
    pthread_mutex_unlock(&mutex); // critical section exit
}
```

When we run the code, we get the expected result, it took a few seconds to finish.

```shell
./mutex
shared resource: 50000000
```

A critical section is a section of code that needs to be executed without external interference, i.e. without another thread being able to affect the states within the section.  

* `pthread_mutex_lock(&mutex)` locks the mutex and the calling thread will be the owner of the lock.
*  if any other thread that try to call `pthread_mutex_lock(&mutex)` on the same mutex object, it will be suspended until the current owner thread releases the lock with `pthread_mutex_unlock(&mutex)`. 
* If multiple threads have tried to lock the same mutex while it is locked by the owner thread, then when the mutex is released by the owner thread with `pthread_mutex_unlock(&mutex)` only one of them will be able to proceed.

## Semaphore

Semaphore is as an integer variable which is used to solve the problem of the critical section in process synchronization. I'm running this code in a macOS and if we run the approach below we will get a deprecated warning message.

```cpp
#include <semaphore.h>

#define THREADS 5
// semaphore declaration
sem_t semaphore;
// shared resource
unsigned long int shared = 0;

for (int x = 0; x < 10000000; x++)
{
    sem_wait(&semaphore); // critical section entry
    shared = shared + 1;
    sem_post(&semaphore); // critical section exit
}

int main()
{
    // initialize semaphore
    sem_init(&semaphore, 0, 1);
}
```

warning message when I tried to compile

```shell
warning: 'sem_init' is deprecated [-Wdeprecated-declarations]
   25 |     sem_init(&semaphore, 0, 1);
      |     ^
/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/semaphore.h:55:42: note: 'sem_init' has been explicitly marked deprecated here
   55 | int sem_init(sem_t *, int, unsigned int) __deprecated;
      |                                          ^
/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/cdefs.h:214:40: note: expanded from macro '__deprecated'
  214 | #define __deprecated    __attribute__((__deprecated__))
      |                                        ^
1 warning generated.
```

If we have a look at [semaphore.h](https://github.com/swiftlang/swift-corelibs-libdispatch/blob/main/dispatch/semaphore.h) we will see that we need to `#include <dispatch/dispatch.h>` instead of `semaphore.h` Therefore, let's use Dispatch Semaphore Synchronization in macOS with C++ which is part of the Grand Central Dispatch (GCD) framework.

```cpp
#include <dispatch/dispatch.h>

#define THREADS 5
// semaphore declaration
dispatch_semaphore_t semaphore;
// shared resource
unsigned long int shared = 0;

for (int x = 0; x < 10000000; ++x)
{
    dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER); // critical section entry
    shared = shared + 1;
    dispatch_semaphore_signal(semaphore); // critical section exit
}

int main()
{
    // initialize semaphore with a count of 1
    semaphore = dispatch_semaphore_create(1);
}
```

When we run the code, we get the expected result, it took a few seconds to finish.

```shell
./semaphore
shared resource: 50000000
```

* `dispatch_semaphore_wait()` a function that decrements semaphore value.
* `dispatch_semaphore_signal()` a function that increments semaphore value.

if we'd like to double check we can change it like below

```cpp
int waitResult, signalResult;
for (int x = 0; x < 10; ++x)
{
    waitResult = dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
    std::cout << "wait result: " << waitResult << std::endl;
    shared = shared + 1;
    signalResult = dispatch_semaphore_signal(semaphore);
    std::cout << "signal result: " << signalResult << std::endl;
}
```

which it will display

```shell
wait result: 0
signal result: 1
wait result: 0
signal result: 1
wait result: 0
signal result: 1
wait result: 0
signal result: 1
```

This way we achieve thread synchronization using dispatch semaphore.

## License<a id="license"></a>

This repository is licensed under the [MIT License](https://github.com/rubnsbarbosa/threads/blob/main/LICENSE)
