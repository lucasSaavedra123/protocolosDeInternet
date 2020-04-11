
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

int main(){
    printf("Servidor Iterativo UDP\n");

    int fileDescriptorSocket;

    struct sockaddr_in socketAddress;
    struct sockaddr_in clientAddress;

    socklen_t addressSize = sizeof(struct sockaddr_in);;

    char messageReceived[MAXDATASIZE];
    char messageToSent[MAXDATASIZE];

    int hostAddressSize;
    int returnedInteger;
    
    fileDescriptorSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    reportErrorIfNecessary(fileDescriptorSocket,"socket");

    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = htons(PORT);
    socketAddress.sin_addr.s_addr  = inet_addr("127.0.0.1"); //Esto quiere decir que acepta cualquier dirección
    bzero( &(socketAddress.sin_zero), sizeof(unsigned char) * 8 );

    returnedInteger = bind(fileDescriptorSocket, (struct sockaddr *) &(socketAddress), sizeof(struct sockaddr) );
    reportErrorIfNecessary(returnedInteger,"bind");

    while(1){
	memset(messageReceived, 0, MAXDATASIZE);

        returnedInteger = recvfrom(fileDescriptorSocket, messageReceived, MAXDATASIZE, 0, (struct sockaddr *) &(clientAddress), (socklen_t *) &addressSize );
        reportErrorIfNecessary(returnedInteger, "recvfrom");
	
	printf("%ld\n", strlen(messageReceived) );

        printf("Message received from (%s): %s\n", inet_ntoa(clientAddress.sin_addr), messageReceived);

        printf("Type answer: ");
        receiveMessageFromKeyboard(messageToSent);

        returnedInteger = sendto(fileDescriptorSocket, messageToSent, MAXDATASIZE, 0, (struct sockaddr *) &(clientAddress), addressSize);
        reportErrorIfNecessary(returnedInteger, "sendto");

	/*
		Se podria haber usado "connect()" que lo que hace es vincular la dirección del cliente de manera que pueda usar el send para mandar
		mensajes. Si hace connect(cliente)->send(mensaje)->connect(clienteDeReset)

		El "clienteDeReset" es un cliente que tiene la dirección todo en 0. Si no lo hacemos, no nos permite recibir/mandar mensajes
		de otro cliente.

		Preferí usar recvfrom y sendto por que en realidad en UDP no hacemos una conexión.
	*/


    }

    close(fileDescriptorSocket);

    return 0;
}
