#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

#define NUM_RECT 100000000
#define NUM_THREADS 8
#define PAD 8

int main(){  
	struct timespec ts1, ts2;
	clock_gettime(CLOCK_REALTIME, &ts1);

	int h = NUM_RECT / NUM_THREADS;
	double gpi = 0.0;  //  global accumulator for areas computed
	double pi[NUM_THREADS][PAD];
	#pragma omp parallel num_threads(NUM_THREADS)
	{ 
	int th = omp_get_thread_num();

		for(int i = th*h; i < (th + 1)*h; i++){
			if(i % 2 == 0){
			  pi[th][0] += 4.0/(2.0*i + 1);
			}
			else
			{
			  pi[th][0] += -4.0/(2.0*i + 1);
			}

		}
	}
	for(int i = 0; i < NUM_THREADS; i++){
		gpi += pi[i][0];		
	}

	clock_gettime(CLOCK_REALTIME, &ts2);	
	printf("%ld.%09ld\n", (long)(ts2.tv_sec - ts1.tv_sec), ts2.tv_nsec - ts1.tv_nsec);

	printf("Computed value of Pi:  %12.9f\n", gpi );
	return 0;
}


