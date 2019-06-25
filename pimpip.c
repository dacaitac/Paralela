#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

double f( double a )
{
    return (4.0 / (1.0 + a*a));
}

int main(int argc, char** argv){

	int done = 0, n, myid, numprocs, i;
	double PI25DT = 3.141592653589793238462643;
	double mypi, pi, h, sum, x;

	// Initialize the MPI environment
	MPI_Init(NULL, NULL);

	// Get the number of processes
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

	// Get the rank of the process
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

	// Get the name of the processor
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int name_len;
	MPI_Get_processor_name(processor_name, &name_len);

	// Print off a hello world message
	//printf("Processor %s, rank %d out of %d processors\n",processor_name, numprocs, myid);

	n = 0;
	while (!done){
		if (myid == 0)		{
			/*
			printf("Enter the number of intervals: (0 quits) ");
			scanf("%d",&n);
			*/
			if (n==0)
				n=1024*numprocs;
			else
				n=0;
		}
		MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
		if (n == 0)
			done = 1;
		else{
			h   = 1.0 / (double) n;
			sum = 0.0;
			for (i = myid + 1; i <= n; i += numprocs){
				x = h * ((double)i - 0.5);
				sum += f(x);
			}
			mypi = h * sum;

			MPI_Reduce(&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

			if (myid == 0){
				printf("pi is approximately %.16f, Error is %.16f\n",pi, fabs(pi - PI25DT));
			}
		}
	}

	// Finalize the MPI environment.
	MPI_Finalize();
}