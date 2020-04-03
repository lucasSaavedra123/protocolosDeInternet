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

#define PORT 4500
#define BACKLOG 5 //Define el largo maximo de la cola de conexiones pendientes del servidor
#define MAXDATASIZE 100

void reportErrorIfNecessary(int value, char * string){
    if(value == -1){
        perror(string);
        exit(1);
    }
}

int main(){
    printf("Servidor Concurrente TCP\n");

    int fileDescriptorSocket;
    int fileDescriptorNewClientSocket;

    struct sockaddr_in serverSocketAddress;
    struct sockaddr_in newClientSocketAddress;

    socklen_t socketAddressSize;

    char messageToSent[] = "Hello World!";
    char messageReceive[MAXDATASIZE];

    int returnedInteger; //Lo uso para controlar errores
    int processId;
    int quantityOfBytesReceived;

    //El cero indica que solamente un solo protocolo estará usando este socket
    fileDescriptorSocket = socket(AF_INET, SOCK_STREAM, 0);
    reportErrorIfNecessary(fileDescriptorSocket, "socket");

    serverSocketAddress.sin_family = AF_INET;
    serverSocketAddress.sin_addr.s_addr = INADDR_ANY;
    serverSocketAddress.sin_port = htons(PORT);
    bzero(&(serverSocketAddress.sin_zero), sizeof(unsigned char) * 8);

    //Se podria haber usado
    //memset(&(serverSocketAddress.sin_zero), 0, sizeof(unsigned char) * 8); 
    //Con memset llenamos un espacio de memoria con una constante

    returnedInteger = bind( fileDescriptorSocket, (struct sockaddr *) &(serverSocketAddress), sizeof(struct sockaddr) );
    reportErrorIfNecessary(returnedInteger, "bind");
    
    returnedInteger = listen(fileDescriptorSocket, BACKLOG); //Ponemos en modo pasivo al servidor
    reportErrorIfNecessary(returnedInteger, "listen");

    while(1){

        socketAddressSize = sizeof(struct sockaddr_in);

        fileDescriptorNewClientSocket = accept( fileDescriptorSocket, (struct sockaddr * ) &newClientSocketAddress, &socketAddressSize ); //Aceptamos una conexión con el socket
        reportErrorIfNecessary(fileDescriptorNewClientSocket, "accept");
        printf("Server: I've got connection from: %s\n", inet_ntoa(newClientSocketAddress.sin_addr)); //inet_ntoa convierte la direcion IP
                                                                                                      //que esta orden de la red
                                                                                                      //en un string con la notación X.X.X.X
        
        processId = fork();

        if(processId != 0){
            returnedInteger = send(fileDescriptorNewClientSocket, messageToSent, strlen(messageToSent) * sizeof(char), 0); //Utilizamos send para poder mandar un mensaje sobre un socket
            reportErrorIfNecessary(returnedInteger, "send");
            printf("Message Sent!\n");

            //Utilizamos recv() para recibir mensajes que llegan a un socket
            //Devuelve la cantidad de bytes recibidos

            quantityOfBytesReceived = recv(fileDescriptorNewClientSocket, messageReceive, MAXDATASIZE, 0);

            reportErrorIfNecessary(quantityOfBytesReceived, "recv");

            messageReceive[quantityOfBytesReceived] = '\0';

            printf("Message Received: %s\n", messageReceive);
            printf("\n");
        }
        else{
            close(fileDescriptorNewClientSocket);
        }

        while(waitpid(-1, NULL, WNOHANG) > 0);
        //Espera que un proceso cambie de estado
    }

    return 0;
}