#include <complex>
#include <iostream>
#include <pthread.h>
#include <sys/time.h>

double wtime() {
  struct timeval t;
  gettimeofday(&t, NULL);
  return t.tv_sec + t.tv_usec / 1000000.0;
}

using namespace std;

int main(){
	// double start_time = wtime();
	int max_row, max_column, max_n;
	cin >> max_row;
	cin >> max_column;
	cin >> max_n;

	char **mat = new char*[max_row];

	for (int i=0; i<max_row;i++)
		mat[i] = new char[max_column];

	#pragma omp parallel for schedule(dynamic)
	for (int r = 0; r < max_row; ++r){
		for(int c = 0; c < max_column; ++c){
			complex<float> z;
			int n = 0;
			while(abs(z) < 2 && ++n < max_n)
				z = (z * z) + complex<float>((float)c * 2 / max_column - 1.5, (float)r * 2 / max_row - 1);
			mat[r][c]=(n == max_n ? '#' : '.');
		}
	}
	// double finish_time = wtime();

	for (int r = 0; r < max_row; ++r){
		for(int c = 0; c < max_column; ++c)
			printf("%c", mat[r][c]);
		printf("\n");
	}
	// printf("%f\n", finish_time - start_time);
}