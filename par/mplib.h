#ifndef MPLIB_H
#define MPLIB_H

#include <stdio.h>
#include <mpi.h>
#include "arralloc.h"

int rank,size;
int *Mlist;
int *Nlist;

MPI_Comm comm_cart;

MPI_Datatype *mapBlock;
MPI_Datatype oldBlock;

MPI_Request *srequest_s;
MPI_Request rrequest_s;
MPI_Request *rrequest_g;
MPI_Request srequest_g;

MPI_Datatype row,col;
MPI_Status status;

MPI_Request upsrequest, uprrequest, downsrequest, downrrequest;
MPI_Request leftsrequest, leftrrequest, rightsrequest, rightrrequest;

#endif
