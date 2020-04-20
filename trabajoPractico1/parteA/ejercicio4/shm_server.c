#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

#define MAXDATASIZE 100
#define MAXMESSAGES 1024

typedef char shm_message[MAXDATASIZE];

int segmentID = 0;
int flag = 1;

struct shmSegment{
    int first, last, count;
    shm_message messages[MAXMESSAGES];
};

/*Recibe un 'signal' del sistema operativo y libera la memoria compartida*/
void Free_SHM(int signalNumber){
    printf("\nFree SHM...\n");
    shmctl(segmentID, IPC_RMID, 0);
    exit(1);
}

void reportErrorIfNecessary(int value, char * string){
    if(value == -1){
        perror(string);
        exit(1);
    }
}

void receiveMessageFromKeyboard(char * string){

    int index = 0;

    char character = getchar();
    fflush(stdin);

    while(index < MAXDATASIZE && character != 10){
        string[index] = character;
        
        fflush(stdin);
        character = getchar();
        
        index++;
    }

    string[index] = '\0';

}

void main(){
    struct sigaction signalAction;

    struct shmSegment * sharedMemoryAddress = NULL;

    key_t shmKey = ftok("shm_server.c", (int) 'A');

    /*Esta parte es opcional pero establecemos que hacer al tocar CTRL+C */
    memset( &(signalAction), 0, sizeof(struct sigaction) );
    signalAction.sa_handler = &Free_SHM;
    sigaction(SIGINT, &signalAction, NULL);

    segmentID = shmget( shmKey, (size_t) sizeof(struct shmSegment), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR );
    reportErrorIfNecessary(segmentID, "shmget");

    sharedMemoryAddress = (struct shmSegment *) shmat(segmentID, NULL, 0);/*El null es para decirle que el sistema nos asigne
                                                                            la dirección*/

    printf("Shared Memory attached at address %p\n", sharedMemoryAddress);


    //Tomamos control de lo que hemos escrito
    sharedMemoryAddress->first  = 0;
    sharedMemoryAddress->last   = 0;
    sharedMemoryAddress->count  = 0;


    strcpy( sharedMemoryAddress->messages[0], "Hello World!\0" );
    sharedMemoryAddress->count++;

    while(1){
        if(sharedMemoryAddress->count < MAXMESSAGES){
            sharedMemoryAddress->first = (sharedMemoryAddress->first+1) % MAXMESSAGES;  //Logra que first
                                                                                        //vaya de 0 a MAXMESSAGES
                                                                                        //y luego vuelva a 0
            printf("Type message number %d:", sharedMemoryAddress->first);
            receiveMessageFromKeyboard(sharedMemoryAddress->messages[sharedMemoryAddress->first]);
            sharedMemoryAddress->count++;
        }
    }

    Free_SHM(0);
}