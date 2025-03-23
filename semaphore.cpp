#include <iostream>
#include <pthread.h>
#include <dispatch/dispatch.h>

#define THREADS 5
dispatch_semaphore_t semaphore;
// shared resource
unsigned long int shared = 0;

void *threadIncrementSharedResource(void *)
{
    for (int x = 0; x < 10000000; ++x)
    {
        dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
        shared = shared + 1;
        dispatch_semaphore_signal(semaphore);
    }
    return NULL;
}

int main()
{
    pthread_t t[THREADS];
    int r;
    // initialize semaphore with a count of 1 (binary semaphore for mutual exclusion)
    semaphore = dispatch_semaphore_create(1);
    
    // creating threads
    for (int i = 0; i < THREADS; ++i)
    {
        r = pthread_create(&t[i], NULL, threadIncrementSharedResource, NULL);
    }

    // wait threads to finish
    for (int i = 0; i < THREADS; ++i)
    {
        r = pthread_join(t[i], NULL);
    }
    
    dispatch_release(semaphore);

    std::cout << "shared resource: " << shared << std::endl;
 
    return 0;
}
