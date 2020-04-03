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

#define PORT 4700
#define MAXDATASIZE 100

void reportErrorIfNecessary(int value, char * string){
    if(value == -1){
        perror(string);
        exit(1);
    }
}

int main(int quantityOfArgumentsReceived, char * listOfArguments[]){
    printf("Cliente TCP\n");

    int fileDescriptorSocket;

    //struct hostent es para poder representar un 'record', 'entry', o entrada en la base de datos del host
    struct hostent * hostEntry;
    struct sockaddr_in socketAddress;

    char messageReceive[MAXDATASIZE];

    int returnedInteger; //Lo uso para controlar errores
    int quantityOfBytesReceived;

    if(quantityOfArgumentsReceived != 2){
        fprintf(stderr, "usage: client hostname"); //Lo mandamos al stream de errores
        exit(1);
    }

    hostEntry = gethostbyname(listOfArguments[1]);
    // Devuelve la información acerca de un host con un nombre determinado

    if(hostEntry == NULL){
        reportErrorIfNecessary(-1, "gethostbyname");
    }

    fileDescriptorSocket = socket(AF_INET, SOCK_STREAM, 0);
    reportErrorIfNecessary(fileDescriptorSocket, "socket");

    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = htons(PORT);
    socketAddress.sin_addr = *( (struct  in_addr *) hostEntry -> h_addr);
    bzero(&(socketAddress.sin_zero), sizeof(unsigned char) * 8);

    returnedInteger = connect( fileDescriptorSocket, (struct sockaddr *) &socketAddress, sizeof(struct sockaddr) ); //Conecta el socket
    reportErrorIfNecessary(returnedInteger, "connect");
	
    //Con 'disc' el servidor nos cierra la conexión
    while( strcmp(messageReceive, "disc") != 0 ){

        quantityOfBytesReceived = recv(fileDescriptorSocket, messageReceive, MAXDATASIZE, 0);
        reportErrorIfNecessary(returnedInteger, "recv");

        messageReceive[quantityOfBytesReceived] = '\0';

        printf("Message Received: %s [%d bytes]\n", messageReceive, strlen(messageReceive));

    }

    printf("Connection Closed!\n");
    close(fileDescriptorSocket);

    return 0;
}
