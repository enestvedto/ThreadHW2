#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

// global arrays
int *unsorted = NULL;
int sorted[20];

void initializeArrays()
{
    // initialize global variable
    unsorted = malloc(20 * sizeof(int));

    printf("The unsorted array is:\n");

    // fill array with random numbers from 1-100 and complete print statement
    // for first 19/20 numbers with trailing comma
    for (int i = 0; i < 19; i++)
    {
        unsorted[i] = rand() % 100 + 1;
        printf("%d, ", unsorted[i]);

        // initialize sorted array to all zero
        sorted[i] = 0;
    }
    sorted[19] = 0;

    // fill and print  final value without a comma.
    unsorted[19] = rand() % 100 + 1;
    printf("%d\n", unsorted[19]);
}

// required to use counting sort for this method
void *sortArray(void *arg)
{
    int *firstIndex = (int *)arg;
    int output[10];

    // Find the largest element of the array
    int max = -1;
    for (int i = *firstIndex; i < *firstIndex + 10; i++)
    {
        if (unsorted[i] > max)
            max = unsorted[i];
    }

    // The size of count must be at least (max+1) but
    // we cannot declare it as int count(max+1) in C as
    // it does not support dynamic memory allocation.
    // So, its size is provided statically.
    int count[101];

    // Initialize count array with all zeros.
    for (int i = 0; i <= max; ++i)
    {
        count[i] = 0;
    }

    // Store the count of each element
    for (int i = *firstIndex; i < *firstIndex + 10; i++)
    {
        count[unsorted[i]]++;
    }

    // Store the cummulative count of each array
    for (int i = 1; i <= max; i++)
    {
        count[i] += count[i - 1];
    }

    // Find the index of each element of the original array in count array, and
    // place the elements in output array
    for (int i = (*firstIndex + 10) - 1; i >= *firstIndex; i--)
    {
        output[count[unsorted[i]] - 1] = unsorted[i];
        count[unsorted[i]]--;
    }

    // Copy the sorted elements into original array
    // if first half
    if (*firstIndex == 0)
    {
        for (int i = 0; i < 10; i++)
        {
            unsorted[i] = output[i];
        }
    }
    // if second half
    else
    {
        for (int i = 0; i < 10; i++)
        {
            unsorted[i + 10] = output[i];
        }
    }

    return NULL;
}

void *mergeArrays(void *arg)
{
    // Initialize i and j to store indices of the last element of 1st and 2nd array respectively...
    int i = 9;
    int j = 19;
    int idx = 19;
    // Create a loop until either of i or j becomes zero...
    while (i >= 0 && j >= 10)
    {
        if (unsorted[i] > unsorted[j])
        {
            sorted[idx--] = unsorted[i];
            i--;
        }
        else
        {
            sorted[idx--] = unsorted[j];
            j--;
        }
    }
    // While j does not become zero...
    while (j >= 10)
    {
        sorted[idx--] = unsorted[j];
        j--;
    }
    while (i >= 0)
    {
        sorted[idx--] = unsorted[i];
        i--;
    }
    return NULL;
}

void printSorted()
{
    printf("The sorted array is:\n");

    // fill array with random numbers from 1-100 and complete print statement
    // for first 19/20 numbers with trailing comma
    for (int i = 0; i < 19; i++)
    {
        printf("%d, ", sorted[i]);
    }
    // print final value without a comma.
    printf("%d\n", sorted[19]);
}

int main(int argc, char const *argv[])
{
    // for different random numbers every time
    srand(time(NULL));

    initializeArrays();

    int rc;

    // spawn worker thread for first half
    pthread_t t1;
    int idxZero = 0;
    rc = pthread_create(&t1, NULL, sortArray, &idxZero);
    assert(rc == 0);

    // spawn worker thread for second half
    pthread_t t2;
    int idxTen = 10;
    rc = pthread_create(&t2, NULL, sortArray, &idxTen);
    assert(rc == 0);

    // wait for threads to finish
    rc = pthread_join(t1, NULL);
    assert(rc == 0);
    rc = pthread_join(t2, NULL);
    assert(rc == 0);

    // now indices 0-9 is sorted and indices 10-19 are sorted
    pthread_t t3;
    rc = pthread_create(&t3, NULL, mergeArrays, NULL);
    assert(rc == 0);
    rc = pthread_join(t3, NULL);
    assert(rc == 0);

    // print new sorted array
    printSorted();

    free(unsorted);
    return 0;
}
