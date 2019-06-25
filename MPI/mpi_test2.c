//mpicc mpi_test2.c -o mpi_test2 -lm
//

#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <math.h>
#define MSG_LENGTH 15
main (int argc, char *argv[]){
    int i, tag=1, tasks, iam;
    double x;
    char message[MSG_LENGTH];
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int namelen;
    MPI_Status status;
    /* Initialize the message passing system, get the number of nodes,
    and find out which node we are. */
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &tasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &iam);

    if (iam == 0) {
        /* Root node sends message to everyone. */
        strcpy(message, " Hello, world!");
        for (i=1; i<tasks; i++)
            MPI_Send(message, MSG_LENGTH, MPI_CHAR, i, tag, MPI_COMM_WORLD);    
    } 
    else {
        /* Receive message from root. */
        MPI_Recv(message, MSG_LENGTH, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);
        MPI_Get_processor_name(processor_name, &namelen);
        printf("\n-->%s", processor_name);
        for(i = 0; i < 3e08; i++)
            x = x + sin(i);
    }
    printf(" node %d: %s \n", iam, message);
    /* Shut down the message passing system. */
    MPI_Finalize();
}
