#include <mpi.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int rank, size;
    int tag = 0, i;
    int token = -1;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (rank == 0)
    {
        token = 0;
        MPI_Send(&token, 1, MPI_INT, (rank + 1) % size, tag, MPI_COMM_WORLD);
        printf("[%2d] Enviei token %d\n", rank, token);
        MPI_Recv(&token, 1, MPI_INT, size - 1, tag, MPI_COMM_WORLD, &status);
        printf("[%2d] Recebi token %d\n", rank, token);
        printf("Anel finalizado. Soma: %d\n", token);
    }
    else
    {
        MPI_Recv(&token, 1, MPI_INT, (rank - 1) % size, tag, MPI_COMM_WORLD, &status);
        printf("[%2d] Recebi token %d\n", rank, token);
        token += rank;
        MPI_Send(&token, 1, MPI_INT, (rank + 1) % size, tag, MPI_COMM_WORLD);
        printf("[%2d] Enviei token %d\n", rank, token);
    }

    MPI_Finalize();
    return 0;
}