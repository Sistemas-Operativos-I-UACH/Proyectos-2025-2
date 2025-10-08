#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define MAX_PATH 256
#define BUFFER_SIZE 1024

typedef struct {
    int pid;
    char nombre[256];
    char estado;
    int prioridad;
    char tipo[16];
} InformacionProceso;

int ObtenerTipoProceso(int pid) {
    char path[MAX_PATH];
    FILE *proceso;
    char buffer[BUFFER_SIZE];
    long tam;

    snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);

    proceso = fopen(path, "r");
    if (proceso == NULL) {
        return -1;  // Error
    }

    tam = fread(buffer, 1, sizeof(buffer) - 1, proceso);
    fclose(proceso);

    return tam > 0; // Si el archivo tiene contenido (true), es un proceso de usuario. Si no tiene contenido (false), es un proceso de kernel.
}


int ObtenerInformacionProceso(int pid, InformacionProceso *info) {
    char path[MAX_PATH];
    FILE *proceso;
    char buffer[BUFFER_SIZE];
    int resultado;

    snprintf(path, sizeof(path), "/proc/%d/stat", pid);

    proceso = fopen(path, "r");
    if (proceso == NULL) {
        return -1;
    }

    if (fgets(buffer, sizeof(buffer), proceso) == NULL) {
        fclose(proceso);
        return -1;
    }

    fclose(proceso);

    char *parentesis_abre = strchr(buffer, '(');
    char *parentesis_cierre = strrchr(buffer, ')');

    if (parentesis_abre == NULL || parentesis_cierre == NULL || parentesis_cierre < parentesis_abre) {
        return -1;
    }

    size_t nombre_tam = parentesis_cierre - parentesis_abre - 1;  // Tamaño del nombre del proceso
    if (nombre_tam >= sizeof(info->nombre)) {           	 // Si el tamaño es mayor al espacio disponible
        nombre_tam = sizeof(info->nombre) - 1;          	 // Entonces, lo reducimos y dejamos espacio para el caracter nulo de fin de cadena '/0'
    }
    strncpy(info->nombre, parentesis_abre + 1, nombre_tam);   // Copiamos a info->name, el texto que empieza luego del parentesis de apertura, todos los caracteres del tamaño del nombre del proceso
    info->nombre[nombre_tam] = '\0';  	// Agregamos el caracter nulo de fin de cadena

    char *resto = parentesis_cierre + 1;	 // Nos movemos despues del parentesis de cierre del nombre.

    // Buscamos los campos: 1: PID (ya extraido), 2: Name (ya extraido), 3: State, ... 18: Priority
    char estado;

    resultado = sscanf(resto, " %c %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %d", &estado, &info->prioridad); // Con %*s se saltean campos

    if (resultado != 2) {
        return -1; 	// No se pudieron extraer los campos necesarios
    }

    info->estado = estado;
    info->pid = pid;

    return 0;
}


int main() {
    DIR *dir;
    struct dirent *entrada;

    dir = opendir("/proc"); // Abre el directorio /proc
    if (dir == NULL) {
        perror("Error al abrir /proc");
        return 1;
    }

    printf("ID de Proceso\t| %-48s | %-15s | %-15s | Tipo\n", "Nombre Proceso", "Estado", "Prioridad");
    printf("----------------|--------------------------------------------------|-----------------|-----------------|---------\n");

    while ((entrada = readdir(dir)) != NULL) { 	// Revisamos si hay directorios en la ruta.
        if (entrada->d_type == DT_DIR) {  	// Revisamos si es un directorio.
            int pid = atoi(entrada->d_name); 	// El nombre del directorio es el ID del proceso, asi que lo convertimos a entero.
            if (pid > 0) {   			// Revisamos que sea un ID valido.
                InformacionProceso info;  	// Creamos una variable donde se almacenara toda la informacion del proceso.
                int tipo;

                if (ObtenerInformacionProceso(pid, &info) == 0) {
                    tipo = ObtenerTipoProceso(pid);

                    if (tipo == 1) strcpy(info.tipo, "Usuario");
                    else if (tipo == 0) strcpy(info.tipo, "Kernel");
                    else {
                        if (info.estado == 'Z') strcpy(info.tipo, "Kernel (Zombie)");
                        else strcpy(info.tipo, "Desconocido");
                    }
                    printf("%-15d | %-48s | %-15c | %-15d | %s\n", info.pid, info.nombre, info.estado, info.prioridad, info.tipo);
                }
            }
        }
    }

    if(closedir(dir) == -1){
	perror("Error cerrando el directorio");
	return 1;
    }
    return 0;
}

