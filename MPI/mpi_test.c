// test de lam mpi
// compilar con  mpirun -np 2 mpi_test
// ejecutar primero lamboot -d lambhost.def, que tiene la lista de los nodos con sis ip
// ejecutar con mpirun -np 4 mpi_test
// detener con lamhalt

#include <stdio.h>
#include <string.h>
#include <mpi.h>

#define MSG_LENGTH 15

main (int argc, char *argv[]){
    int i, tag=1, tasks, iam;
    char message[MSG_LENGTH];
    MPI_Status status;

    /* Initialize the message passing system, get the number of nodes,
    and find out which node we are. */

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &tasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &iam);
    if (iam == 0) {
        /* Root node sends message to everyone. */
        strcpy(message, "Hello, world!");
        for (i=1; i<tasks; i++)
            MPI_Send(message, MSG_LENGTH, MPI_CHAR, i, tag, MPI_COMM_WORLD);
    }
    else {
        /* Receive message from root. */
        MPI_Recv(message, MSG_LENGTH, MPI_CHAR, 0, tag,
        MPI_COMM_WORLD, &status);
    }
    printf("node %d: %s\n", iam, message);

    /* Shut down the message passing system. */

    MPI_Finalize();

}

