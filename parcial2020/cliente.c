/*

------------------------------------
|              CLIENTE             |
------------------------------------
Autor: Lucas Ariel Saavedra
Nro-Registro: 15-180013-1

Para correrlo, ingrese en la linea de comandos:

./cliente nombreDeHost numeroDePuerto

*/

#include "custom.h"

int main(int cantidadDeArgumentos, char * listaDeArgumentos[]){
    int descriptorDeSocket; //El descriptor de archivo para el socket

    struct hostent * informacionDeHost;  //Es una estructura para guardar informacion de red sobre un host determinado
    struct sockaddr_in direccionDeSocket;//Guardamos la familia, el puerto, y la dirección a la cual vamos a vincular al socket
    struct sockaddr_in direccionLocal;   //Solo se usará para obtener el numero de puerto que el cliente esta utilizando para poder comunicarse
                                         //con el servidor

    int enteroRetornado; //Es simplemente para control de errores
    int primeraVez = 1;
    solicitud solicitudAEnviar;//Aca guardamos información acerca de la solicitud que el cliente le manda al servidor
    char * extremo = NULL; //Guardaremos el string de <extremo> que se encuentra en la solicitud
    char puerto[6]; //Guardaremos el numero de puerto para luego verificar que es correcto
    char mensajeAEnviar[LONGITUD_MAX_DE_DATOS]; //Es un string que contiene solo los datos que se va a enviar. Recordar que nunca guardamos una solicitud acá adentro
    char mensajeRecibido[LONGITUD_MAX_DE_DATOS];//Es un string que contiene todo lo que recibimos del servidor
    struct timeval tiempoUno; //Es una estructura para especificar un intervalo de tiempo
    struct timeval tiempoDos; //Es una estructura para especificar un intervalo de tiempo
    long tiempoTransacurrido; //Aca se guardará la estimación del viaje de ida y de vuelta de los mensajes entre cliente-servidor
    
    socklen_t tamanoDeDireccionDeSocket; //Se guardará (a continuación) el tamaño de la dirección del socket (el tamaño del sockaddr).
    tamanoDeDireccionDeSocket = sizeof(struct sockaddr_in);

    //Se verifica que se haya ingresado un nombre de host y puerto
    if(cantidadDeArgumentos != 3){
        fprintf(stderr, "Uso: Ingresar como argumento el nombre del host a la cual nos queremos conectar y luego el numero de puerto\n"); //Lo mandamos al stream de errores
        exit(1);
    }

    int numeroDePuerto = (int) atoi(listaDeArgumentos[2]);  //Convertimos (si es que se puede) el numero de puerto que se paso como argumento
                                                            //de string a entero

    //Corroboramos que el puerto ingresado solo contenta numeros y sea un numero de puerto realmente válido
    if(  !(contieneSoloNumeros(listaDeArgumentos[2]) && esPuertoValido(numeroDePuerto)) ){
        fprintf(stderr, "ERROR: Ingrese un numero de puerto válido\n"); //Lo mandamos al stream de errores
        exit(1);        
    }
    
    //Imprimimos un cartel de bienvenida al cliente
    imprimirMensajeDeBienvenidaCliente();

    //Obtenemos la información acerca del host (en nuestro caso el servidor) que paso el usuario como argumento
    informacionDeHost = gethostbyname(listaDeArgumentos[1]);

    //En el caso de que gethostbyname() no haya encontrado al host que le pasamos, devolverá NULL.
    //Por eso, este 'if' se fija de que no haya habido errores. En el caso de lo que los haya, reportará el error.
    if(informacionDeHost == NULL){
        //Le pasamos -1 para que reporte error si o si y el nombre de la función donde tuvimos el error.
        reportarSiEsNecesario(-1, "gethostbyname");
    }
    
    /*
        Pasaremos a obtener el descriptor de archivo del socket. 
        Se pasan tres parámetros a esta función:

        1) Familia: Le indicamos la familia de protocolos que vamos a usar. En nuestro caso, AF_INET (hace referencia a IPV4).
        2) Tipo: Le indicamos el tipo de servicio que usaremos. En nuestro caso, SOCK_STREAM ofrece una comunicación secuencial, confiable y de dos vias.
        3) Protocolo: Especifica el tipo de protocolo a ser usado en el socket. Con 0, indicamos que usamos TCP.
    */

    descriptorDeSocket = socket(AF_INET, SOCK_STREAM, 0);//
    //socket() devuelve -1 en caso de error. reportarSiEsNecesario() justamente reporta un error en el caso de que una función haya fallado
    reportarSiEsNecesario(descriptorDeSocket, "socket");

    /*Procedemos a configurar el sockaddr_in*/

    direccionDeSocket.sin_family = AF_INET;//Especificamos la familia a utilizar
    
    /*
        Se esepecifica el numero de puerto en la cual el cliente se va a querer conectar en el servidor.
        Se utiliza htons() para que el orden de los bytes pase del orden de bytes del host al orden de bytes de la red.
        Es de vital importancia ya que en la red el orden en que los bytes se envian es esencial
    */
    
    direccionDeSocket.sin_port = htons(numeroDePuerto);
    direccionDeSocket.sin_addr = *( (struct  in_addr *) informacionDeHost -> h_addr);//Guardamos la dirección IPV4 del servidor
    bzero(&(direccionDeSocket.sin_zero), sizeof(unsigned char) * 8);//Limpiamos el buffer

    //A continuacion guardamos un intervalo de tiempo determinado para luego estimar el tiempo de ida y de vuelta de los mensajes
    gettimeofday(&tiempoUno, 0);
    /*
        Tratamos de lograr una conexión con el servidor
        ¿Por la estructura sockaddr_in tuvo que ser casteada a sockaddr?
        Primero, se utiliza sockaddr_in por cuestiones de comodidad. Y segundo, a la función hay que pasarle si o si una estrucutra del tipo sockaddr.    
    */

    enteroRetornado = connect( descriptorDeSocket, (struct sockaddr *) &direccionDeSocket, sizeof(struct sockaddr) ); //Conecta el socket
    gettimeofday(&tiempoDos, 0);//Registramos el intervalo de tiempo despues de haber realizado (o no) la conexión
    reportarSiEsNecesario(enteroRetornado, "connect");//Reportamos error en caso de ser necesario

    /*Calculamos el tiempo transcurrido (en milisegundos) entre que el cliente envió la solicitud de conexión
    y el servidor responde.*/
    tiempoTransacurrido = ((tiempoDos.tv_sec-tiempoUno.tv_sec)*1000000 + tiempoDos.tv_usec-tiempoUno.tv_usec)/1000;

    printf("Conexión exitosa...\n");//Le informamos al cliente que la conexióń con el servidor es correcta

    //Empezamos a llenar la sollicitud que vamos a enviar
    recibirNombreDeUsuarioPorTeclado(&(solicitudAEnviar.nombreDeUsuario));//Le pedimos al cliente que ingrese un nombre de usuario
    recibirTipoDeSolicitudPorTeclado(&(solicitudAEnviar.tipoDeSolicitud));//Le pedimos al cliente que ingrese un tipo de solicitud
    
    //Utilizamos esta función para obtener la dirección IP del cliente y el numero de puerto que está utilizando
    getsockname(descriptorDeSocket, (struct sockaddr *) &direccionLocal, &tamanoDeDireccionDeSocket);
    //Para guardar el string del extremo, vamos a conseguir el espacio de memoria suficiente para guardar el numero de puerto y la dirección IP
    solicitudAEnviar.extremo = malloc((strlen(inet_ntoa(direccionLocal.sin_addr))+ 5 + 1) * sizeof(char));

    strcpy(solicitudAEnviar.extremo,inet_ntoa(direccionLocal.sin_addr));//Guardamos la dirección IP en el extremo de la solicitud

    //Convertimos el numero de puerto a entero y convertimos el orden de bytes de red al orden de bytes del cliente
    sprintf(puerto, "%d", ntohs(direccionLocal.sin_port));

    strcat(solicitudAEnviar.extremo,"-");//Concatenamos '-' para luego concatenar el puerto ya convertido en string
    strcat(solicitudAEnviar.extremo,puerto);

    solicitudAEnviar.TIVms = malloc(sizeof(char) * 7);//Alojamos la cantidad suficiente de memoria para guardar el string sobre el timepo estimado

    sprintf(solicitudAEnviar.TIVms, "%ldms", tiempoTransacurrido);//Convertimos el tiempo estimado en string

    enviarSolicitudAServidor(descriptorDeSocket, solicitudAEnviar);//Enviamos la solicitud (esta función convierte la solicitud en string y lo manda)
    enteroRetornado = recv(descriptorDeSocket,mensajeRecibido,LONGITUD_MAX_DE_DATOS * sizeof(char), 0);   //Recibimos los datos del servidor y verificamos que no hayan errores y si no hay, 
                                                                                        //se procede a mostrar el saludo
    reportarSiEsNecesario(enteroRetornado, "recv");//Reportamos error en caso de ser necesario

    verificarError(mensajeRecibido, &solicitudAEnviar, descriptorDeSocket);//Verificamos que el servidor haya confirmado que los datos son correcto

    /*Ahora dependeremos de lo que el cliente haya elegido.
    
        -Si eligió la solicitud 'msj', simplemente podra registrar mensajes en el servidor. Para cortar el envio de mensajes,
        escribe simplemente -1 que hace referencia al EOF. Luego se le pedirá enviar otra solicitud.
        -Si eligió la solicitud 'log', le pide al servidor que reporte el historial de mensajes que hubo con este cliente.
        En el caso de que no haya ningun registro, se le informará y se le pedirá solicitud de nuevo.
        -Si eligió la solicitud 'fin', se cierra conexión con el servidor y el cliente termina.
    
    */

    //Se hara este bucle siempre y cuando la solicitud no sea del tipo "fin"
    while ( strcmp(solicitudAEnviar.tipoDeSolicitud,"fin") != 0 )
    {

        if(primeraVez){
            primeraVez = 0;
            printf("%s\n",mensajeRecibido);//Imprimimos la respuesta del servidor (si se llega a estas instancias, se imprime OK)
        }

        //Se elige que hacer segun lo elegido por el cliente
        if ( strcmp(solicitudAEnviar.tipoDeSolicitud,"msj") == 0 )
            msj_cliente(descriptorDeSocket);//Ejecutamos esta función para realizar la operacion "msj" del lado cliente
        else if( strcmp(solicitudAEnviar.tipoDeSolicitud,"log") == 0 )
            log_cliente(descriptorDeSocket);//Ejecutamos esta función para realizar la operacion "log" del lado cliente

        recibirTipoDeSolicitudPorTeclado(&(solicitudAEnviar.tipoDeSolicitud));//Una vez terminadas las solicitudes anteriores, le pedimos al cliente que ingrese otra solicitud
        enviarSolicitudAServidor(descriptorDeSocket, solicitudAEnviar);//Enviamos la solicitud de nuevo
        enteroRetornado = recv(descriptorDeSocket,mensajeRecibido,LONGITUD_MAX_DE_DATOS * sizeof(char), 0);   //Recibimos los datos del servidor y verificamos que no hayan errores y si no hay, 
                                                                                                                //se procede a mostrar el saludo
        reportarSiEsNecesario(enteroRetornado, "recv");//Reportamos error en caso de ser necesario

        verificarError(mensajeRecibido, &solicitudAEnviar, descriptorDeSocket);//Verificamos que el servidor haya confirmado que los datos son correcto
    }

    /*
        Iniciamos el proceso de desconexión. Con el simbolo SHUT_RDWE, logramos que no se pueda
        ni recibir ni mandar mas información sobre el socket al cliente.
    */   
    shutdown(descriptorDeSocket,SHUT_RDWR);
    close(descriptorDeSocket);
    /*
        Una vez cerrada la conexión, volvemos a accept() para esperar otra solicitud de conexión de otro
        cliente
    */

    return 0;
}
