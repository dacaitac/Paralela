#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

#define NUM_RECT 300000000
#define NUMTHREADS 16
double gPi = 0.0;  //  global accumulator for areas computed

void *area(void *pArg){
   int myNum = *((int *)pArg);
   double h = 2.0 / NUM_RECT;
   double partialSum = 0.0, x;  // local to each thread

   // use every NUMTHREADS-th ste
   for (int i = myNum; i < NUM_RECT; i += NUMTHREADS){
     x = -1 + (i + 0.5f) * h;
     partialSum += sqrt(1.0 - x*x) * h;
   }
   gPi += partialSum;  // add partial to global final answer
   return 0;
}

int main(int argc, char **argv) {
  struct timespec ts1, ts2;
  clock_gettime(CLOCK_REALTIME, &ts1);

  pthread_t tHandles[NUMTHREADS];
  int tNum[NUMTHREADS], i;

  for ( i = 0; i < NUMTHREADS; ++i ) {
    tNum[i] = i;
    pthread_create(&tHandles[i], NULL, area, (void*)&tNum[i]);
  }
  for ( i = 0; i < NUMTHREADS; ++i ) {
     pthread_join(tHandles[i], NULL);
  }
  gPi *= 2.0;
  printf("Computed value of Pi:  %12.9f\n", gPi );
  clock_gettime(CLOCK_REALTIME, &ts2);
  // double secs = (double)(t_fin-t_ini)*1000/CLOCKS_PER_SEC;
  printf("%ld.%09ld\n", (long)(ts2.tv_sec - ts1.tv_sec),
         ts2.tv_nsec - ts1.tv_nsec);  return 0;
}
