#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>


int main () {

   system("clear");

   DIR *directorio = opendir("/proc");
   struct dirent *entry; //Apuntador a la estructura dirent

   int tamName;

   printf("PID\t\tNOMBRE\t\t\t\t\tESTADO\t\tPRIORIDAD\tPROCESO\n\n");

   while((entry = readdir(directorio)) != NULL){
       if (isdigit(entry->d_name[0])) {
           //int pid = atoi(entry->d_name);
              
               char statPATH[256] = "/proc/";
               strcat(statPATH, entry->d_name);
               strcat(statPATH, "/stat");

               char line[1024];
               FILE *fstat = fopen(statPATH, "r");
               if (!fstat) continue;  // Hace que salte el PID si es que no puede abrir el archivo y así no puede haber fallas
               fgets(line, sizeof(line), fstat);
               fclose(fstat);
               int pid, priority;
               char name[256], state;
               //EL sscanf lee una cadena en memoria, es decir, es lo que le va llegando
               sscanf(line, "%d (%[^)]) %c %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %d", &pid, name, &state, &priority);

               tamName = strlen(name);
               tamName = 40 - tamName;

               //IMPRIMIR DATOS SOLICITADOS
               printf("%d\t\t", pid);
               printf("%s", name);
               for (int i=0; i<=tamName; i++){
                   printf(" ");
               }
               printf("%c\t\t", state);
               printf("%d\t\t", priority);

                //CHECAMOS KERNEL o USUARIO
               char cmdPATH[256] = "/proc/";
               strcat(cmdPATH, entry->d_name);
               strcat(cmdPATH, "/cmdline");
              
               FILE *fcmd = fopen(cmdPATH, "r");

               int isKernel = 1; // Se asume que es KERNEL (variable BOOLEANA)
               if (fcmd) {
                   if (fgets(line, sizeof(line), fcmd)) {
                       if (strlen(line) > 0) isKernel = 0;
                   }
                   fclose(fcmd);
               }
               printf("%s\n", isKernel ? "KERNEL" : "USUARIO");
       }
   }

   closedir(directorio);

   return 0;
}
