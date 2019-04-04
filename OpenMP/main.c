#include <stdio.h>
#include <omp.h>

int main(int argc, char **argv) {
	// Fork a team of threads giving them their own copies of variables

	// Argumentos para omp parallel:
	//	private(A) -> leva ao escopo da Thread uma cópia da variável A sem o valor
	//	firstprivate(B) -> leva ao escopo da Thread uma cópia da variável B incluindo valor
	/*
	int tid;
	int x;
	int y = 10;
	int z;
	#pragma omp parallel private(tid, x, z) firstprivate(y)
	{
		// Obtain thread number
		tid = omp_get_thread_num();
		printf("Hello from thread #%2d: x = %d; y = %d\n", tid, x, y);
		#pragma omp master
		printf("Oi, sou o mestre, o thread #%2d\n", tid);
	}
	*/

	// Fork a team of threads
	int n = 10000, i;
	double a[10000], b[10000], sum;

	#pragma omp parallel for num_threads(8)
	for (i = 0; i < n; i++) {
		a[i] = b[i] = i * 1.0;
	}
	sum = 0.0;

	// Aqui tem concorrência no sum
	// #pragma omp parallel for num_threads(8)
	// for (i = 0; i < n; i++) {
	// 	sum = sum + (a[i] + b[i]);
	// }

	// Usar o reduction faz: variável privada pra cada um, realiza as operações da thread
	//   e depois unifica com a operação definida dentro dos parênteses
	#pragma omp parallel for reduction(+: sum) num_threads(8)
	for (i = 0; i < n; i++) {
		sum = sum + (a[i] * b[i]);
	}

	printf("sum = %.0f\n", sum);

	return 0;
}
