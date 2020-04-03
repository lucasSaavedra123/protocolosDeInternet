#include <studio.h>
#include <stdlib.h>


//Servidores Concurrentes


void main(){
    int sockfd, new_fd; //sockfd es para escuchar
                        //new_fd es para nuevas conexiones entrantes

    struct sockaddr_in my_addr;
    struct sockaddr_in their_addr;
    socklen_t sin_size;
    int numbytes;
    char buf[MAXDATASIZE];
    

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket");
        exit(1);
    }

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(MYPORT); //El S.O asigna el puerto y se traduce
    my_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(my_addr.sin_zero),0);

    if(bind(sockfd,(struct sockaddr *) &my_addr, sizeof(struct sockaddr)) == -1){
        perror("bind");
        exit(1);
    }

    if(listen(sockfd, BACKLOG) == -1){
        perror("listen");
        exit(1);
    }

    while(1){
        sin_size = sizeof(struct sockaddr_in);

        if((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1){
            perror("accept");
            continue;
        }

        print("server: got connection from %s\n", inet_ntoa(their_addr.sin_addr));

        if(!fork()){
            
        }




    }




    
}