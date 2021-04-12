#include <stdio.h>
#include "uni.h"
/*
* This file contains the functions for initialization
*/

// set M and N;
void calcMN(int length, int h, int w, int *M, int *N, int procId){
    int i,j;
    int quoM, quoN, remM, remN;
    quoM = length/h; //Quotients
    quoN = length/w;
    remM = length%h; //Remainders
    remN = length%w;
    *M = quoM; // if divisible, the quotient is result.
    *N = quoN;

    // if not divisible, then equal distribute the remainder
    // to the first crows and cols
    for (i=0; i<remM; i++){
        for (j=0; j<w; j++){
            int curProcId;
            curProcId = getPID(i,j);
            if (procId == curProcId){
                *M = quoM + 1;
            }
        }
    }

    for (j=0; j<remN; j++){
        for (i=0; i<h; i++){
            int curProcId;
            curProcId = getPID(i,j);
            if (procId == curProcId){
                *N = quoN + 1;
            }
        }
    }

}

// initialize the map
void initMap(int **map, int L, double rho){
    int i,j;
    float r;
    int nhole = 0; //accumulator
    for (i=0; i < L; i++){
        for (j=0; j < L; j++){
            r = random_uniform(); //get random number
            if(r < rho){
                map[i][j] = 0; //filled cell
            }
            else{
                nhole++;
                map[i][j] = nhole; //empty cell
            }
        }
    }

    // the actual density may be different to rho which we set
    printf("percolate: rho = %f, actual density = %f\n",rho,
    1.0 - ((double) nhole)/((double) L*L) );
}

// scatter the map from master to other processors.
void scatterMap(int **map, int **old, int L, int M, int N, int procId){
    int i,j;
    createScatType(L, M, N); // create data type that used for scatter and gather.
    if (procId == 0){
        sendAllBlock(map, M, N); // master processor scatter map to all processors
    }
    // every processor eceive map block from master processor, store it in the old
    recvBlock(&old[1][1]);
}

// gather old array from all processors to the master
void gatherMap(int **map, int **old, int M, int N, int procId){
    sendBlock(&old[1][1]); //every processor send old to master
    if (procId == 0){
        recvAllBlock(map, M, N); // master processor receive old and store them in map
    }
    gathFinish();
}

// set the halo values to zero.
void setHalo(int **old, int M, int N){
    int i,j;
    for (i=0; i <= M+1; i++){// zero the bottom and top halos
        old[i][0]   = 0;
        old[i][N+1] = 0;
    }
    for (j=0; j <= N+1; j++){// zero the left and right halos
        old[0][j]   = 0;
        old[M+1][j] = 0;
    }
}
