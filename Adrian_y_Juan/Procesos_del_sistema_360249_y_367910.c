// Actividad: Procesos del sistema
// Integrantes del equipo:
// - 360249 Adrian Ricardo Galicia Gutierrez
// - 367910 Juan David Rocha Montelongo
// Fecha de la elaboracion: 01/10/2025

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>

int main() {
    DIR *directorio;
    struct dirent *entrada;
    char ruta[256];
    FILE *archivo;
    int pid;
    char nombre[256];
    char estado;
    long prioridad;
    char buffer[1024];
    char cmdline[256];
    char tipo[16];

    directorio = opendir("/proc");
    if (directorio == NULL) {
        printf("No se pudo abrir el directorio /proc\n");
        return 1;
    }

    printf("PID\t\tNOMBRE\t\tESTADO\t\tPRIORIDAD\tTIPO\n");
    printf("--------------------------------------------------------------------------------------\n");

    while ((entrada = readdir(directorio)) != NULL) {
        if (!isdigit(entrada->d_name[0]))
            continue;

        sprintf(ruta, "/proc/%s/stat", entrada->d_name);
        archivo = fopen(ruta, "r");
        if (archivo == NULL)
            continue;

        if (fgets(buffer, sizeof(buffer), archivo) != NULL) {
            sscanf(buffer,
                   "%d (%[^)]) %c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %*u %*u %*d %*d %ld",
                   &pid, nombre, &estado, &prioridad);
        }
        fclose(archivo);

        sprintf(ruta, "/proc/%s/cmdline", entrada->d_name);
        archivo = fopen(ruta, "r");
        if (archivo == NULL)
            continue;

        if (fgets(cmdline, sizeof(cmdline), archivo) == NULL || cmdline[0] == '\0')
            strcpy(tipo, "Kernel");
        else
            strcpy(tipo, "Usuario");

        fclose(archivo);

        printf("%d\t\t%-16s\t%c\t\t%ld\t\t%s\n",
               pid, nombre, estado, prioridad, tipo);
    }

    closedir(directorio);
    return 0;
}
