#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <conio.h>

#include "libxml.h"

#define MAX_ADAPTADORES 10
#define MAX_SALTOS 30
#define MAX_LONGITUD_IP 20




void pasos()
{
    int op;
    op = menu();
    while (op >= 0 && op <= 3) {
        if (op == 1) {
            solicitarAdaptador();
        }
        else if (op == 0) {
            break;
        }
        op = menu();
    }
}


int menu()
{
    int op;
    printf("\n                                   ---MENU----                                         \n");
    printf("wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww\n\n");
    printf("    1. Show system network adapters and choose which one we want data from.\n");
    printf("    0. Exit menu\n\n");
    printf("wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww\n\n");
    printf("\n\n");
    printf("Select an option:  ");
    scanf("%d", &op);
    return op;
}

//Preguntamos inicialmente el nombre del adaptador de red a analizar y guardamos los datos:
void solicitarAdaptador()
{
    // Declaramos variables
    char str[255] = "";
    char line[255] = "";
    char adaptador[255] = "";
    char ip[255] = "";
    char mascara[255] = "";
    char pEnlace[255] = "";
    char dns[255] = "";
    char media[255];
    int numSaltos = 0;
    char* rutaAdaptadores = "./adaptadores.tmp";
    char* rutaDnsSeleccionado = "./dnsSeleccionado.tmp";

    FILE* f = _popen("netsh interface ipv4 show interfaces", "r");

    if (f == NULL) {
        printf("Error.");
        exit(1);
    }// Mostrar los adaptadores disponibles.
    printf("The following network adapters are available:\n\n");
    while (fgets(line, sizeof line, f) != NULL) {
        printf("%s", line);
    }
    // Solicitar adaptador a analizar.
    printf("Type the name of the adapter you want to get data from: ");
    getchar();
    fgets(adaptador, sizeof(adaptador), stdin);
    adaptador[strcspn(adaptador, "\n")] = '\0'; // Eliminar el salto de linea al final del adaptador
    _pclose(f);
    printf("\n+++++++++++++++++++++++++++++ SEARCHING ++++++++++++++++++++++++++++++");
    printf("\n\n");

    // Creamos los archivos temporales necesarios
    temporales(adaptador);

    //  Funcion para obtener los datos requeridos del DNS;
    obtenerDatos(rutaAdaptadores, ip, mascara, pEnlace, dns);


    //Función para hacer el ping();
    ping(dns, media);



    // Imprimimos los valores de los datos requeridos.

    printf("    Network name: %s\n", &adaptador);
    printf("    IP Address: %s\n", &ip);
    printf("    Subnet mask: %s\n", &mascara);
    printf("    Default gateway: %s\n", &pEnlace);
    printf("    Average response speed of the selected server: %s\n", media);

    printf("\n-------------------  Press ENTER to continue. ----------------------\n");
    getchar();
    printf("+++++++++++++++++++++++++++++ SEARCHING ++++++++++++++++++++++++++++++\n\n");

    // Generamos los saltos:
    numSaltos = saltos(dns);

    // Creamos el archivo XML
    archivoXML(ip, mascara, pEnlace, dns, media, numSaltos);
    eliminar_temporales();
    limpiar_terminal();
}



// Se crean los archivos temporales para despues obtener los datos que necesitamos:
void temporales(char* adaptador)
{
    char comando_dns[200];
    char comando_ipconfig[200];

    sprintf(comando_dns, "netsh interface ipv4 show dnsservers \"%s\" > dnsSeleccionado.tmp", adaptador);
    sprintf(comando_ipconfig, "netsh interface ipv4 show config \"%s\" > adaptadores.tmp", adaptador);

    system(comando_dns);
    system(comando_ipconfig);
}

// Función para obtener los datos que necesitamos:
void obtenerDatos(char* rutaAdaptadores, char* ip, char* mascara, char* pEnlace, char* dns)
{
    // Abrir archivo temporal
    FILE* archivo = fopen("adaptadores.tmp", "r");
    while (!feof(archivo)) {
        char linea[256] = "";
        fgets(linea, 256, archivo);

        // Buscamos la IP:
        if (strstr(linea, "IP") != NULL)
        {
            // Extraer la direccion IP del servidor DNS:
            char* dnsStart = strstr(linea, ":") + 28;
            char* dnsEnd = strstr(linea, "\n");
            if (dnsEnd != NULL) {
                *dnsEnd = '\0';
            }
            // Actualizar la variable dns con la direccion IP:
            strcpy(ip, dnsStart);

            // Eliminar espacios en blanco al inicio. 
            char* ipTrimmed = ip;
            while (*ipTrimmed == ' ') {
                ipTrimmed++;
            }
        }// Buscamos Puerta de enlace predeterminada:
        else if (strstr(linea, "Default gateway") != NULL)
        {
            // Extraer la direccion desde el caracter ":":
            char* dnsStart = strstr(linea, ":") + 20;
            char* dnsEnd = strstr(linea, "\n");
            if (dnsEnd != NULL) {
                *dnsEnd = '\0';
            }
            // Actualizar la variable pEnlace:
            strcpy(pEnlace, dnsStart);

            // Eliminar espacios en blanco al inicio.
            char* peTrimmed = pEnlace;
            while (*peTrimmed == ' ') {
                peTrimmed++;
            }
        }// Buscamos la mascara (en mi archivo no imprime máscara por el acento, dejando un salto)
        // por eso busco la palabra "scara".
        else if (strstr(linea, "subnetMask") != NULL)
        {
            // Extraer la direccion a partir de la "a" + 4 espacios:
            char* dnsStart = strstr(linea, "a") + 4;
            char* dnsEnd = strstr(linea, ")\n");
            if (dnsEnd != NULL) {
                *dnsEnd = '\0';
            }
            // Actualizar la variable mascara:
            strcpy(mascara, dnsStart);

            // Eliminar espacios en blanco al inicio 
            char* mskTrimmed = mascara;
            while (*mskTrimmed == ' ') {
                mskTrimmed++;
            }
        }// Buscamos el DNS:
        else if (strstr(linea, "DNS Servers") != NULL)
        {
            // Extraer el DNS:
            char* dnsStart = strstr(linea, ":") + 3;
            char* dnsEnd = strstr(linea, "\n");
            if (dnsEnd != NULL) {
                *dnsEnd = '\0';
            }
            // Actualizar la variable dns:
            strcpy(dns, dnsStart);

            // Eliminar espacios en blanco al inicio 
            char* dnsTrimmed = dns;
            while (*dnsTrimmed == ' ') {
                dnsTrimmed++;
            }
        }
    }
    fclose(archivo);
}

//Funcion para realizar el ping al DNS seleccionado:
void ping(char* dns, char* media)
{
    //Ejecutar el comando ping y guardarlo en archivo temporal
    char comando_ping[200];
    sprintf(comando_ping, "ping  \"%s\" >> pingSeleccionado.tmp", dns);
    system(comando_ping);

    // Abrir archivo
    FILE* arcPing = fopen("pingSeleccionado.tmp", "r");
    while (!feof(arcPing)) {
        char linea[256] = "";
        fgets(linea, 256, arcPing);
        // Leer lineas hasta encontrar el string "Media":
        if (strstr(linea, "Media") != NULL)
        {
            // Extraer Media contando los caracteres:
            char* dnsStart = strstr(linea, ", Media = ") + 10;
            char* dnsEnd = strstr(linea, "\n");
            if (dnsEnd != NULL) {
                *dnsEnd = '\0';
            }
            // Actualizar la variable media:
            strcpy(media, dnsStart);

            // Eliminar espacios en blanco al inicio 
            char* mdTrimmed = media;
            while (*mdTrimmed == ' ') {
                mdTrimmed++;
            }
        }
    }
}

//Funcion para obtener los saltos usando el comando "TRACERT":
int saltos(char* dns)
{
    //Declarar variables:
    int numSaltos = 0;
    int numLineas = 0;
    char saltos[MAX_SALTOS][MAX_LONGITUD_IP];
    char linea[256];


    //Ejecutar el comando TRACERT y guardarlo en archivo temporal:
    char comando[255];
    sprintf(comando, "tracert -d %s > tracert.tmp", dns);
    system(comando);

    // Introducimos la funcion para poder leer medor los datos.
    modificarTracert();

    //Abrir archivo tracert.tmp
    FILE* salto = fopen("tracert.tmp", "r");
    if (salto == NULL) {
        printf("Error opening tracert file\n");
        exit(1);
    }
    // Leer el archivo línea por línea buscando el string "ms" o "*":
    while (fgets(linea, sizeof(linea), salto) != NULL) {
        if (strstr(linea, "ms") != NULL || strstr(linea, "*") != NULL) {
            numSaltos++;
        }

        // Si la linea contiene el caracter "." busca el primer espacio seleccionando el "7" string.
        if (strstr(linea, ".") != NULL) {
            char* token = strtok(linea, " ");
            int count = 0;
            while (token != NULL) {
                if (count == 7) {
                    strcpy(saltos[numLineas], token);
                    numLineas++;
                    break;
                }
                token = strtok(NULL, " ");
                count++;
            }
            char* lastToken = token;
            while (lastToken != NULL) {
                if (strlen(lastToken) > 0 && lastToken[strlen(lastToken) - 1] == '*') {
                    lastToken[strlen(lastToken) - 1] = '\0';
                    numSaltos++;
                }
                lastToken = strtok(NULL, " ");
            }
        }
    }
    fclose(salto);
    return numSaltos;
}


//Generamos el Archivo XML.
void archivoXML(char* ip, char* mascara, char* pEnlace, char* dns, char* media, int numSaltos)
{
    // Generar el archivo XML
    FILE* archivo = fopen("datos.xml", "w");
    char linea[255] = "";
    if (archivo == NULL) {
        printf("Error opening the file.");
    }
    // Limpiar lineas vacias.
    lineas_vacias();
    // Abrir archivo .XML
    FILE* xml = fopen("datos.xml", "w");
    if (xml == NULL) {
        printf("Error opening the file.");
    }
    //Imprimir los datos con su formato .XML
    fprintf(xml, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(xml, "<RedAdapter>\n");
    fprintf(xml, "		<interface>\n");
    fprintf(xml, "			<IP>%s</IP>\n", ip);
    fprintf(xml, "			<subnet_mask>%s</subnet_mask>\n", mascara);
    fprintf(xml, "			<Gateway>%s</Gateway>\n", pEnlace);
    fprintf(xml, "		<interface>\n");
    fprintf(xml, "		<DNS>\n");
    fprintf(xml, "			<Primary_DNS_Server>%s</Primary_DNS_Server>\n", dns);
    fprintf(xml, "			<SpeedResponseDNS>%s</SpeedResponseDNS>\n", media);
    fprintf(xml, "			<NumberJump>%d</NumberJump>\n", numSaltos);
    fprintf(xml, "		</DNS>\n");
    fprintf(xml, "</RedAdapter>\n");

    // Abrir archivo temporal
    FILE* jumps = fopen("saltos.tmp", "r");
    if (xml == NULL) {
        printf("Error opening the file.");
    }// Bucle para imprimir los saltos y IP
    while (!feof(jumps))
    {
        fgets(linea, 255, jumps);
        if (linea[0] != '\n') { // Verificar si la línea no está vacía
            char* dnsStart = linea;
            char* dnsEnd = strstr(linea, "\n");
            if (dnsEnd != NULL) {
                *dnsEnd = '\0';
            }//Imprimir los datos con su formato .XML
            fprintf(xml, "\t\t\t<ip>%s</ip>\n", linea);
        }
    }
    //Imprimir los datos con su formato .XML
    fprintf(xml, "\t\t</IpSaltos>\n");
    fprintf(xml, "\t</dns>\n");
    fprintf(xml, "</AdaptadorRed>\n");
    // Cerrar archivo y aviso que esta generado con exito.
    fclose(xml);
    printf("XML file generated successfully.\n");
}

// Funcion para modificar el tracert y recuparar los saltos y las IPs.
void modificarTracert() {

    FILE* archivo = fopen("tracert.tmp", "r");
    if (archivo == NULL) {
        printf("Error opening tracert file\n");
        exit(1);
    }
    // Saltar la primera línea
    char linea[256];
    fgets(linea, sizeof(linea), archivo);

    // Abrir archivo para escritura
    FILE* archivoModificado = fopen("salto.tmp", "w");
    if (archivoModificado == NULL) {
        printf("Error opening the modified file\n");
        exit(1);
    }
    int salto = 0;
    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        // Buscar líneas que contengan el formato de salto y dirección IP
        if ((strstr(linea, " ms") != NULL) || (strstr(linea, "*") != NULL)) {
            salto++;
            char ip[20];
            char* token = strtok(linea, " ");
            while (token != NULL) {
                if (strstr(token, ".") != NULL) {
                    strcpy(ip, token);
                    break;
                }
                token = strtok(NULL, " ");
            }
            fprintf(archivoModificado, "Salto %d: %s\n", salto, ip);
        }
    }
    fclose(archivo);
    fclose(archivoModificado);
    rename("salto.tmp", "saltos.tmp");
    printf("File modified successfully. Found %d skips.\n", salto);
}

// Funcion para reescribir el archivo saltos.tmp y dejarlo limpio de lineas vacias.
void lineas_vacias() {
    FILE* limpArch = fopen("saltos.tmp", "r+");
    if (limpArch == NULL) {
        printf("Error al abrir el archivo.\n");
        return;
    }
    // Variables para leer y escribir en el archivo
    char linea[255];
    char contenido[255] = "";

    // Leer y escribir el archivo línea por línea
    while (fgets(linea, sizeof(linea), limpArch) != NULL) {
        if (strcmp(linea, "\n") != 0) {
            strcat(contenido, linea);
        }
    }
    // Volver al inicio del archivo y sobrescribir su contenido
    rewind(limpArch);
    fprintf(limpArch, "%s", contenido);

    // Cerrar el archivo
    fclose(limpArch);

    printf("File successfully cleaned.\n");
}

// Funcion Limpiar terminal.
void limpiar_terminal()
{
    printf("\n");
    system("pause");
    system("cls || clear");
}
// Funcion Eliminar temporales.
void eliminar_temporales()
{
    remove("adaptadores.tmp");
    remove("dnsSeleccionado.tmp");
    remove("pingSeleccionado.tmp");
    remove("saltos.tmp");
    remove("tracert.tmp");
}
