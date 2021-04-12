#include "mplib.h"

void mpstart(int *procId, int *numProc){
    MPI_Init(NULL, NULL);

    // get size and rank
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    *procId = rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    *numProc = size;

    if (rank == 0){
        // allocate memory for request list
        srequest_s = (MPI_Request*)arralloc(sizeof(MPI_Request), 1, size);
        rrequest_g = (MPI_Request*)arralloc(sizeof(MPI_Request), 1, size);

        // allocate memory for DataType list, every block may be different,
        // when the number of processors in two dimensions are different,
        // or when the length is not divisible by the number of processors
        mapBlock = (MPI_Datatype*)arralloc(sizeof(MPI_Datatype), 1, size);

        // lists store the M and N of old array in different processors,
        // the master need these information to do the scatter and gather.
        Nlist = (int*)arralloc(sizeof(int), 1, size);
        Mlist = (int*)arralloc(sizeof(int), 1, size);
    }
}

void create2DCart(int* h, int* w){
    int periods[2];
    int dims[2];
    dims[0] = 0;
    dims[1] = 0;
    periods[0] = 1;
    periods[1] = 0;
    MPI_Dims_create(size, 2, dims); //set dims
    *h = dims[0];
    *w = dims[1];

    //create a 2D Cartesian Virtual Topology
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &comm_cart);
}

void createScatType(int L, int M, int N){
    int localM = M; //different processor have different M and N
    int localN = N;

    //gather M and N to master node so to be used in scatter and gather.
    MPI_Gather(&localM, 1, MPI_INT, Mlist, 1, MPI_INT, 0, comm_cart);
    MPI_Gather(&localN, 1, MPI_INT, Nlist, 1, MPI_INT, 0, comm_cart);

    // on master, define the data type for scatter and gather
    if (rank == 0){
        int i;
        for (i=0; i<size; i++){
            MPI_Type_vector(Mlist[i], Nlist[i], L, MPI_INT, &mapBlock[i]);
            MPI_Type_commit(&mapBlock[i]);
        }
    }

    //create old block type for scatter and gather.
    MPI_Type_vector(M, N, N+2, MPI_INT, &oldBlock);
    MPI_Type_commit(&oldBlock);
}

//map ranks to process grid coordinates
void calcCoords(int procId, int coords[2]){
    MPI_Cart_coords(comm_cart, procId, 2, coords);
}

//scatter map to all processors
void sendAllBlock(int **map, int M, int N){
    int i,j;
    int tag = 0;
    int coords[2];
    for (i = 0; i < size; i++){
        MPI_Cart_coords(comm_cart, i, 2, coords); // get the coordinates of destination

        //calculate the memory start point of corresponding map block of different processor
        int x=0;
        int y=0;
        int curRank;
        for (j=0; j<coords[0]; j++){
            curRank = getPID(j, 0);
            x += Mlist[curRank];
        }
        for (j=0; j<coords[1]; j++){
            curRank = getPID(0, j);
            y += Nlist[curRank];
        }

        //send data to processors
        MPI_Issend(&map[x][y], 1, mapBlock[i], i, tag, comm_cart, &srequest_s[i]);
    }
}

//receive the map block the sent from master
void recvBlock(int **recvData){
    MPI_Irecv(recvData, 1, oldBlock, 0, 0, comm_cart, &rrequest_s);
}

//send old array to master
void sendBlock(int **sendData){
    MPI_Issend(sendData, 1, oldBlock, 0, 0, comm_cart, &srequest_g);
}

//master node receive the old array that sent from all processors
void recvAllBlock(int **map, int M, int N){
    int i,j;
    int tag = 0;
    int coords[2];
    for (i = 0; i < size; i++){
        MPI_Cart_coords(comm_cart, i, 2, coords); // get coordinates

        //calculate the memory start point of corresponding map block of different processor
        int x=0;
        int y=0;
        int curRank;
        for (j=0; j<coords[0]; j++){
            curRank = getPID(j, 0);
            x += Mlist[curRank];
        }
        for (j=0; j<coords[1]; j++){
            curRank = getPID(0, j);
            y += Nlist[curRank];
        }

        // receive data and put them in the correct map block
        MPI_Irecv(&map[x][y], 1, mapBlock[i], i, tag, comm_cart, &rrequest_g[i]);
    }
}

// calculate the neighbour processor
void findNeib(int *rank_up, int *rank_down, int *rank_left, int *rank_right){
    MPI_Cart_shift(comm_cart, 0, 1, rank_up, rank_down);
    MPI_Cart_shift(comm_cart, 1, 1, rank_left, rank_right);
}

// create data type for halo swaps
void createSwapType(int M, int N){
    // rows
    MPI_Type_contiguous( N, MPI_INT, &row);
    MPI_Type_commit(&row);
    //col
    MPI_Type_vector(M, 1, N+2, MPI_INT, &col);
    MPI_Type_commit(&col);
}

// finish the non-blocking send and receive in scatter stage
void scatFinish(){
    int i;
    if (rank == 0){
        for (i = 0; i < size; i++){
            MPI_Wait(&srequest_s[i], &status);
        }
    }
    MPI_Wait(&rrequest_s, &status);
}

// finish the non-blocking send and receive in gather stage
void gathFinish(){
    int i;
    if (rank == 0){
        for (i = 0; i < size; i++){
            MPI_Wait(&rrequest_g[i], &status);
        }
    }
    MPI_Wait(&srequest_g, &status);
}

//swap halo
void swapHalo(int **old, int M, int N, int rank_up, int rank_down, int rank_left, int rank_right){
    int tag = 0;
    //send and recv the top row;
    MPI_Issend(&old[1][1], 1, row, rank_up, tag, comm_cart, &upsrequest);
    MPI_Irecv(&old[M+1][1], 1, row, rank_down, tag, comm_cart, &uprrequest);
    //send and recv the bottom row;
    MPI_Issend(&old[M][1], 1, row, rank_down, tag, comm_cart, &downsrequest);
    MPI_Irecv(&old[0][1], 1, row, rank_up, tag, comm_cart, &downrrequest);
    //send and recv the left col;
    MPI_Issend(&old[1][1], 1, col, rank_left, tag, comm_cart, &leftsrequest);
    MPI_Irecv(&old[1][N+1], 1, col, rank_right, tag, comm_cart, &leftrrequest);
    //send and recv the right col;
    MPI_Issend(&old[1][N], 1, col, rank_right, tag, comm_cart, &rightsrequest);
    MPI_Irecv(&old[1][0], 1, col, rank_left, tag, comm_cart, &rightrrequest);
}

// finish the non-blocking send and receive in halo swap stage
void swapFinish(){
    MPI_Wait(&upsrequest, &status);
    MPI_Wait(&uprrequest, &status);
    MPI_Wait(&downsrequest, &status);
    MPI_Wait(&downrrequest, &status);
    MPI_Wait(&leftsrequest, &status);
    MPI_Wait(&leftrrequest, &status);
    MPI_Wait(&rightsrequest, &status);
    MPI_Wait(&rightrrequest, &status);
}

void getSum(int *nchange){
    int ncSum;
    MPI_Allreduce(nchange, &ncSum, 1, MPI_INT, MPI_SUM, comm_cart);
    *nchange = ncSum;
}

void getMapSum(unsigned long long *partialSum){
    unsigned long long totalSum;
    MPI_Reduce(partialSum, &totalSum, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, comm_cart);
    *partialSum = totalSum;
}

// map coordinates to rank
int getPID(int i, int j){
    int tempRank;
    int coords[2];
    coords[0] = i;
    coords[1] = j;
    MPI_Cart_rank(comm_cart, coords, &tempRank);
    return tempRank;
}

void getTime(double *time)
{
    MPI_Barrier(comm_cart);
    *time = MPI_Wtime();
}

//stop the MPI
void mpstop(void)
{
  MPI_Finalize();
}
