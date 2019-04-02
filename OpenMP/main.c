#include <stdout.h>
#include <omp.h>

int main(int argc, char **argv) {
	int tid;
	/* Fork a team of threads giving them their own copies of variables */
	#pragma omp parallel private(tid)
	{
		/* Obtain thread number */
		tid = omp_get_thread_num();
		printf("");

	printf("Hello World\n");
	return 0;
}
