/*
376901 - Jackson Armando Velazquez Vargas
377054 - Carlos Uriel Reyes Juárez

Abrir el directorio /proc/
en un bucle:
Mientras la entrada sea una carpeta y el nombre de la carpeta sea un numero:
    /proc/<pid>/stat obtener solo datos necesarios con fscanf, los primeros 4
    /proc/pid/cmdline obtener tipo leyendo datos, (leer algun dato -> datos = fin del archivo = el proceso es kernel)
    imprimir
*/
#define _DEFAULT_SOURCE //para los defines con los tipos de entrada de directorio, nomas se ocupa para identificar que sea un directorio/carpeta
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>

#define PROCESO_RUTA_PRINCIPAL "/proc"
#define PROCESO_MAX_NOMBRE 255 //Se supone que es 28 pero no encontre el dato exacto del tamanio maximo del nombre de un proceso

#define PROCESO_TIPO_DESCONOCIDO -1
#define PROCESO_TIPO_KERNEL 0
#define PROCESO_TIPO_USUARIO 1

typedef struct{
    int id;
    char nombre[PROCESO_MAX_NOMBRE];
    char estado;
    long int prioridad;
    int tipo;
}Proceso;

int EsNumero(const char *);
Proceso ObtenerDatosDelProceso(const char *);
void ImprimirDatosDelProceso(Proceso);

int main(void){
    DIR *carpeta_procesos = opendir(PROCESO_RUTA_PRINCIPAL);
     
    if(!carpeta_procesos){
        printf("ERROR: No se pudo abrir el directorio\n");
        return 1;
    }

    struct dirent *entrada;
    Proceso proceso_temp;
    char ruta_temp[255];
    unsigned int num_procesos = 0;

    printf("Lista de Procesos\n\n");
    printf("ID        Nombre");
    for(int i = 1; i <= PROCESO_MAX_NOMBRE; i++) printf(" ");
    printf("Estado  Prioridad  Tipo\n");

    while((entrada = readdir(carpeta_procesos)) != NULL){
        if(entrada->d_type != DT_DIR || !EsNumero(entrada->d_name))
            continue;

        num_procesos++;
        proceso_temp = ObtenerDatosDelProceso(entrada->d_name);
        ImprimirDatosDelProceso(proceso_temp);
    }

    printf("\nNumero de procesos encontrados: %d\n", num_procesos);
    
    closedir(carpeta_procesos);
    return 0;
}

Proceso ObtenerDatosDelProceso(const char *proceso_id){
    //Revisar archivo stat y obtener la informacion
    char nueva_ruta[255];
    Proceso temp;
    snprintf(nueva_ruta, 255, "%s/%s/stat", PROCESO_RUTA_PRINCIPAL, proceso_id);
    FILE *ptrStat = fopen(nueva_ruta, "r"), *ptrCmdLine;

    if(!ptrStat){
        temp.id = 0;
        strcpy(temp.nombre, "N/A");
        temp.prioridad = 0;
        temp.tipo = PROCESO_TIPO_DESCONOCIDO;
        return temp;
    }

    char contenido[1024];

    fgets(contenido, sizeof(contenido), ptrStat);
    sscanf(contenido, "%d (%255[^)]) %c %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %ld", &temp.id, temp.nombre, &temp.estado, &temp.prioridad);
    fclose(ptrStat);

    //Revisar el archivo cmdline y obtener el tipo de proceso

    snprintf(nueva_ruta, 255, "%s/%s/cmdline", PROCESO_RUTA_PRINCIPAL, proceso_id);
    ptrCmdLine = fopen(nueva_ruta, "r");

    if (!ptrCmdLine) {
        temp.tipo = PROCESO_TIPO_DESCONOCIDO;
    } else {
        int dato = fgetc(ptrCmdLine);

        if(dato == EOF){
            temp.tipo = PROCESO_TIPO_KERNEL;
        }else{
            temp.tipo = PROCESO_TIPO_USUARIO;
        }
        fclose(ptrCmdLine);
    }
    return temp;
}

void ImprimirDatosDelProceso(Proceso proceso){
    int desplazamiento = (PROCESO_MAX_NOMBRE - strlen(proceso.nombre)) + 6;

    printf("%08d  %s", proceso.id, proceso.nombre);
    for(int i = 1; i <= desplazamiento; i++) printf(" ");
    printf("%c       %09ld  %s\n", proceso.estado, proceso.prioridad, proceso.tipo == PROCESO_TIPO_KERNEL ? "Kernel" : proceso.tipo == PROCESO_TIPO_USUARIO ? "Usuario" : "Desconocido");

}

int EsNumero(const char *cadena){
    int longitud = strlen(cadena);

    if(longitud <= 0)
        return 0;
    

    for(int i = 0; i < longitud; i++){
        if(!isdigit(cadena[i])){
            return 0;
        }
    }

    return 1;
}

