#include <stdio.h>
#include <stdlib.h>

#include "argInput.h"
#include "arralloc.h"
#include "uni.h"

// store the input arguements
struct inputArgs inputVal;
int M;
int N;

// M × N integer arrays old and new with halos
int** old;
int** new;

// L × L array map without halos
int** map;

//MPI variables
int procId; // rank, the ID of processors
int numProc; // size, the number of processors
