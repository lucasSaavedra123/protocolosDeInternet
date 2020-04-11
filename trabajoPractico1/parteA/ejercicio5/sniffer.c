#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>



#define MAXDATASIZE 100
#define MAXMESSAGES 1024

#define MAXDATASIZE     100
#define MAXMESSAGES     1024
#define MAXALIASSIZE    15

int segmentID = 0;

typedef char shm_message[MAXDATASIZE];

struct shm_register{
    char alias[MAXALIASSIZE];
    shm_message message;
};

struct shmSegment{
    int first, last, count;
    struct shm_register registers[MAXMESSAGES];
};

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
    key_t shmKey = ftok("sniffer.c", (int) 'A');

    char alias[MAXALIASSIZE];
    char message[MAXDATASIZE];

    /*Esta parte es opcional pero establecemos que hacer al tocar CTRL+C */
    memset( &(signalAction), 0, sizeof(struct sigaction) );
    signalAction.sa_handler = &Free_SHM;
    sigaction(SIGINT, &signalAction, NULL);



    segmentID = shmget( shmKey, (size_t) sizeof(struct shmSegment), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR );
    reportErrorIfNecessary(segmentID, "shmget");

    sharedMemoryAddress = (struct shmSegment *) shmat(segmentID, NULL, 0);/*El null es para decirle que el sistema nos asigne
                                                                            la dirección*/

    printf("Shared Memory attached at address %p\n", sharedMemoryAddress);

    while(1){
        if( sharedMemoryAddress->count > 0 ){
            strcpy(alias,(sharedMemoryAddress->registers[sharedMemoryAddress->last]).alias );
            strcpy( message,(sharedMemoryAddress->registers[sharedMemoryAddress->last]).message );

            printf("%s:%s\n", alias, message);
            sharedMemoryAddress->last++;
            sharedMemoryAddress->count--;
        }
        else sleep(5);
    }

    Free_SHM(0);
}