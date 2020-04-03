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

    socklen_t socketAddressSize;

    char messageReceived[MAXDATASIZE];

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
        
        recv(fileDescriptorSocket, messageReceived, MAXDATASIZE, 0);
        printf("%s", messageReceived);

    }

    close(fileDescriptorSocket);

    return 0;
}