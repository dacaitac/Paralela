#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

struct params{
  double start;
  double end;
};

long NUM_RECTS = 200000;
int CPU_CORES = 8;

double* sum( void *arg){
  double mid, height, suma;
  double width = 2.0 / (double) NUM_RECTS;

  struct params *p;
  p = (struct params *)arg;
  suma = 0.0;

  for (double i = p->start; i < p->end; i+=CPU_CORES) {
    mid = (i ) * width;
    height = 4.0 / (1.0 + mid * mid);
    suma += height;
  }

  double* area = (double*) malloc(sizeof(double));
  *area = suma;
  printf("%lf\n", suma);
  return area;
}

int main ( ) {
  clock_t t_ini, t_fin;
  double secs;
  double pi = 0.0;
  double width = 1.0 / (double) NUM_RECTS;

  pthread_t threads[CPU_CORES];

  t_ini = clock();
  // Compute in parts
  for (int i = 0; i < CPU_CORES; i++) {
    struct params p1;
    p1.start = i * NUM_RECTS / CPU_CORES;
    p1.end = (i + 1) * NUM_RECTS / CPU_CORES;
    pthread_create (&threads[i] , NULL , sum , (void *)&p1);
  }

  double sum = 0.0;
  for (int i = 0; i < CPU_CORES; i++) {
    double *s;
    pthread_join( threads[i], &s ) ;
    sum += *s;
  }

  t_fin = clock();
  secs = (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
  printf("%.16g milisegundos\n", secs * 1000.0);
  printf("%s\n", "--------------");
  pi = width * sum;
  printf("%lf\n", pi);
  printf("%s\n", "FIN");

}
