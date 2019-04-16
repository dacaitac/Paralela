#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

#define NUM_RECT 600000000

int main ( ) {
  clock_t t_ini, t_fin;
  double secs;

  double mid, height, width, area, x;
  double partialSum = 0.0;

  double h = 2.0 / NUM_RECT;
  t_ini = clock();
  for (long i = 0; i < NUM_RECT; i++) {
    x = -1 + (i + 0.5f) * h;
    partialSum += sqrt(1.0 - x*x) * h;
  }

  // area = width * sum;
  t_fin = clock();
  secs = (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
  printf("%.16g milisegundos\n", secs * 1000.0);
  printf("%f\n", partialSum);
}
