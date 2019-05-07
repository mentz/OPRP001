#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int rank, size, i;
    int *sndbuffer, recvbuffer;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    sndbuffer = (int *)malloc(size * sizeof(int));

    if (rank == 0)
    {
        for (i = 0; i < size; i++)
        {
            sndbuffer[i] = i * i;
            printf("(%d) - Generated %d \n", rank, sndbuffer[i]);
        }
    }
    MPI_Scatter(sndbuffer, 1, MPI_INT, &recvbuffer, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank != 0)
    {
        printf("(%d) - Received %d\n", rank, recvbuffer);
    }

    MPI_Finalize();
    return 0;
}