// calculate the sum of old array
#include <stdio.h>
unsigned long long sumOld(int **old, int M, int N){
    unsigned long long sum = 0;
    int i,j;
    for (i=1; i<=M; i++){
        for (j=1; j<=N; j++){
            sum += old[i][j];
        }
    }
    return sum;
}

//main update loop
int updateOld(int **old, int **new, int L, int M, int N, int rank){
    int i,j;

    // Find the neighbour processors
    int procUp, procDown, procLeft, procRight;
    findNeib(&procUp, &procDown, &procLeft, &procRight);

    // Define type for halo swaps, cols and rows.
    createSwapType(M, N);

    //finish the map scatter
    scatFinish();

    //loop variables
    int maxstep = 16*L;
    int nchange = 0;
    int step = 1;
    int printfreq = 100;

    //timing begin
    double tstart;
    getTime(&tstart);

    //start the update loop
    while (step <= maxstep){
        nchange = 0;

        // halo swaps
        swapHalo(old, M, N, procUp, procDown, procLeft, procRight);

        //compute the interio cells, which do not depend on the halo
        nchange += updateCell(old, new, 2, M-1, 2, N-1);

        //finish the Haloswap before updating boundary cells
        swapFinish();

        //compute the boundary cells
        //left col
        nchange += updateCell(old, new, 1, M, 1, 1);
        //right col
        nchange += updateCell(old, new, 1, M, N, N);
        //top row
        nchange += updateCell(old, new, 1, 1, 2, N-1);
        //bottom row
        nchange += updateCell(old, new, M, M, 2, N-1);

        // get sum of changes
        getSum(&nchange);

        // Report progress every now and then
        if (step % printfreq == 0){
            //calculate the sum of every old and map.
            unsigned long long partialSum;
            partialSum = sumOld(new, M, N);
            getMapSum(&partialSum);

            if (rank == 0){
                double average = (partialSum*1.0)/(L*L); // calculate average
                printf("percolate: number of changes on step %d is %d\n", step, nchange);
                printf("The average of map is %f\n\n", average);
            }
        }

        //stop iterion when there is no change
        if (nchange == 0){
            break;
        }

        // Copy back in preparation for next step, omitting halos
        for (i=1; i<=M; i++){
            for (j=1; j<=N; j++){
                old[i][j] = new[i][j];
            }
        }

        step++; //count step
    } //end while

    //timing end
    double tstop;
    getTime(&tstop);

    if (rank == 0){
        printf("time = %f\n", tstop-tstart);
        printf("step= %d\n", step);
    }

    /*
     *  We set a maximum number of steps to ensure the algorithm always
     *  terminates. However, if we hit this limit before the algorithm
     *  has finished then there must have been a problem (e.g. maxstep
     *  is too small)
     */
    if (nchange != 0){
        printf("percolate on rank %d: WARNING max steps = %d reached before nchange = 0\n", rank, maxstep);
    }
    return nchange;
}

//judge if the map is percolate
int isPercolate(int **map, int L){
    int perc = 0;
    int itop, ibot;
    for (itop=0; itop < L; itop++){
        if (map[itop][L-1] > 0){
            for (ibot=0; ibot < L; ibot++){
                if (map[itop][L-1] == map[ibot][0]){
                    perc = 1;
                }
            }
        }
    }
    return perc;
}

// update the cell to be the maximum value of neighbours
int updateCell(int **old, int **new, int iStart, int iStop, int jStart, int jStop){
    int i,j;
    int nchange = 0;
    int oldval, newval;

    for (i=iStart; i<=iStop; i++){
        for (j=jStart; j<=jStop; j++){
            oldval = old[i][j];
            newval = oldval;
            if (oldval != 0){//Set new[i][j] to be the maximum value of neighbours
                if (old[i-1][j] > newval) newval = old[i-1][j];
                if (old[i+1][j] > newval) newval = old[i+1][j];
                if (old[i][j-1] > newval) newval = old[i][j-1];
                if (old[i][j+1] > newval) newval = old[i][j+1];
                if (newval != oldval){
                    ++nchange;
                }
            }
            new[i][j] = newval;
        }
    }

    return nchange;
}
