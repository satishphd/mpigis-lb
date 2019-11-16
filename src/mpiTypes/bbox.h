#ifndef BBOX_H
#define BBOX_H


#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <stddef.h>


/*
int main1(int argc, char **argv) {

    const int tag = 13;
    int size, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        fprintf(stderr,"Requires at least two processes.\n");
        exit(-1);
    }

    // create a type for struct box 
    const int nitems=5;
    int          blocklengths[5] = {1,1,1,1,1};
    MPI_Datatype types[5] = {MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_INT};
    MPI_Datatype mpi_box_type;
    MPI_Aint     offsets[5];

    offsets[0] = offsetof(box, x1);
    offsets[1] = offsetof(box, x2);
    offsets[2] = offsetof(box, y1);
    offsets[3] = offsetof(box, y2);
    offsets[4] = offsetof(box, id);

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_box_type);
    MPI_Type_commit(&mpi_box_type);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        box send;
        send.x1 = 4.1;
        send.x2 = 4.2;
 		send.y1 = 10.1;
        send.y2 = 10.2;
        send.id = 33;

        const int dest = 1;
        MPI_Send(&send,   1, mpi_box_type, dest, tag, MPI_COMM_WORLD);

        printf("Rank %d: sent structure box\n", rank);
    }
    if (rank == 1) {
        MPI_Status status;
        const int src=0;

        box recv;

        MPI_Recv(&recv, 1, mpi_box_type, src, tag, MPI_COMM_WORLD, &status);
        printf("Rank %d: Received: x1 = %f, x2 = %f, y1 = %f, y2 = %f, id = %d\n", rank,
                 recv.x1, recv.x2, recv.y1, recv.y2, recv.id);
    }

    MPI_Type_free(&mpi_box_type);
    MPI_Finalize();

    return 0;
}*/
#endif