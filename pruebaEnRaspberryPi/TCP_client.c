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

#define MAXDATASIZE 100

//Si value es igual a -1, nos da un error y cierra el programa
void reportErrorIfNecessary(int value, char * string){
    if(value == -1){
        perror(string);
        exit(1);
    }
}

//Lo usamos para tomar datos desde el teclado
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

int isThereAnyInvalidCharacterInString(char * string){
    int isThere = 0;
    int stringLength = strlen(string);
    int stringIndex;
    int characterNumber = 0;

    while (stringIndex < stringLength)
    {   
        characterNumber = (int) string[stringIndex];
        
        if( characterNumber <= 45 || characterNumber == 47 || (characterNumber > 58) )
            isThere = 1;

        stringIndex++;
    }

    return isThere;
}

//Lo realizamos para validar la dirección IP ingresada por el usuario
int isValidIpAddress(char * ipAddressString){
    int isValid = 1;
    int ipAddressByte = 0;
    int quantityOfBytes = 0;

    char * ipAddressByteString = NULL;
    char ipAddressStringCopy[50];

    strcpy(ipAddressStringCopy, ipAddressString);

    if(isThereAnyInvalidCharacterInString(ipAddressStringCopy)){
        isValid = 0;
        return isValid;
    }

    ipAddressByteString = strtok(ipAddressStringCopy, ".");

    while(ipAddressByteString != NULL){
        quantityOfBytes++;

        ipAddressByte = atoi(ipAddressByteString);

        if(ipAddressByte < 0 || ipAddressByte > 255)
            isValid = 0;

        ipAddressByteString = strtok(NULL, ".");
    }

    if(quantityOfBytes != 4)
        isValid = 0;

    return isValid;
}

int isValidPortnumber(int portNumber){
    int isValid = 0;

    if(portNumber >= 0 && portNumber <= 65535)
        isValid = 1;

    return isValid;
}

void validateInputs(char * ipAddressString, int portNumber){
    
    if(!isValidIpAddress(ipAddressString)){
        fprintf(stderr, "invalid IP: Enter a valid IPV4 address (X.X.X.X)");
        exit(1);
    }
    else if(!isValidPortnumber(portNumber)){
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
    int socketDescriptor = 0;
    struct sockaddr_in socketAddress;
    struct in_addr auxiliarSinAddr;

    //Client Data
    char messageToSend[MAXDATASIZE];
    char messageToReceive[MAXDATASIZE];

    if(quantityOfArgumentsReceived != 3){
        fprintf(stderr, "usage: server IPV4 address and port number");
        exit(1);
    }

    ipAddressString = listOfArguments[1];
    portNumber = atoi(listOfArguments[2]); //Converts port number string to int

    validateInputs(ipAddressString, portNumber);
    
    socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);

    returnedInteger =  inet_aton(ipAddressString, &(auxiliarSinAddr) );
    reportErrorIfNecessary(returnedInteger, "inet_aton");

    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = htons(portNumber);
    socketAddress.sin_addr = auxiliarSinAddr;
    bzero(&(socketAddress.sin_zero), sizeof(unsigned char) * 8);

    printf("Trying to connect...\n");
    returnedInteger = connect(socketDescriptor, (struct sockaddr *) &socketAddress, sizeof(struct sockaddr) );
    reportErrorIfNecessary(returnedInteger, "connect");

    returnedInteger = recv(socketDescriptor,  messageToReceive, MAXDATASIZE, 0);
    reportErrorIfNecessary(returnedInteger, "send");

    printf("Server: %s", messageToReceive);

    strcpy(messageToReceive, "");

    while ( strcmp(messageToReceive, "disc") != 0 )
    {
        printf("Type:");
        receiveMessageFromKeyboard(messageToSend);
        returnedInteger = send(socketDescriptor,  messageToSend, strlen(messageToSend), 0);
        reportErrorIfNecessary(returnedInteger, "send");
        
        returnedInteger = recv(socketDescriptor,  messageToReceive, MAXDATASIZE, 0);
        reportErrorIfNecessary(returnedInteger, "recv");
        printf("Server: %s", messageToReceive);

    }
    
    printf("Trying to disconnect...\n");
    returnedInteger = close(socketDescriptor);
    reportErrorIfNecessary(returnedInteger, "close");
    return 0;
}