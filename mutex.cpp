#include <iostream>
#include <pthread.h>

#define THREADS 5
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// shared resource
unsigned long int shared = 0;

void *threadIncrementSharedResource(void *)
{
    for (int x = 0; x < 10000000; x++)
    {
        pthread_mutex_lock(&mutex);
        shared = shared + 1;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main()
{
    pthread_t t[THREADS];
    int r;

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

    std::cout << "shared resource: " << shared << std::endl;

    return 0;
}
