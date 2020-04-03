#include <stdio.h>  //Libreria estandar de C
#include <stdlib.h> //Libreria que permite el manejo de la memoria, entre otras cosas mas
#include <errno.h>  //Define la variable 'int errno' (Error Number)
                    //Es usado por 'System Calls'/Funciones para indicar que 'algo salio mal'
#include <string.h> //Nos da funciones para manejar Strings
#include <netdb.h>  //Deficiones para operacion de base de datos de red
#include <netinet/in.h> //Define in_port_t, in_addr_t, sin_family, sin_port, entre otros.
#include <unistd.h> //Define constantes y declara funciones

#include <sys/types.h>  //Define los tipos de datos como lo es 'id_t'
#include <sys/socket.h> //Define los tipos de datos como lo es 'sockaddr'
#include <sys/wait.h>   //Utiliza constantes para el uso de waitpid();

#include <arpa/inet.h>  //Define operaciones de internet


#define PORT 5500  // Puerto al cual nos conectaremos
#define MAXDATASIZE 100

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

int main( int quantityOfArgumentsReceived, char * listOfArguments[] ){
    printf("Cliente UDP\n");
    int fileDescriptorSocket;
    struct sockaddr_in socketAddress;
    socklen_t socketAddressSize;
    char * messageToSent = listOfArguments[2];
    char messageToReceive[MAXDATASIZE];

    int returnedInteger;
    struct hostent * hostEntry;

    if(quantityOfArgumentsReceived != 3){
        fprintf(stderr, "usage: client hostname and message"); //Lo mandamos al stream de errores
        exit(1);
    }

    hostEntry = gethostbyname(listOfArguments[1]);
    // Devuelve la información acerca de un host con un nombre determinado
    if(hostEntry == NULL){
        reportErrorIfNecessary(-1, "gethostbyname");
    }

    fileDescriptorSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    reportErrorIfNecessary(fileDescriptorSocket, "socket");

    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = htons(PORT);
    socketAddress.sin_addr = *( (struct  in_addr *) hostEntry -> h_addr);
    bzero(&(socketAddress.sin_zero), sizeof(unsigned char) * 8);

    returnedInteger = connect(fileDescriptorSocket, (struct sockaddr *) &(socketAddress), sizeof(struct sockaddr) );
    reportErrorIfNecessary(returnedInteger,"connect");

    returnedInteger = send(fileDescriptorSocket, messageToSent, strlen(messageToSent) * sizeof(char), 0);
    reportErrorIfNecessary(returnedInteger, "send");

    returnedInteger = recv(fileDescriptorSocket, messageToReceive, MAXDATASIZE, 0);

    printf("%s\n", messageToReceive);

    close(fileDescriptorSocket);

    return 0;
}