#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>

struct Estado {
    char codigo;
    const char *descripcion;
} estados[] = {
    { 'R', "Ejecutando (Running)" },
    { 'S', "Durmiendo (Sleeping)" },
    { 'D', "Espera ininterrumpible" },
    { 'T', "Detenido" },
    { 't', "Trazado/Depuración" },
    { 'Z', "Zombie" },
    { 'I', "Inactivo (Idle)" },
    { 'K', "Wakekill" },
    { 'W', "Waking" },
    { 'X', "Muerto (Dead)" },
    { 'x', "Muerto (Dead)" },
    { 'P', "Parked" },
    { 0,   "Desconocido" }
};

const char* descripcion_estado(char c) {
    for (int i = 0; estados[i].codigo != 0; i++) {
        if (estados[i].codigo == c) return estados[i].descripcion;
    }
    return "Desconocido";
}

struct Proceso {
    int  pid;
    char nombre[256];
    char estado;
    long prioridad;
    char tipo[10];
};

int es_numero(const char *s) {
    if (!s || !*s) return 0;
    for (int i = 0; s[i]; i++) {
        if (!isdigit((unsigned char)s[i])) return 0;
    }
    return 1;
}

int leer_proceso(const char *pid_str, struct Proceso *info) {
    char path[256];
    FILE *f;

    // --- Leer PID, nombre, estado y prioridad ---
    snprintf(path, sizeof(path), "/proc/%s/stat", pid_str);
    f = fopen(path, "r");
    if (!f) return 0;

    int leidos = fscanf(f,
        "%d (%255[^)]) %c"
        " %*d %*d %*d %*d %*d"
        " %*u %*u %*u %*u %*u"
        " %*u %*u %*d %*d"
        " %ld",
        &info->pid, info->nombre, &info->estado, &info->prioridad
    );
    fclose(f);
    if (leidos != 4) return 0;

    // --- Determinar tipo (usuario/kernel) ---
    snprintf(path, sizeof(path), "/proc/%s/cmdline", pid_str);
    f = fopen(path, "r");
    if (!f || fgetc(f) == EOF) {
        strcpy(info->tipo, "kernel");
    } else {
        strcpy(info->tipo, "usuario");
    }
    if (f) fclose(f);

    return 1;
}

int main(void) {
    DIR *dir = opendir("/proc");
    if (!dir) {
        perror("No se pudo abrir /proc");
        return 1;
    }
    printf("------  Proyecto_1-------------------------------------------------------------\n");
    printf("-------------------------------------------------------------------------------\n");
    printf("%6s  %-24s  %-24s  %-9s  %-8s\n",
           "PID", "NOMBRE", "ESTADO", "PRIORIDAD", "TIPO");
    printf("------  ------------------------  ------------------------  ---------  --------\n");

    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) {
        if (!es_numero(ent->d_name)) continue;

        struct Proceso pr;
        if (!leer_proceso(ent->d_name, &pr)) continue;

        printf("%10d  %60s  %c (%-20s)  %9ld  %-8s\n",
               pr.pid, pr.nombre, pr.estado, descripcion_estado(pr.estado),
               pr.prioridad, pr.tipo);
    }

    closedir(dir);
    return 0;
}
