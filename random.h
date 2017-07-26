#ifndef RANDOM_H
#define RANDOM_H

//needed for getting system time and for random generation functions rand and srand
#include <time.h>
#include <stdlib.h>

//this function returns a random number between an inclusive range
int randomRange(const int & min, const int & max)
{
    if (max >= min)
        return ((rand() % (max+1-min))+min);
    else
        return ((rand() %  (min+1-max))+max);
}

//this function returns a random seed for use in a random number generator
int getRandomSeed(const int & min = 1, const int & max = 10000)
{
    /*in order to get a random seed, this function:
    (1)seeds the random number generator with time, so it is seeded differently every time it is called
    (2)compound the difference by continually setting the seed with the last generated number
    (3)return the unique seed
    */

    //to start of with a number that is never the same
    srand(time(0));

    //in order to make even small differences in system time come out quite differently
    for (int i = 1; !(i > 10); i++)
    {
        //the range is logical because it can give all possible positive results for the next seed
        srand(randomRange(1, RAND_MAX));
    }

    //this final result is thoroughly scrambled from the initial system time input
    return (randomRange(min, max));
}

//this function sets the random number generator's seed with a random seed
void setRandomSeed()
{
    srand(getRandomSeed());
}


#endif
