#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

#define MAXDATASIZE 100
#define MAXMESSAGES 1024

typedef char shm_message[MAXDATASIZE];

struct shmSegment{
    int first, last, count;
    shm_message messages[MAXMESSAGES];
};

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

    int segmentID = 0;
    struct shmSegment * sharedMemoryAddress = NULL;

    key_t shmKey = ftok("shm_server.c", (int) 'A');

    segmentID = shmget( shmKey, (size_t) sizeof(struct shmSegment), IPC_EXCL | S_IRUSR | S_IWUSR );
    reportErrorIfNecessary(segmentID, "shmget");

    sharedMemoryAddress = (struct shmSegment *) shmat(segmentID, NULL, 0);/*El null es para decirle que el sistema nos asigne
                                                                            la dirección*/

    printf("Shared Memory attached at address %p\n", sharedMemoryAddress);

    while(1){
    
        if( sharedMemoryAddress->count > 0 ){
            printf("New Message: %s\n", sharedMemoryAddress->messages[sharedMemoryAddress->last]);
            sharedMemoryAddress->last = (sharedMemoryAddress->last + 1) % MAXMESSAGES ;
            sharedMemoryAddress->count--;
        }
        else sleep(5);
    
    }

}