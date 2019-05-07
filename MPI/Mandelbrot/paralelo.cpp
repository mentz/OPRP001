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
	int max_row, max_column, max_n;
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
		my_r = 0;
		my_maxr = (max_row / size);
	}
	MPI_Bcast(&max_row, 1, MPI_INT, 0, comm);
	MPI_Bcast(&max_column, 1, MPI_INT, 0, comm);
	MPI_Bcast(&max_n, 1, MPI_INT, 0, comm);

	printf("[%*d] - r%d c%d n%d\n", (int)ceil(log10(size)), rank, max_row, max_column, max_n);
	// fiz :D

	char **mat = new char *[max_row];
	char *tmp = new char[max_row * max_column];

	for (int i = 0; i < max_row; i++)
		mat[i] = &tmp[i * max_column];

	for (int i = 1; i < size; i++)
	{
		int seu_r = (max_row / size) * i;
		int seu_maxr = (max_row / size) * (i + 1);
		MPI_Send(&seu_r, 1, MPI_INT, i, 0, comm);
		MPI_Send(&seu_maxr, 1, MPI_INT, i, 0, comm);
	}

	for (int r = my_r; r < my_maxr; r++)
	{
		for (int c = 0; c < max_row; c++)
		{
			complex<float> z;
			int n = 0;
			while (abs(z) < 2 && ++n < max_n)
				z = (z * z) + complex<float>((float)c * 2 / max_column - 1.5, (float)r * 2 / max_row - 1);
			mat[r][c] = (n == max_n ? '#' : '.');
		}
	}
	// double finish_time = wtime();

	for (int r = 0; r < max_row; ++r)
	{
		for (int c = 0; c < max_column; ++c)
			printf("%c", mat[r][c]);
		printf("\n");
	}
	// printf("%f\n", finish_time - start_time);

	MPI_Finalize();
	return 0;
}