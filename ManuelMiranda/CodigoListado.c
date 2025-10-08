#include <stdio.h> //PARA ENTRADA Y SALIDA 
#include <stdlib.h> //FUNCIONES GENERALES TALES COMO ATOI Y ATOL
#include <dirent.h> //PARA LEER LOS DIRECTORIOS opendir y readdir
#include <string.h> //MANIPULACION DE LAS CADENAS
#include <ctype.h> //FUNCIONES COMO isdigit
#include <unistd.h> //FUNCIONES DEL SISTEMA pid_t
#include <sys/types.h> //TIPOS DE DATOS DEL SISTEMA


#define Max_Lines 512
#define Max_Process 1024

//ESTRUCTURA PARA AGRUPAR LOS DATOS
typedef struct{
    pid_t pid; //PID DEL PROCESO
    char Nombre[256];//NOMBRE DEL PROGRAMA
    char Estado;//CARACTER QUE REPRESENTA EL ESTADO
    int Prioridad;//NUMERO DE IMPORTANCIA DEL PROCESO
    long Utime;//TIEMPO EN MODO USUARIO, NO DEL KERNEL
    long Stime;//TIEMPO EN MODO DEL KERNEL
}Proceso;

//FUNCION PARA LA LECTURA DE LOS DATOS
//VA A RECIRBIR EL NUMERO DEL PROCESO, pid, Y UN PUNTERO A LA ESCTRUCTURA PROCESO
int LeerInfoProceso(pid_t Pid,Proceso*proc){
    char filename[256]; //PARA ALMACENAR LA RUTA DEL ARCHIVO
    char Line[Max_Lines];// BUFFER PARA LEER LAS LINEAS DEL ARCHIVO
    char comm[256];//AQUI SE VA A GUARDAR EL NOMBRE DEL PROCESO TEMPORALMENTE

   sprintf(filename,"/proc/%d/stat",Pid); //CREA UNA CADENA /proc/pid/stat

    FILE *file = fopen(filename,"r");
    if(!file){
        return -1; //SI EL ARCHIVO NO SE PUDO ABRIR, SE TERMINA EL PROCESO
    }

    //PROCESAMIENTO DEL ARCHIVO
    if(fgets(Line,sizeof(Line),file)){ //LEE LA LINEA DE COMANDOS SEPARADOS POR ESPACIO
        char *token = strtok(Line," "); 
        int campo = 1;
        //CON strtok PARTO LOS CAMPOS DE LA DIRECCION DEL PROCESO, DE TAL FORMA QUE ME 
        //QUEDA LA CADENA DELA DIRECCION EN CAMPOS SEPARADOS.
        while(token != NULL){
            switch(campo){
                case 1://PID
                    proc->pid = atoi(token); //atoi CONVIERTE UNA CADENA DE CARACTERES
                    //EN UN VALOR ENTERO QUE REPRESENTA ESA CADENA DE CARACTERES 
                    //OSEA ME CONVIERTE EL TOKEN EN EL VALOR DEL PID
                    break;
                case 2://NOMBRE CON EL PARENTESIS
                    strncpy(comm,token,sizeof(comm)-1);
                    if(comm[0]=='('){
                        memmove(comm,comm+1,strlen(comm)); //memmove REMOVERA EL ( SI EXISTE
                        //EN ESTE CASO memmove SU FUNCION PRINCIPAL ES MOVER UN BLOQUE DE 
                        //MEMORIA DE UNA UBICACION A OTRA, INCLUSO SI SE SUPERPONEN
                
                        if(comm[strlen(comm)-1]==')'){
                            comm[strlen(comm)-1] ='\0';//SE ELIMAN LOS PARENTESIS CON UN CAMPO NULO
                            //YA QUE SE LEE Comm, LA ULTIMA POSICION SI ES PARENTESIS SE REMPLAZA CON EL CARACTER NULO
                        }
                    }
                    strncpy(proc->Nombre,comm,sizeof(proc->Nombre)-1);
                    break;
                case 3://ESTADO DEL PROCESO
                    proc->Estado = token[0]; //ME VA A GUARDAR EL ESTADO DEL PROCESO
                    break;
                case 18:
                    //PRIORIDAD CONVERTIDO A ENTERO
                    proc->Prioridad = atoi(token); //SE USA PARA CADENAS MAS CORTAS
                    break;
                case 14://TIEMPO DE USUARIO CONVERTIDO A LONG
                    proc->Utime = atol(token); //ATOL SE USA PARA CADENAS MAS LARGAS
                    break;
                case 15:// TIEMPO DE KERNEL CONVERTIVO A LONG
                    proc->Stime = atol(token);
                    break;
            }//FIN DEL SWITCH
            token = strtok(NULL," ");  //AQUI OBTIENE EL SIGUIENTE TOKEN
            campo++; //AUMENTA LA POSICION DEL CAMPO DE LA CADENA DEL Comm
        }
    }
    fclose(file);
    return 0;
}//FIN FUNCION DE LECTURA

const char* EstadoATexto(char estado){
    switch(estado){
        case 'R':
            return "Running(Ejecutandose)";
        case 'S':
            return "Sleeping(Durmiendo)";
        case 'D':
            return "Disk Sleep(E/S Disco)";
        case 'K':
            return "Wake Kill(Matar al despertar)";
        case 'P':
            return "Parked(Estacionado)"; //NO ESTA EN LACOLA DE PLANIFICACION
        case 'I':
            return "Idle(Inactivo)";
        case 'T':
            return "Stopped(Detenido)";
        case 't':
            return "Stop Stracing(Depuracion)";//PROCESO TERMINADO POR UNA SEÑAL
        case 'Z':
            return "Zombie";
        case 'X':
            return "Dead(Muerto)";
        case 'L':
            return "Locked(Bloqueado en memoria)";
        case 'N':
            return "Noise(Proceso ruidoso)";//GENERA MUCHA ACTIVIDAD
        default:
            return "Unknown(Proceso desconocido)";
    }
}

void MostrarProcesoCompleto(){
    DIR *dir; //ME ABRE EL DIRECTORIO
    struct dirent *entry; //ME AYUDA A LEER LAS ENTRADAS DEL DIRECTORIO
    Proceso procesos[Max_Process]; //AQUI SE ALMACENAN LOS PROCESOS
    int count = 0; //ME CUENTA LA CANTIDAD DE PROCESOS QUE ENCUENTRA

    printf("\n%-8s %-25s %-12s %-10s %-12s %-12s","PID","Nombre","Estado","Prioridad","User time","Kernel time");//ENCABEZADO DE COLUMNAS ALINEADAS
    
    dir = opendir("/proc");
    //opendir ABRE EL DIRECTORIO PROC QUE TIENE LA INFORMACION EL KERNEL EN TIEMPO REAL
    if(dir==NULL){
        perror("Error al abrir el /proc");
        return;
    }

    //CON ESTE CICLO ESTAMOS LEYENDO EL NOMBRE EL DIRECTORIO, POR LO TANTO,
    //SI EL CARACTER NO ES NULO Y ADEMAS EL CONTADOR ES MENOR AL MAXIMO TAMAÑO
    //ENTONCES ES CUANDO NOSOTROS CONFIRMAMOS SI EL DIRECTORIO SI ES UN DIRECTORIO, Y ADEMAS
    //SI ESTE VALOR ES UN DIGITO, QUE ES EL VALOR DEL PID NECESARIO
    while((entry = readdir(dir)) != NULL && count < Max_Process){
        if(entry->d_type == DT_DIR && isdigit(entry->d_name[0])){
            pid_t Pid = atoi(entry->d_name);

            if(LeerInfoProceso(Pid,&procesos[count])==0){
                count++;
            }
        }
    }
    closedir(dir);
    
    //IMPRESION DE LOS RESULTADOS
    for(int i = 0;i<count;i++){
        printf("%-8d %-25s %-12s %-10d %-12ld %-12ld\n",  //PARA CADA PROCESO QUE ENCUENTRA ME IMPRIME LA INFORMACION.
               procesos[i].pid, procesos[i].Nombre, 
               EstadoATexto(procesos[i].Estado), procesos[i].Prioridad, 
               procesos[i].Utime, procesos[i].Stime);
    }
    printf("Total de procesos %d\n",count);

}//FIN METODO MOSTRAR COMPLETO

//MOSTRANDO LA INFORMACION ADICIONAL DEL SISTEMA
void MostrarEstadisticas(){
    char Line[Max_Lines];
    FILE *file;

    file = fopen("/proc/uptime","r"); //ESTE ES UN ARCHIVO ESPECIAL DEL KERNEL, SEGUNDOS DESDE EL ULTIMO REINICIO Y SEGUNDOS EN IDLE
    if(file){ //CONFIRMAMOS QUE EL ARCHVO SE ABRIO CORRECTAMENTE
        double uptime;//VARIABLE QUE ALMACENA LOS SEGUNDOS
        fscanf(file,"%lf",&uptime); //LEE EL PRIMER NUMERO DEL ARCHIVO COMO UN LONG FLOAT, COLOCAMOS EL &uptime PORQUE ES LA DIRECCION DE MEMORIA DONDE GUARDAR EL VALOR
        
        fclose(file);
        printf("Uptime del sistema: %.2f segundos (%.2f minutos)\n",uptime,uptime/60);
        //MUESTRA LA INFORMACION
    }
    
    file = fopen("/proc/loadavg","r"); //AQUI NOS MUESTRA LA CARGA DEL SISTEMA LOS UTLIMOS 1, 5 Y 15 MINUTOS
    //EJEMPLO
    /*
    0.45 0.30 0.25 2/500 12345
    
    0.45 - CARGA DEL ULTIMO MINUTO
    0.30 - CARGA DE LOS ULTIMOS 5 MINUTOS
    0.25 - CARGA DE LOS ULTIMOS 15
    2/500 PROCESOS ACTIVOS/TOTAL DE PROCESOS
    12345 - PID DEL PROCESO 
    Carga < 1 = bien
    Carga = 1 = Cuidado
    Carga > 1 = Sistema excedido
    
    */
    
    if(file){
        fgets(Line,sizeof(Line),file); //LEE EL ARCHIVO CARACTER POR CARACTER Y DESPUES LO VA GUARDANDO EN EL LINE[] CON LA POSICION ITERATIVA.
        printf("Carga del sistema: %s",Line);
        fclose(file);
    }
}//FIN FUNCION MOSTRAR ESTADISTICAS

int main(){
    MostrarEstadisticas();
    MostrarProcesoCompleto();
    return 0;
}

