#include <mpi.h>
#include <math.h>
#include <complex>
#include <iostream>
#include <pthread.h>
#include <sys/time.h>

double wtime()
{
	struct timeval t;
	gettimeofday(&t, NULL);
	return t.tv_sec + t.tv_usec / 1000000.0;
}

using namespace std;

int main(int argc, char *argv[])
{
	int rank, size;
	// double start_time = wtime();
	int max_row, max_column, max_n, qtde_por_thread;
	int vec_rows, my_r, my_maxr;

	MPI_Comm comm = MPI_COMM_WORLD;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(comm, &rank);
	MPI_Comm_size(comm, &size);

	/* Preciso informar as dimensões para todos */
	// for para trabalhadores
	if (rank == 0)
	{
		cin >> max_row;
		cin >> max_column;
		cin >> max_n;
	}
	MPI_Bcast(&max_row, 1, MPI_INT, 0, comm);
	MPI_Bcast(&max_column, 1, MPI_INT, 0, comm);
	MPI_Bcast(&max_n, 1, MPI_INT, 0, comm);
	qtde_por_thread = ceil((double)max_row / size);

	printf("[%*d] - r%d c%d n%d q%d\n", (int)ceil(log10(size)), rank, max_row, max_column, max_n, qtde_por_thread);
	// fiz :D

	char **mat = new char *[max_row];
	char *tmp = new char[max_row * max_column * 2];

	for (int i = 0; i < max_row * 2; i++)
		mat[i] = &tmp[i * max_column];

	my_r = rank * qtde_por_thread;
	my_maxr = min(max_row, (rank + 1) * qtde_por_thread);

	for (int r = my_r; r < my_maxr; r++)
	{
		for (int c = 0; c < max_column; c++)
		{
			printf("oi [%d] r%d c%d\n", rank, r, c);
			fflush(stdout);
			complex<float> z;
			int n = 0;
			while (abs(z) < 2 && ++n < max_n)
				z = (z * z) + complex<float>((float)c * 2 / max_column - 1.5, (float)r * 2 / max_row - 1);
			mat[r][c] = (n == max_n ? '#' : '.');
		}
	}
	// double finish_time = wtime();
	printf("tchau [%d]\n", rank);
	fflush(stdout);

	MPI_Gather(mat[0], qtde_por_thread * max_column, MPI_CHAR, mat[0], qtde_por_thread * max_column, MPI_CHAR, 0, comm);
	printf("tchau2 [%d]\n", rank);
	fflush(stdout);

	if (rank == 0)
	{
		printf("tchau3 [%d]\n", rank);
		fflush(stdout);
		for (int r = 0; r < max_row; ++r)
		{
			for (int c = 0; c < max_column; ++c)
				printf("%c", mat[r][c]);
			printf("\n");
		}
		// printf("%f\n", finish_time - start_time);
		printf("tchau4 [%d]\n", rank);
		fflush(stdout);
	}

	printf("tchau5 [%d]\n", rank);
	fflush(stdout);
	MPI_Finalize();
	printf("tchau6 [%d]\n", rank);
	fflush(stdout);
	return 0;
}