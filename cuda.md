#  Programar en CUDA

1. Declarar punteros
2. Reservar memoria  -> *a, *b, *c
3. Inicializar memoria
4. Copiar a Device
5. Ejecutar Kernel -> a, b, c
6. Copiar a Host
7. Liberar memoria

kernel<<N,M>> Un bloque es un conjunto de hilos que uno quiere ejecutar en un mulptiprocesador

N bloques cada uno de M hilos
 ### Un bloque no sale de ejecucion de un multiprocesador hasta que la ejecucion no se haya terminado 
 #### -> No existen los cambios de contexto en los bloques.

# Colab
 Notebook -> Entorno de Ejecucion
|--> Python3
|--> GPU
Los codigos estan en el sitio