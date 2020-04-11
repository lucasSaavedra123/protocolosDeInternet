#include <stdio.h>  
#include <stdlib.h> 
#include <errno.h>  
#include <string.h> 
#include <netdb.h>  
#include <netinet/in.h> 
#include <unistd.h> 
#include <sys/types.h>  
#include <sys/socket.h> 
#include <arpa/inet.h>

#define BACKLOG 5
#define MAXDATASIZE 100

//Si value es igual a -1, nos da un error y cierra el programa
void reportErrorIfNecessary(int value, char * string){
    if(value == -1){
        perror(string);
        exit(1);
    }
}

//Lo usamos para tomar datos desde el teclado
void receiveMessageFromKeyboard(char * string){

    int quantityOfCharacters = 0;

    char character = getchar();
    fflush(stdin);

    string[0] = '\0';


    while(character != 10 && quantityOfCharacters < MAXDATASIZE-1){
        string[quantityOfCharacters] = character;

        fflush(stdin);
        character = getchar();
        
        quantityOfCharacters++;
    }

    string[quantityOfCharacters] = '\0';

}

int isValidPortnumber(int portNumber){
    int isValid = 0;

    if(portNumber >= 0 && portNumber <= 65535)
        isValid = 1;

    return isValid;
}

void validateInputs(int portNumber){

    if(!isValidPortnumber(portNumber)){
        fprintf(stderr, "invalid port: Enter a valid port number (0->65535)");
        exit(1);
    }

}

int main(int quantityOfArgumentsReceived, char * listOfArguments[]){

    //Server Data
    char * ipAddressString = NULL;
    int portNumber = 0;

    //Socket handling data
    int returnedInteger = 0;
    int listeningSocketDescriptor = 0;
    int clientSocketDescriptor = 0;
    struct sockaddr_in listeningSocketAddress;
    struct in_addr clientSocketAddress;
    int processID = 0;
    socklen_t socketAddressSize;

    //Server Data
    char messageToSend[MAXDATASIZE];
    char messageToReceive[MAXDATASIZE];

    if(quantityOfArgumentsReceived != 2){
        fprintf(stderr, "usage: port number");
        exit(1);
    }

    portNumber = atoi(listOfArguments[1]); //Converts port number string to int

    validateInputs(portNumber);
    
    listeningSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);

    listeningSocketAddress.sin_family = AF_INET;
    listeningSocketAddress.sin_port = htons(portNumber);
    listeningSocketAddress.sin_addr.s_addr = INADDR_ANY;
    bzero(&(listeningSocketAddress.sin_zero), sizeof(unsigned char) * 8);

    printf("Preparing Server...\n");

    returnedInteger = bind(listeningSocketDescriptor, (struct sockaddr *) &listeningSocketAddress, sizeof(struct sockaddr) );
    reportErrorIfNecessary(returnedInteger,"bind");

    returnedInteger = listen(listeningSocketDescriptor, BACKLOG);
    reportErrorIfNecessary(returnedInteger,"bind");

    socketAddressSize = sizeof(struct sockaddr_in);

    printf("Server Ready...\n");


    while(1){

        clientSocketDescriptor = accept(listeningSocketDescriptor, (struct sockaddr *) &clientSocketAddress, &socketAddressSize );
        reportErrorIfNecessary(clientSocketDescriptor, "accept");

        processID = fork();

        if(processID != 0){
            printf("User connected...\n");

            strcpy(messageToSend,"Hi Client! I'll talk with you! Type 'disc' to close connection. You'll close if I want\n");            
            send(clientSocketDescriptor, messageToSend, strlen(messageToSend), 0);
            reportErrorIfNecessary(returnedInteger, "send");

            while ( strcmp(messageToSend, "disc") != 0  || strcmp(messageToReceive, "disc") != 0  )
            {   
                returnedInteger = recv(clientSocketDescriptor,  messageToReceive, MAXDATASIZE, 0);
                reportErrorIfNecessary(returnedInteger, "recv");
                printf("User: %s", messageToReceive);
                
                printf("Type:");
                receiveMessageFromKeyboard(messageToSend);
                returnedInteger = send(clientSocketDescriptor,  messageToSend, strlen(messageToSend), 0);
                reportErrorIfNecessary(returnedInteger, "send");

            }
        }
        else{
            close(clientSocketDescriptor);
        }



    }

    printf("Server Close...\n");
    returnedInteger = close(listeningSocketDescriptor);
    reportErrorIfNecessary(returnedInteger, "close");


    return 0;
}