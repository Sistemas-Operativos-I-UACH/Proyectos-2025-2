#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>

typedef struct {
    int   id_proceso;
    char  nombre_proceso[256];
    char  estado;
    long  prioridad;
    int   es_kernel;
} info_proceso;

int verif_tipo_proc(char *nombre_archivo) {
    FILE *archivo = fopen(nombre_archivo, "r");
    if (archivo == NULL) {
        return 1;
    }

    int primerCar = fgetc(archivo);
    fclose(archivo);

    if (primerCar == EOF) {
        return 1;  
    } else {
        return 0;
    }
}

void obt_info_proc(char *nombre_archivo, info_proceso *info) {
    FILE *archivo = fopen(nombre_archivo, "r");
    if (archivo == NULL) {
        return;
    }

    char buffer[1024];
    if (fgets(buffer, sizeof(buffer), archivo)) {
        sscanf(buffer, "%d (%255[^)]) %c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %*u %*u %*d %*d %ld",
               &info->id_proceso, info->nombre_proceso, &info->estado, &info->prioridad);
    }
    fclose(archivo);
}

int main() {
    struct dirent *entrada;
    DIR *directorio;
    char ruta_stat[256];
    char ruta_cmdline[256];
    info_proceso info;

    directorio = opendir("/proc");
    if (!directorio) {
        printf("Error abriendo el directorio /proc\n");
        return 1;
    }

    printf("%-10s %-20s %-10s %-10s %s\n", "PID", "Nombre", "Estado", "Prioridad", "Tipo");
    printf("----------------------------------------------------------------\n");

    while ((entrada = readdir(directorio)) != NULL) {
        if (!isdigit(entrada->d_name[0])) continue;

        sprintf(ruta_stat, "/proc/%s/stat", entrada->d_name);
        sprintf(ruta_cmdline, "/proc/%s/cmdline", entrada->d_name);

        obt_info_proc(ruta_stat, &info);
        info.es_kernel = verif_tipo_proc(ruta_cmdline);

        if (info.es_kernel) {
            printf("%-10d %-20.20s %-10c %-10ld %s\n", 
                   info.id_proceso, info.nombre_proceso, info.estado, info.prioridad, "Kernel");
        } else {
            printf("%-10d %-20.20s %-10c %-10ld %s\n", 
                   info.id_proceso, info.nombre_proceso, info.estado, info.prioridad, "Usuario");
        }
    }

    closedir(directorio);
    return 0;
}
