#ifndef ARGINPUT_H
#define ARGINPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct inputArgs{
    int length; //map width and height.
    float density; //density
    int seed; //seed for random number generator.
    char pgmFile[128]; //PGM file name
};

// deal with the input argeuments from commond line
int input(int argc, char *args[], struct inputArgs *inputVal);

//check if the input is valid.
int checkInput(struct inputArgs *inputVal);

//print out the arguments, just for debug.
void printArgs(struct inputArgs *inputVal);


#endif
