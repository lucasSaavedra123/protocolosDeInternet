/*
    Nombre de archivo: custom.h
    Autor: Lucas Ariel Saavedra
    Nro de Registro: 15-180013-1

    Descripcion: Se declaran todas las funciones esenciales para el funcionamiento
    del servidor-cliente del parcial de Protocolos De Internet 2020 - 1er Cuatrimestre


*/

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
#include <sys/stat.h>	//Define estrcuturas necesarias
#include <arpa/inet.h>  //Define operaciones de internet
#include <time.h>	//Se la utiliza para manejar contadores de tiempo
#include <sys/time.h> //Igual que el anterior
#include <ctype.h>	//Define operaciones basicas para strings

#define BACKLOG 1 //Define el largo maximo de la cola de conexiones pendientes del servidor
//Definimos las longitudes maximas de los valores
#define LONGITUD_MAX_DE_DATOS 100
#define LONGITUD_MAX_DE_NOMBRE_DE_USUARIO 64
#define LONGITUD_MAX_DE_SOLICITUD (41 + 64)
#define LONGITUD_MAX_DE_EXTREMO 21 
#define TRUE 1 //Lo usamos como variables "booleaneas"
#define FALSE 0 //Lo usamos como variables "booleaneas"

typedef int boolean;

struct solicitud_estructura{
    char * tipoDeSolicitud;
    char * nombreDeUsuario;
    char * extremo;
    char * TIVms;
} typedef solicitud;

boolean esNombreDeUsuarioValido(char *);
boolean contieneCaracteresNoVisualizables(char *);
boolean esUnCaracterVisualizable(char);
boolean esTipoDeSolicitudValido(char *);
boolean esPuertoValido(int);
boolean esUnNumero(char);
boolean contieneSoloNumeros(char *);
boolean esExtremoValido(char *);
boolean sonCorrectosLosParametros(char **);

void reportarSiEsNecesario(int, char *);
void recibirMensajesDesdeElTeclado(char **);
void registrarMensaje(char *, char *);
void obtenerParametrosDeSolicitud(char ***,char *);
void recibirNombreDeUsuarioPorTeclado(char **);
void recibirTipoDeSolicitudPorTeclado(char **);
void convertirSolicitudEnString(char *,solicitud);
void enviarSolicitudAServidor(int, solicitud);
void msj_cliente(int);
void msj_servidor(int, char *);
void log_servidor(int, solicitud);
void log_cliente(int);
void imprimirMensajeDeBienvenidaCliente();
void imprimirMensajeDeBienvenidaServidor();
void verificarError(char *, solicitud *, int);
void recibirSolicitud(int, char *, char ***);
void armarEstructuraDeSolicitud(solicitud *, char **);
void armarSaludo(char *, solicitud);
void armarMensajeDeDesconexion(char *, solicitud);
int analizarParametros(char **, int);

boolean esNombreDeUsuarioValido(char * nombreDeUsuario){
    /*
        Toma como entrada un string y devuelve FALSE en el caso que no sea valido
        para tomarlo como nombre de usuario. TRUE en caso contrario.
    */
    boolean esValido = TRUE;//Variable que devolvemos

    if( strlen(nombreDeUsuario) > LONGITUD_MAX_DE_NOMBRE_DE_USUARIO ) //Si el largo es mayor del permitido, entonces no es valido
        esValido = FALSE;//Igualamos la variable a FALSE
    else if( contieneCaracteresNoVisualizables(nombreDeUsuario) )//Si contiene variables que no son visualizables, entonces no es valido
        esValido = FALSE;//Igualamos la variable a FALSE

    return esValido;//Devolvemos la variable esValido
}

boolean contieneCaracteresNoVisualizables(char * string){
    /*
        Toma como entrada un string y devuelve FALSE en el caso de que no haya caracteres no visualizables.
        TRUE en caso contrario.
    */
    int indice = 0;//Usamos esta variable para recorrer el string
    char caracter = 0;//Guardamos el caracter que estamos leyendo
    boolean contiene = FALSE;//Variable que devolveremos

    caracter = string[indice];//Guardamos el primer caracter del string en 'caracter'

    /*   
        Mientras el caracter que leemos sea distinto al '\0' (fin de string)m
        seguimos tomando todos los caracteres
    
    */
    while( caracter != '\0' ){
        if( !esUnCaracterVisualizable(caracter) )//Si es caracter no es visulizable, entonces hay caracteres de este tipo
            contiene = TRUE;//
        
        indice++;//Seguimos moviendonos en el string
        caracter = string[indice];//Guardamos el caracter en la posicion "indice" y la usamos para ver si hay que seguir o no en el while
    }

    return contiene;//Devuelve la variable contiene
}

boolean esUnCaracterVisualizable(char caracter){
    /*
        Toma como entrada un caracter y devuelve FALSO en el caso de que no sea visualizable.
        TRUE en caso contrario.
    */
    
    boolean esVisualizable = FALSE;//Variable que devolveremo  s
    /*
        La siguiente condicion verifica que el caracter esté entre 32 y 254 en la codificación ASCII.
        Entre 32 y 254 se encuentran todos los caracteres visibles.
    */
    if( caracter >= 32 && caracter <= 254 )
        esVisualizable = TRUE;

    return esVisualizable;//Devolvemos esVisualizable
}

boolean esTipoDeSolicitudValido(char * tipoDeSolicitud){
    /*
        Toma como entrada un caracter y devuelve FALSO en el caso de que no corresponda a un tipo de solicitud.
        TRUE en caso contrario.
    */
    
    boolean esValido = FALSE;//Variable a retornar

    /*
        Será muy estricto el uso de tipo de solicitudes en minuscula.
        A continuacion se compara el tipo de solicitud con los conocidos.
    */

    if(strcmp("msj", tipoDeSolicitud) == 0)//Si "msj", es válido
        esValido = TRUE;//Se guarda TRUE en esValido
    else if(strcmp("log", tipoDeSolicitud) == 0)//Si "log", es válido
        esValido = TRUE;//Se guarda TRUE en esValido
    else if(strcmp("fin", tipoDeSolicitud) == 0)//Si "fin", es válido
        esValido = TRUE;//Se guarda TRUE en esValido

    return esValido;//Se retorna esValido
}

boolean esPuertoValido(int numero){
    /*
        Toma como entrada un numero entero y devuelve TRUE en el caso que sea un numero de puerto valido.
        Devuelve FALSE en caso contrario.
    */
    
    boolean esValido = TRUE;//Variable a devolver

    /*
        Si el numero de puerto esta entre 0 y 65535, es valido.
    */

    if(numero < 0 || numero > 65535)//Si se encuentra fuera del rango dicho anteriormente, no es valido
        esValido = FALSE;//Se guarda FALSE en esValido

    return esValido;//Se retorna variable esValido
}

boolean esUnNumero(char caracter){
    /*
        Toma como entrada un caracter y devuelve TRUE si el caracter corresponde a un numero.
        FALSE en caso contrario.
    */

    boolean esNumero = FALSE;//Variable a devolver

    /*
        Los numeros en el codigo ASCII se encuentran entre 48 y 57.
    */

    if( caracter >= 48 && caracter <= 57 )//Si el caracter se encuentra en este intervalo, es válido
        esNumero = TRUE;//Guarda TRUE en esNumero

    return esNumero;//Retornamos es esNumero
}

boolean contieneSoloNumeros(char * string){
    /*
        Toma como entrada un string y devuelve TRUE en el caso de que solo haya numeros.
        FALSE en caso contrario.
    */
    boolean contiene = TRUE;//Variable a devolver
    int indice = 0;//Indice que usaremos para recorrer el string

    //Recorremos el string en el while
    while( string[indice] != '\0' ){
        /*Si el caracter no es un numero, entonces contiene caracteres que no son numeros*/
        if( !esUnNumero(string[indice]) )
            contiene = FALSE;//Guarda FALSE en contiene
        
        indice++;//Se incremente el indice para desplazarnos en el string
    }
    return contiene;//Devuelve contiene
}

boolean esExtremoValido(char * extremo){
    /*
        Toma como entrada un string y devuelve TRUE si corresponde a un formato correcto de 
        "extremo" de la solicitud. Devuelve FALSE en caso contrario.
    */
    boolean esValido = TRUE;//Variable a devolv er
    int enteroRetornado;//Variable que tendremos encuenta al usar inet_pton
    int puertoEnEntero;//Numero de puerto utilizado

    char extremoCopia[LONGITUD_MAX_DE_EXTREMO];
    strcpy(extremoCopia,extremo);

    char * direccionIPEnString = strtok(extremoCopia, "-");//Con strtok tomamos partes de un string segun una "clave".
                                                      //En nuestro caso es "-". Tomamos la IP en este caso
    char * numeroDePuerto = strtok(NULL,"-");//Aca tomamos al numero de puerto en string
    char auxiliar[25];//Cadena de caracteres que la usamos solamente para el inet_pton. No tiene importancia. 

    enteroRetornado = inet_pton(AF_INET, direccionIPEnString, auxiliar);//Si la cadena de caracteres corresponde a una IP,
                                                                        //inet_pton devuelve 1.
    puertoEnEntero = atoi(numeroDePuerto);//Convertimos la cadena de caracteres en un entero

    if(enteroRetornado != 1)//Si no hubo éxito con inet_pton, no es valido el extremo
        esValido = FALSE;//Se guarda FALSE en esValido
    else if( !(contieneSoloNumeros(numeroDePuerto) && esPuertoValido(puertoEnEntero)) )//Si en el puerto hay letras y tampoco corresponde a un puerto correcto, no es valido el extremo
        esValido = FALSE;//Se guarda FALSE en esValido
    
    return esValido;//Devolvemos esValido
}

boolean sonCorrectosLosParametros(char ** parametros){
    /*
        Toma como entrada una lista de parametros en char y devuelve TRUE en el caso de que todos sean validos
        y que la cantidad sea valida. FALSE en caso contrario. Recordar la importancia del orden de los parametros
        en la solicitud.
    */
    boolean sonCorrectos = TRUE;//Variable a devolver

    int indice = 0;//Indice que usaremos para iterar en la lista de parametros
    int cantidadDeParametros = 0;//Variable que usaremos para controlar la cantidad de parametros existentes

    while (parametros[indice][0] != '\0' )//Recorremos la lista
        indice++;//Nos vamos desplazando en la lista
    
    cantidadDeParametros = indice;//El indice termina siendo igual a la cantida de parametros

    //Si la cantidad de parametros (incluyendo al encabezado "ParcialTL2020") es distinto de 5, los parametros
    //se consideraran incorrectos.
    if (cantidadDeParametros != 5){
        sonCorrectos = FALSE;//Guardamos FALSE en las variables sonCorrectos
    }
    //En caso de que sean 5, analizamos si cada una es valida
    else
    {
        if( strcmp("ParcialTL2020", parametros[0]) != 0 )//Si el encabezado no tiene ParicalTL2020, los parametros son incorrectos
            sonCorrectos = FALSE;//Guardamos FALSE en sonCorrectos
        else if( !esTipoDeSolicitudValido(parametros[1]) )//Si el primer parametro no es un tipo de solicitud valido, los parametros son incorrectos
            sonCorrectos = FALSE;//Guardamos FALSE en sonCorrectos
        else if( !esNombreDeUsuarioValido(parametros[2]) )//Si el segundo parametro no es un nombre de usuario valido, los parametros son incorrectos
            sonCorrectos = FALSE;//Guardamos FALSE en sonCorrectos
        else if( !esExtremoValido(parametros[3]) )//Si el segundo tercer parametro no es un tipo de extremo valido, los parametros son incorrectos
            sonCorrectos = FALSE;//Guardamos FALSE en sonCorrectos
    }

    return sonCorrectos;//Retornamos sonCorrectos
}

void reportarSiEsNecesario(int valor, char * string){
    /*Se tomará el valor de error como -1. No retorna nada, simplemente cierra el proceso
    en el caso de que haya habiado un error con respecto a system calls importantes*/
    if(valor == -1){
        perror(string);
        exit(1);
    }
}

void recibirMensajesDesdeElTeclado(char ** string){
    /*
        Permite que el usuario escriba mensajes desde el teclado. Se usará para mandar mensajes
        que van a ser registrados por el servidor. Se guardará lo escrito en el puntero pasado por parametro
    */

    int index = 0;//Variable que se usará como indice para recorrer la cadena de caracteres
    char * stringAuxiliar = malloc(sizeof(char));   //Obtenemos el espacio de memoria necesario para
                                                    //guardar los caracteres que se van escribiendo
    
    fflush(stdin);//Limpia el STANDARD INPUT
    char caracter = getchar();//Toma un solo caracter del teclado
    
    //Mientras el caracter no sea el 'enter' y ni tampoco se exceda la maxima cantidad de caracteres
    //permitidos en un mensaje, se toman caracteres y se va formando la cadena a enviar
    while(caracter != 10 && index < LONGITUD_MAX_DE_DATOS ){
        stringAuxiliar[index] = caracter;//Guardamos el caracter "tipeado"

        index++;//Nos desplazamos en la cadena
        stringAuxiliar = realloc(stringAuxiliar, sizeof(char) * (index+1) );//Obtenemos mas espacio para
                                                                            //guardar mas caracteres

        fflush(stdin);//Limpiamos el STANDARD INPUT
        caracter = getchar();//Toma un solo caracter del teclado
    }

    stringAuxiliar[index] = '\0';//Cerramos el string de manera correcta

    *string = stringAuxiliar;//Guardamos el string generado en esta función adentro del puntero pasado por parametro
}

void obtenerParametrosDeSolicitud(char *** puntero,char * stringDeSolicitud){
    /*
        Le pasamos la direccion de un arreglo de strings y el string que representa la solicitud
        del usuario. Termina configurando al arreglo de strings dejandole todos los parametros de manera
        comoda.
    */

    char * parametro = strtok(stringDeSolicitud," ");//Tomamos el primer parametro de la solicitud (en caso idea, deberia ser ParcialTL2020)
    int cantidadDeParametros = 0;//Contador de parametros
    char ** parametros;//Espacio en el que vamos guardando los parametros

    parametros = (char **) malloc(sizeof(char *));//Alojamos lo suficiente para poder empezar a grabar strings

    /*Seguiremos tomando parametros hasta que no haya mas parametros que tomar (cuando parametro vale NULL)*/
    while( parametro != NULL ) {
        parametros[cantidadDeParametros] = parametro;//Guardamos la cadena obtenida dentro del arreglo de strings
        cantidadDeParametros++;//Aumentamos el contador de parametros
        parametros = (char **) realloc(parametros, (sizeof(char *)) * (cantidadDeParametros+1) );   //Obtenemos mas espacio de memoria para poner
                                                                                                    //mayor cantidad de parametros
        parametro = strtok(NULL, " ");//Obtenemos otro parametro de stringDeSolicitud
    }

    parametros[cantidadDeParametros] = malloc(sizeof(char));//Alojamos un espacio mas para guardar simplemente '\0'. Indica el fin de los parametros
    parametros[cantidadDeParametros][0] = '\0';//Guardamos '\0' en el ultimo "parametro" para indicar que ahi termina el arreglo
    *puntero = parametros;//Guardamos la direccion de los parametro dentro del puntero pasado como entrada.
}

void registrarMensaje(char * nombreDeUsuario, char * mensaje){
    /*
        Guarda en nombre de un usuario un mensaje determinado dentro de los directorios del servidor.
    */
    FILE * archivo;//Declaramos una variable puntero del tipo FILE
    char nombreDeArchivo[LONGITUD_MAX_DE_NOMBRE_DE_USUARIO+5];//En esta variabel guardaremos el nombre de Archivo a usar

    strcpy(nombreDeArchivo,nombreDeUsuario);//Guardamos el nombre de usuario dentro de nombre de archivo
    strcat(nombreDeArchivo,".txt\0");//Y de paso le concatenamos ".txt\0". Lo guardaremos en este formato

    printf("Se registra '%s' en el archivo %s...\n", mensaje, nombreDeArchivo);//Reportamos en el servidor el mensaje guardado

    archivo = fopen(nombreDeUsuario, "a+"); //Abrimos el archivo de manera que el mensaje a ingresar se guarde a lo ultimo del mismo. 
                                            //En caso de no estar creado, se creará
    fprintf(archivo, "%s\n", mensaje);//Guardamos el mensaje con el formato correspondiente
    fclose(archivo);//Cerramo el archivo
}

void recibirNombreDeUsuarioPorTeclado(char ** punteroString){
    
    /*
        Guarda un nombre de usuario escrito por el cliente adentro de un puntero de un string
    */

    char * nombreDeUsuario = NULL;//En esta variable guardaremos el nombre del usuario
    printf("Ingrese nombre de usuario (%d Caracteres): ",LONGITUD_MAX_DE_NOMBRE_DE_USUARIO);//Mostramos un mensaje para que
                                                                                            //el cliente escriba
    
    recibirMensajesDesdeElTeclado(&nombreDeUsuario);//Obtenemos el nombre y lo guardamos en nombreDeUsuario

    //Mientras el nombre de usuario sea incorrecto, ingresará el nombre de usuario hasta que ingrese uno válido
    while(!esNombreDeUsuarioValido(nombreDeUsuario)){
        free(nombreDeUsuario);//Liberamos el espacio de memoria asignado anteriormente
        printf("ERROR. Ingrese nombre de usuario: ");//Mostramos mensaje de error
        recibirMensajesDesdeElTeclado(&nombreDeUsuario);//Obtenemos el nombre y lo guardamos en nombreDeUsuario
    }

    *punteroString = nombreDeUsuario;//Guardamos el nombre de usuario adentro del puntero pasado por parametros
}

void recibirTipoDeSolicitudPorTeclado(char ** punteroString){
    /*
        Guarda un tipo de solicitud escrito por el cliente adentro de un puntero de un string
    */
    char * tipoDeSolicitud = NULL;//En esta variable guardaremos el tipo de solicitud
    printf("Ingrese tipo de solicitud:");  //Mostramos un mensaje para que
                                            //el cliente escriba
    
    
    recibirMensajesDesdeElTeclado(&tipoDeSolicitud);//Obtenemos el tipo de solicitud y lo guardamos en tipoDeSolicitud

    /*Mientras el tipo de suaurio sea incorrecto, ingresará el tipo de usuario hasta que ingrese uno válido*/
    while(!esTipoDeSolicitudValido(tipoDeSolicitud)){
        free(tipoDeSolicitud);//Liberamos el espacio de memoria asignado anteriormente
        printf("ERROR. Ingrese tipo de solicitud: ");//Mostramos mensaje de error
        recibirMensajesDesdeElTeclado(&tipoDeSolicitud);//Obtenemos el tipo de solicitud y lo guardamos en tipoDeSolicitud
    }

    *punteroString = tipoDeSolicitud;//Guardamos el tipo de solicitud adentro del puntero pasado por parametros

}

void convertirSolicitudEnString(char * string ,solicitud solicitudAConvertir){
    /*
        Se pasa por parametros un string ya con "ParcialTL2020" y luego una solicitud y se procede a
        y concatenando los parametros de la solicitud en el string.
    */
    strcat(string, " ");//Concatenamos a string un espacio
    strcat(string, solicitudAConvertir.tipoDeSolicitud);//Concatenamos a string el tipo de solicitud

    strcat(string, " ");//Concatenamos a string un espacio
    strcat(string, solicitudAConvertir.nombreDeUsuario);//Concatenamos a string el tipo de solicitud

    strcat(string, " ");//Concatenamos a string un espacio
    strcat(string, solicitudAConvertir.extremo);//Concatenamos a string el extremo

    strcat(string, " ");//Concatenamos a string un espacio
    strcat(string, solicitudAConvertir.TIVms);//Concatenamos a string el TIVms
}

void enviarSolicitudAServidor(int descriptorDelSocket, solicitud solicitudAEnviar){
    /*
        Enviamos una solicitud a traves de un socket
    */
    int enteroRetornado = 0;//Se usará para reportar errores
    char solicitudHechaString[LONGITUD_MAX_DE_SOLICITUD] = "ParcialTL2020";//Agregamos el encabezado de la solicitud
    
    convertirSolicitudEnString(solicitudHechaString,solicitudAEnviar);//Convertimos la solicitud en string

    printf("%s\n", solicitudHechaString);//Imprimos la solicitud
    enteroRetornado = send(descriptorDelSocket, solicitudHechaString, LONGITUD_MAX_DE_DATOS * sizeof(char), 0); //Utilizamos send para poder mandar un mensaje sobre un socket.
                                                                                                                //Como mucho se manda 100Bytes de datos
    reportarSiEsNecesario(enteroRetornado, "send");//Reportamos error si es necesario
}

void msj_cliente(int socket){

    /*
        Opera la solicitud "msj" del lado cliente. Solo le pasamos el socket para comunicarnos con el servidor.
    */

    char mensajeAEnviar[LONGITUD_MAX_DE_DATOS];
    char * mensajeAEnviarAuxiliar;

    printf("Empiece a mandar mensajes...\n");//Le informamos al cliente que empiece a mandar mensajes
    recibirMensajesDesdeElTeclado(&mensajeAEnviarAuxiliar);//Recibimos un mensaje por teclado
    stpcpy(mensajeAEnviar,mensajeAEnviarAuxiliar); //Copiamos lo escrito a mensajeAEnviar
    int enteroRetornado = send(socket, mensajeAEnviar, LONGITUD_MAX_DE_DATOS * sizeof(char), 0);//Enviamos el primer mensaje al servidor
    reportarSiEsNecesario(enteroRetornado, "send");//Reportamos error en el caso de ser necesario

    //Mientras el usuario no haya escrito -1 (EOF), se siguen mandando mensajes.
    while(strcmp("-1",mensajeAEnviar) != 0){    
        recibirMensajesDesdeElTeclado(&mensajeAEnviarAuxiliar);//Recibimos un mensaje por teclado
        stpcpy(mensajeAEnviar,mensajeAEnviarAuxiliar); //Copiamos lo escrito a mensajeAEnviar
        enteroRetornado = send(socket, mensajeAEnviar, LONGITUD_MAX_DE_DATOS * sizeof(char), 0);//Enviamos el primer mensaje al servidor
        reportarSiEsNecesario(enteroRetornado, "send");//Reportamos error en el caso de ser necesario
    }

    free(mensajeAEnviarAuxiliar);//Liberamos el espacio
}

void log_servidor(int socket, solicitud solicitudDelCliente){
    
    /*
        Opera la solicitud "log" del lado servidor. Solo le pasamos el socket para comunicarnos con el cliente
        y la solicitud del mismo.
    */


    FILE * archivo = fopen(solicitudDelCliente.nombreDeUsuario, "r");//Abrimos el archivo para leerlo
    char mensajeAEnviar[LONGITUD_MAX_DE_DATOS];//Guardaremos las lineas que le vamos a ir mandando al cliente
    int enteroRetornado;

    /*
        Si no se encuentra el archivo, le enviaremos un fin con el simplemente hecho de informarle que no hay archivos
        bajo su nombre.
    */
    if(archivo == NULL){
        printf("El usuario nunca estuvo en contacto con este servidor...'\n");//Imprimimos por pantala el no haber encontrado el archivo
        strcpy(mensajeAEnviar,"fin");//ponemos "fin" en el mensaje a enviar
        enteroRetornado = send(socket, mensajeAEnviar, LONGITUD_MAX_DE_DATOS * sizeof(char), 0);//Mandamos el mensaje al cliente
        reportarSiEsNecesario(enteroRetornado, "send");//Reportamos el error de ser necesario
    }

    /*
        Si hay archivo, le mandamos todo el historial
    */
    else{
        printf("Se manda el historial...\n");//Imprimimos por pantalla que se esta mandando el "historial".
        char caracterLeido;//Guardaremos el caracter que vamos leyendo del archivo
        int letrasLeidas = 0;//Contaremos las letras leidas
        caracterLeido = fgetc(archivo);//Tomamos el primer caracter del archivo

        /*Mientras el archivo no haya terminado (no nos encontramos con un EOF), se seguiran leyendo caracteres*/
        while(caracterLeido != EOF){

            letrasLeidas = 0;//Ponemos el contador de letras en 0

            /*
                Si detectamos un '\n' lo consideramos como el fin de un mensaje. Al terminar el ciclo, mandamos el mensaje.
            */
            while(caracterLeido != '\n'){
                mensajeAEnviar[letrasLeidas] = caracterLeido;//Tomamos el caracter y lo vamos guardando para enviarlo despues
                letrasLeidas++;//Nos vamos desplazando en el mensajeAEnviar
                caracterLeido = fgetc(archivo);//Vamos tomando caracteres del archivo
            }

            mensajeAEnviar[letrasLeidas] = '\0';//Cerramos el string
            send(socket, mensajeAEnviar, LONGITUD_MAX_DE_DATOS * sizeof(char), 0);//Enviamos el mensajeAEnviar al cliente y asi lo lee
            caracterLeido = fgetc(archivo);//Agarramos otro caracter del archivo
        }

        strcpy(mensajeAEnviar,"fin");//Guardamos en mensajeAEnviar la palabra "fin" para indicar que ya no hay mas historial
        send(socket, mensajeAEnviar, LONGITUD_MAX_DE_DATOS * sizeof(char), 0);//Mandamos el string de parada
        fclose(archivo);//Cerramos e archivo
    }
}

void log_cliente(int descriptorDeSocket){
    /*
        Opera la solicitud "log" del lado cliente. Solo le pasamos el socket para comunicarnos con el servidor
    */
    char mensajeRecibido[LONGITUD_MAX_DE_DATOS];//Guardamos los mensajes que vamos recibiendo del servidor
    int enteroRetornado;//Lo usaremos de control
    int llegaronMensajes = FALSE;//Lo usaremos para saber si llegaron o no mensajes
    printf("Leemos nuestros datos en el servidor...\n");//Imprimimos en pantalla que vamos recibiendo mensajes
    enteroRetornado = recv(descriptorDeSocket,mensajeRecibido,LONGITUD_MAX_DE_DATOS * sizeof(char), 0);//Recibimos mensajes de parte del servidor
    reportarSiEsNecesario(enteroRetornado, "recv");//Reportamos error en caso de ser necesario

    /*Mientras no se haya recibido el mensaje "fin", seguiremos tomando mensajes*/
    while( strcmp(mensajeRecibido,"fin") != 0 ){
        llegaronMensajes = TRUE;
        printf("%s\n", mensajeRecibido);//Imprimimos por pantalla el mensaje
        enteroRetornado = recv(descriptorDeSocket,mensajeRecibido,LONGITUD_MAX_DE_DATOS * sizeof(char), 0);//Recibimos mensajes de parte del servidor
        reportarSiEsNecesario(enteroRetornado, "recv");//Reportamos error en caso de ser necesario
    }

    //En el caso de que haya llegado mensajes, reportamos el FIN. Sino, avisamos al cliente no se recibió ningun mensaje
    if(llegaronMensajes)
        printf("%s\n", mensajeRecibido);//Imprimimos por pantalla el ultimo mensaje recibido (FIN)
    else
        printf("Es muy posible que nunca hayas iniciado sesion servidor o no tenes registro de mensajes...\n");

}

int analizarParametros(char ** parametros, int descriptorDelSocketCliente){

    /*
        Lo usaremos para analizar los parametros mandados por el cliente. Simplemente pasamos la lista de parametros
        y el socket para comunicarnos con el cliente. Devuelve -1 en el caso de que haya habiado una falla al pedir los parametros otra vez.
    */
    solicitud solicitudAuxiliar; //Lo usaremos para armar el saludo
    int enteroRetornado;//Lo usamos de control
    char solicitudEnString[LONGITUD_MAX_DE_SOLICITUD];//Guardamos el string de la solicitud
    char mensajeAEnviar[LONGITUD_MAX_DE_DATOS];//Guardamos el mensaje a enviar
    char mensajeRecibido[LONGITUD_MAX_DE_DATOS];//Guardamos el mensaje que recibimos

    /*Mientras los parametros no sean correctos, se le va apedir constantemente al  cliente que los mande bien...*/
    while(!sonCorrectosLosParametros(parametros)){
        strcpy(mensajeAEnviar,"ERROR\n");//Ponemos "ERROR" en el mensaje a enviar
        printf("Cliente con datos incorrectos. Se informa error al que lo envió...\n");//Imprimimos en el servidor un mensaje para avisar que un cliente mandó mal los parametros

        //Mandamos el mensaje de ERROR y por las dudas se retiene el valor para informar y cerrar la conexión con el cliente en caso de que haya habido un error en reenvio de parametros
        enteroRetornado = send(descriptorDelSocketCliente, mensajeAEnviar, LONGITUD_MAX_DE_DATOS * sizeof(char), 0); //Utilizamos send para poder mandar un mensaje sobre un socket
        
        //Si se devolvió error, se reporta error con -1
        if(enteroRetornado == -1)
            return -1;//Devolvemos -1
        
        bzero( solicitudEnString, LONGITUD_MAX_DE_SOLICITUD * sizeof(char) );//Ponemos en 0 todo la solicitudEnString
        enteroRetornado = recv(descriptorDelSocketCliente, solicitudEnString, LONGITUD_MAX_DE_SOLICITUD * sizeof(char), 0);//Recibimos lo que el cliente nos mando
        
        //Si se devolvió error, se reporta error con -1
        if(enteroRetornado == -1)
            return -1;//Devolvemos -1
        
        obtenerParametrosDeSolicitud(&parametros,solicitudEnString);//Volvemos a obtener los parametros          
    }
    
    armarEstructuraDeSolicitud(&solicitudAuxiliar,parametros);//Armamos la estructura
    bzero(mensajeAEnviar, LONGITUD_MAX_DE_DATOS * sizeof(char));
    armarSaludo(mensajeAEnviar, solicitudAuxiliar);//Armamos el saludo que le vamos a mandar al cliente

    //Utilizamos send para poder mandar el saludo a traves del socket
    enteroRetornado = send(descriptorDelSocketCliente, mensajeAEnviar, LONGITUD_MAX_DE_DATOS * sizeof(char), 0);
    reportarSiEsNecesario(enteroRetornado, "send");//Reportamos error de ser necesario

    //Si se devolvió error, se reporta error con -1
    if(enteroRetornado == -1)
        return -1;//Devolvemos -1

    return 0;//Devolvemos 0 ya que no hubo errores

}

void imprimirMensajeDeBienvenidaCliente(){
    /*
        Imprime un mensaje de bienvenida al Cliente
    */
    printf("-----------------------------\n");
    printf("----Bienvenido al cliente----\n");
    printf("-----------------------------\n\n");

    printf("-------------------------------------------------------------------------\n");
    printf("Usted ahora mismo se va a conectar al servidor.\n");
    printf("Tiene 3 tipos solicitudes posibles: \n");
    printf("msj: Inicia registro de mensajes con el servidor\n");   
    printf("log: Se le pide al servidor el registro total de mensajes hasta el momento\n");
    printf("fin: Terminar conexión\n");   
    printf("--------------------------------------------------------------------------\n");
}

void imprimirMensajeDeBienvenidaServidor(){
    /*
        Imprime un mensaje de bienvenida al Servidor
    */
    printf("----------------------------\n");
    printf("---Bienvenido al servidor---\n");
    printf("----------------------------\n");
}

void verificarError(char * mensajeRecibido, solicitud * puntero, int descriptorDeSocket){
    /*
        Si el cliente recibe que hubo un error, volverá a mandar los parametros
    */

    //Mientras haya error, le pedimos el re-ingreso de parametros
    while( strcmp(mensajeRecibido,"ERROR") == 0 ){
        printf("Hubo un error en el envio. Intente de nuevo...\n");//Reportamos al cliente que hubo error en el envio
        recibirTipoDeSolicitudPorTeclado(&((*puntero).nombreDeUsuario));//Recibimos por teclado el nombre de usuario
        recibirTipoDeSolicitudPorTeclado(&((*puntero).tipoDeSolicitud));//Recibimos por teclado el tipo de solicitud
        enviarSolicitudAServidor(descriptorDeSocket, *puntero);//Enviamos la solicitud otra vez al servidor
        reportarSiEsNecesario(recv(descriptorDeSocket,mensajeRecibido,LONGITUD_MAX_DE_DATOS * sizeof(char), 0),"recv");//Mandamos el mensaje y en caso de ser necario,
                                                                                                                        //reportamos el error.
    }

}

void recibirSolicitud(int descriptorDelSocketCliente, char * solicitudEnString, char *** puntero){

    /*
        Se utiliza para recibir una solicitud en el lado servidor
        y ya obtener parametros sin problemas
    */
    
    int enteroRetornado;//Se usa de control
    printf("Esperando Solicitud...\n");//Se imprime que estamos esperando solicitud
    enteroRetornado = recv(descriptorDelSocketCliente, solicitudEnString, LONGITUD_MAX_DE_SOLICITUD * sizeof(char), 0);//Esperamos a que nos llegue una solicitud del cliente
    reportarSiEsNecesario(enteroRetornado, "recv");//reportamos error de ser necesario
    printf("%s\n", solicitudEnString);//Imprimimos la solicitud
    obtenerParametrosDeSolicitud(puntero,solicitudEnString);//Obtenemos los parametros
}

void armarEstructuraDeSolicitud(solicitud * solicitudDelCliente, char ** parametros){
        //Armamos la estructura ya con parametros "corregidos"
        solicitudDelCliente->tipoDeSolicitud = parametros[1];
        solicitudDelCliente->nombreDeUsuario = parametros[2];
        solicitudDelCliente->extremo = parametros[3];
        solicitudDelCliente->TIVms = parametros[4];
}

void armarSaludo(char * mensajeAEnviar, solicitud solicitudDelCliente){
    //Armamos el saludo que le vamos a enviar al cliente
    strcpy(mensajeAEnviar,"Hola ");
    strcat(mensajeAEnviar,solicitudDelCliente.nombreDeUsuario);
    strcat(mensajeAEnviar," de ");
    strcat(mensajeAEnviar,solicitudDelCliente.extremo);
}

void msj_servidor(int descriptorDelSocketCliente, char * nombreDeUsuario){
    /*
        Opera la solicitud "msj" del lado servidor. Solo le pasamos el socket para comunicarnos con el cliente y el nombre de usuario para
        registrar el log.
    */
    char mensajeARecibir[LONGITUD_MAX_DE_DATOS];//Variable donde guardaremos el mensaje que recibimos
    printf("Tomando Mensajes...\n");//Reportamos que estamos tomando los mensajes en el servidor
    int enteroRetornado = recv(descriptorDelSocketCliente, mensajeARecibir, LONGITUD_MAX_DE_DATOS * sizeof(char), 0);//Tomamos el mensaje recibido
    mensajeARecibir[enteroRetornado] = '\0';//Cerramos el string de manera correcta
    reportarSiEsNecesario(enteroRetornado, "recv");//Reportamos error en caso de ser necesario

    /*Mientras el usuario no envió el EOF (-1), seguira registrando los mensajes en el log*/
    while(strcmp("-1",mensajeARecibir) != 0){            
        registrarMensaje(nombreDeUsuario, mensajeARecibir);//Registramos el mensaje en el .txt
        enteroRetornado = recv(descriptorDelSocketCliente, mensajeARecibir, LONGITUD_MAX_DE_DATOS * sizeof(char), 0);//Recibimos otro mensaje del cliente
        reportarSiEsNecesario(enteroRetornado, "recv");//Reportamos error en caso de ser necesario
    }
}

void armarMensajeDeDesconexion(char * mensajeAMostrar, solicitud solicitudDelCliente){
    //Armamos el mensaje de desconexión que se va a mostrar en el servidor
    strcpy(mensajeAMostrar,"Conexion con ");
    strcat(mensajeAMostrar,solicitudDelCliente.nombreDeUsuario);
    strcat(mensajeAMostrar," del ");
    strcat(mensajeAMostrar,solicitudDelCliente.extremo);
    strcat(mensajeAMostrar," finalizada\n");
}
