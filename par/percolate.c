#include "percolate.h"

int main(int argc, char *argv[]){
    int flag;
    flag = input(argc, argv, &inputVal); // read arguements
    if (flag != 0){ //if arguements is not valid, stop the program
        return 1;
    }

    // set seed for random number generator.
    rinit(inputVal.seed);

    //start MPI
    mpstart(&procId, &numProc);

    if (procId == 0){
        //print the arguments
        printf("percolate: params are L = %d, rho = %f, seed = %d\n",
        inputVal.length, inputVal.density, inputVal.seed);
    }

    // create a 2D Cartesian Virtual Topology
    int h,w; //h and w means the number of processors in two dimensions
    create2DCart(&h, &w);

    // calculate M and N which are the higtht and width
    // of old array and new array
    calcMN(inputVal.length, h, w, &M, &N, procId);

    //memory allocation
    if (procId == 0){ // allocate the memory for map only on master processor
        map = (int**)arralloc(sizeof(int), 2, inputVal.length, inputVal.length);
    }
    old = (int**)arralloc(sizeof(int), 2, M + 2, N + 2);
    new = (int**)arralloc(sizeof(int), 2, M + 2, N + 2);

    // initialise map on the master processor only
    if (procId == 0){
        initMap(map, inputVal.length, inputVal.density);
    }

    //scatter the map from master to other processors.
    scatterMap(map, old, inputVal.length, M, N, procId);

    //fill the halo of old and new array
    setHalo(old, M, N);

    //update the old
    int nchange;
    nchange = updateOld(old, new, inputVal.length, M, N, procId);

    //send old on every processor to map on master node.
    gatherMap(map, old, M, N, procId);

    // check for percolation and write out the result on the master process only
    if (procId == 0){
        int flag;
        flag = isPercolate(map, inputVal.length);
        if (flag != 0){
            // printf("percolate: cluster DOES percolate\n");
        }
        else{
            // printf("percolate: cluster DOES NOT percolate\n");
        }

        /*
         *  Write the map to the file "map.pgm", displaying only the very
         *  largest cluster (or multiple clusters if exactly the same size).
         *  If the last argument here was 2, it would display the largest 2
         *  clusters etc.
         */
         percwritedynamic(inputVal.pgmFile, map, inputVal.length, 5);
    }

    mpstop();
}
