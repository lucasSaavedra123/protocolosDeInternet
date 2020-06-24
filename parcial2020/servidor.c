/*

------------------------------------
|     SERVIDOR ITERATIVO TCP       |
------------------------------------
Autor: Lucas Ariel Saavedra
Nro-Registro: 15-180013-1

Los servidores iterativos son aquellos que manejan un solicitud a la vez

En este trabajo, hemos implementado un servidor iterativo.

Para correrlo, ingrese en la linea de comandos:

./servidor numeroDePuerto

*/

#include "custom.h"

/*AGREGAR QUE HACER AL TOCAR CTRL+C*/

int main(int cantidadDeArgumentos, char * listaDeArgumentos[]){
    int descriptorDelSocketServidor;    //El descriptor de archivo para el socket que va estar escuchando a solicitudes de conexión
    int descriptorDelSocketCliente;     //El descriptor de archivo para el socket asociado al cliente

    struct sockaddr_in direccionDeSocket; //Guardamos la familia, el puerto, y la dirección a la cual vamos a vincular al socket en estado "modo pasivo" (escuchando solicitudes)
    struct sockaddr_in direccionDelSocketCliente; //Guardamos la familia, el puerto, y la dirección a la cual vamos a vincular al socket del cliente

    socklen_t tamanoDeDireccionDeSocket;//Se guardará (a continuación) el tamaño de la dirección del socket (el tamaño del sockaddr).

    /*Declaramos los strings que vamos a utilizar para guardar datos que vamos a enviar/recibir como las solicitudes*/
    char solicitudEnString[LONGITUD_MAX_DE_SOLICITUD];
    char mensajeARecibir[LONGITUD_MAX_DE_DATOS];
    char mensajeAEnviar[LONGITUD_MAX_DE_DATOS];
    char mensajeAMostrar[LONGITUD_MAX_DE_DATOS];

    char ** parametros;//Es una arreglo de strings. Guardaremos los parametros de la solicitud

    int enteroRetornado; //Se usa simplemente para controlar errores
    int numeroDePuerto = 0;//Lo usamos para guardar el numero de puerto que el usuario del servidor ingreso. Dicho puerto va a ser utilizado para poner al servidor en marcha.
    solicitud solicitudDelCliente;//Guardamos las solicitudes de los clientes en este struct

    /*
        Como es necesario haber ingresado correctamente el numero de puerto para poder poner el servidor en marcha,
        vamos a verificar que los argumentos pasados por parametro sean correctos.

        Primero se verifica que se hayan ingreado correctamente 1 solo argumento (hay dos pero el primero esno es de importancia)
    */
    if( cantidadDeArgumentos != 2){
        fprintf(stderr, "Uso: Ingresar como argumento el puerto en la cual vamos a querer estar escuchando solicitudes de conexion\n"); //Lo mandamos al stream de errores
        exit(1);
    }
    
    //Convertimos (de ser posible) el numero de puerto
    numeroDePuerto = (int) atoi(listaDeArgumentos[1]);

    //Verificamos que el argumentos tenga todos caracteres de numeros y que el numero convertido sea realmente un numero de puerto válido
    if(  !(contieneSoloNumeros(listaDeArgumentos[1]) && esPuertoValido(numeroDePuerto)) ){
        fprintf(stderr, "ERROR: Ingrese un numero de puerto válido\n"); //Lo mandamos al stream de errores
        exit(1);        
    }

    imprimirMensajeDeBienvenidaServidor();//Se muestra un mensaje de bienvenida en el lado del servidor

    printf("Configurando servidor...\n");

    /*

        Pasaremos a obtener el descriptor de archivo del socket. 
        Se pasan tres parámetros a esta función:

        1) Familia: Le indicamos la familia de protocolos que vamos a usar. En nuestro caso, AF_INET (hace referencia a IPV4).
        2) Tipo: Le indicamos el tipo de servicio que usaremos. En nuestro caso, SOCK_STREAM ofrece una comunicación secuencial, confiable y de dos vias.
        3) Protocolo: Especifica el tipo de protocolo a ser usado en el socket. Con 0, indicamos que usamos TCP.
    
    */
    descriptorDelSocketServidor = socket(AF_INET, SOCK_STREAM, 0);
    reportarSiEsNecesario(descriptorDelSocketServidor, "socket");//reportamos error de ser necesario

    direccionDeSocket.sin_family = AF_INET;//Especificamos la familia a utilizar
    
    /*
        Se esepecifica el numero de puerto en la cual el cliente se va a querer conectar en el servidor.
        Se utiliza htons() para que el orden de los bytes pase del orden de bytes del host al orden de bytes de la red.
        Es de vital importancia ya que en la red el orden en que los bytes se envian es esencial
    */
    
    direccionDeSocket.sin_port = htons(numeroDePuerto);

    /*
        La constante INADDR_ANY le permite al programa trabajar sin saber la dirección IP de la maquina o el caso de trabajar con
        una maquina que trabaje con multiples interfaces de red.
    */

    direccionDeSocket.sin_addr.s_addr = INADDR_ANY;
    bzero(&(direccionDeSocket.sin_zero), sizeof(unsigned char) * 8);//Limpiamos el buffer

    //Vinculamos la estructura sockadd_in al socket. Le estamos dando "identidad".
    enteroRetornado = bind( descriptorDelSocketServidor, (struct sockaddr *) &(direccionDeSocket), sizeof(struct sockaddr) );
    reportarSiEsNecesario(enteroRetornado, "bind");

    enteroRetornado = listen(descriptorDelSocketServidor, BACKLOG); //Ponemos en modo pasivo al servidor y se empieza a recibir solicitudes de conexión
    reportarSiEsNecesario(enteroRetornado, "listen");

    //En realidad es "tamaño". Pero para evitar problemas de caracteres "especiales", se introdució tamano
    tamanoDeDireccionDeSocket = sizeof(struct sockaddr_in);//Indicamos el tamaño de la estructura sockaddr_in. Se usa por que es necesario pasarlo en accept.

    while(1){
        printf("Esperando nueva conexion...\n");//Representamos en el servidor que estamos esperando una conexión
        
        /*
            Aceptamos una conexión entrante. accept() va armando una cola de solicitudes en el caso de que hayan conexiones entrantes pendientes.
            Crea un socket para cada conexión entrante y devuelve el descriptor
        */
        descriptorDelSocketCliente = accept( descriptorDelSocketServidor, (struct sockaddr * ) &direccionDelSocketCliente, &tamanoDeDireccionDeSocket ); //Aceptamos una conexión con el socket
        reportarSiEsNecesario(descriptorDelSocketCliente, "accept");//Reportamos error de ser necesario
        
        printf("Cliente conectado...\n");//Informamos que un cliente se conectó

        //Esperamos a que el cliente mande nombre de usuario y solicitud
        //De paso, obtenemos los parametros que pasó con el simple hecho de verificar que esté todo correcto
        recibirSolicitud(descriptorDelSocketCliente,solicitudEnString,&parametros);

        //Analizamos los parametros en busca de errores
        enteroRetornado = analizarParametros(parametros, descriptorDelSocketCliente);   
        armarEstructuraDeSolicitud(&solicitudDelCliente,parametros);//Armamos la estructura
        
        /*
            Mientras el cliente no haya mandado la solicitud 'fin', vamos a trabajar
            sobre las solicitudes "msj"/"log". En el caso de que haya ocurrido un error, con enteroRetornado
            nos aseguraremos de cerrar la conexión con el cliente y que todo este correcto del lado servidor.
        */
        
        while( strcmp(solicitudDelCliente.tipoDeSolicitud,"fin") != 0 && enteroRetornado != -1){
            if( strcmp(solicitudDelCliente.tipoDeSolicitud,"msj") == 0)
                msj_servidor(descriptorDelSocketCliente, solicitudDelCliente.nombreDeUsuario); //Ejecutamos esta función para realizar la operacion "msj" del lado servidor
            else if( strcmp(solicitudDelCliente.tipoDeSolicitud,"log") == 0)
                log_servidor(descriptorDelSocketCliente, solicitudDelCliente); //Ejecutamos esta función para realizar la operacion "log" del lado servidor

            recibirSolicitud(descriptorDelSocketCliente,solicitudEnString,&parametros); //Recibimos una solicitud de nuevo
            enteroRetornado = analizarParametros(parametros, descriptorDelSocketCliente); //Analizamos los parametros de nuevo
            armarEstructuraDeSolicitud(&solicitudDelCliente,parametros);//Armamos de nuevo la estrcutura
            bzero(mensajeAEnviar, LONGITUD_MAX_DE_DATOS * sizeof(char));//Limpiamos el buffer del mensaje que vamos a enviar
        }

        armarMensajeDeDesconexion(mensajeAMostrar, solicitudDelCliente);//Armamos el mensaje que vamos a mostrar por pantalla
        printf("%s\n", mensajeAMostrar);//Mostramos el mensaje en el servidor

        /*
            Iniciamos el proceso de desconexión. Con el simbolo SHUT_RDWE, logramos que no se pueda
            ni recibir ni mandar mas información sobre el socket al cliente.
        */
        
        shutdown(descriptorDelSocketCliente,SHUT_RDWR);
        close(descriptorDelSocketCliente);
        /*
            Una vez cerrada la conexión, volvemos a accept() para esperar otra solicitud de conexión de otro
            cliente
        */

    }

    return 0;
}
