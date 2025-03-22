#include <iostream>
#include <pthread.h>

#define THREADS 5

// shared resource
unsigned long int shared = 0;

void *threadCode(void *)
{
    //int aux = 1;
    for (int x = 0; x < 10000000; x++)
    {
        shared = shared + 1;
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
        r = pthread_create(&t[i], NULL, &threadCode, NULL);
    }

    // wait threads to finish
    for (int i = 0; i < THREADS; ++i)
    {
        r = pthread_join(t[i], NULL);
    }

    std::cout << "shared resource: " << shared << std::endl;

    return 0;
}

