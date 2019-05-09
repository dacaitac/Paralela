#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

#define N 6000
#define NUMTHREADS 80 // Debe ser multiplo de 16


int repa = N/NUMTHREADS;
int res[N][N], A[N][N], B[N][N];

struct params{
	int ini;
	int fin;
};

void *mult( void *pArg){
	struct params *p;
	p = (struct params *)pArg;
	int resp = 0;
	int i,j,k;
	//printf("%d %d\n",p->ini,p->fin);
 	for(i = p->ini; i < p->fin; i++){
 		for(j = 0; j < N; j++){
 			for(k = 0; k < N; k++){
				res[j][i] += A[k][i] * B[j][k];
			}
		}
	}

	return 0;
}

void fill(){
	int i,j;
	for ( i = 0; i < N; ++i ) {
		for ( j = 0; j < N; ++j ) {
			A[i][j]=rand() % 5;
			B[i][j]=rand() % 5;
		}
	}
}

void printeo(int a){
	int i,j;
	for ( i = 0; i < N; ++i ) {
		for ( j = 0; j < N; ++j ) {
			if(a == 1){
				if(res[j][i] >= 10)
					printf("%d ",res[j][i]);
				else
					printf("%d  ",res[j][i]);
			}else if(a == 2){
				if(A[j][i] >= 10)
					printf("%d ",A[j][i]);
				else
					printf("%d  ",A[j][i]);
			}else{
				if(B[j][i] >= 10)
					printf("%d ",B[j][i]);
				else
					printf("%d  ",B[j][i]);
			}
		}
		printf("\n");
	}
	printf("\n\n\n");
}


int main(int argc, char **argv) {
	struct timespec ts1, ts2;

	//printf("Pre Tnum\n");
	pthread_t tHandles[NUMTHREADS];
	int i, j;
	//printf("Pre Struct\n");
	struct params inter[NUMTHREADS];
	int count=0;
	//printf("Pre llenado\n");
	fill();
	//printf("Pre Hilos\n");
	clock_gettime(CLOCK_REALTIME, &ts1);	//printf("Pre Handles\n");
	for ( i = 0; i < NUMTHREADS; i++ ) {
		inter[i].ini=i*repa;
		inter[i].fin=inter[i].ini+repa;
		//printf("%d %d\n\n",inter[i].ini,inter[i].fin);
		pthread_create(&tHandles[i], NULL, mult, (void*)&inter[i]);
	}
	for ( i = 0; i < NUMTHREADS; ++i ) {
		pthread_join(tHandles[i], NULL);
	}

	clock_gettime(CLOCK_REALTIME, &ts2);
  // double secs = (double)(t_fin-t_ini)*1000/CLOCKS_PER_SEC;
  printf("%ld.%09ld\n", (long)(ts2.tv_sec - ts1.tv_sec),
         abs(ts2.tv_nsec - ts1.tv_nsec));
  return 0;
}
