#include<stdio.h>
#include<dirent.h>
#include<ctype.h>
#include<stdlib.h>
#include<string.h>


typedef struct {
	int process_Id;
	char proc_name[255];
	char state;
	long priority;
	int kernel;


}Inform;


void InfoProcess(char *direccion, Inform *info){
	FILE * arch = fopen(direccion, "r");
	if(!arch){
	return;
	}
	fscanf(arch,"%d (%[^)]) %c %*ld %*ld %*ld %*ld %*ld %*ld %*ld %*ld %*ld %*ld %*ld %*ld %*ld",
    	&info->process_Id,info->proc_name,&info->state,&info->priority) ;
   
	fclose(arch);
}

int IsKernel(char *nombre){
	FILE *arch = fopen(nombre,"r");
	char info[1024];                	//Variable para leer
   
	if(!arch){
    	return 1;
	}


	int kernel=1;
   
	if(fgets(info,sizeof(info),arch)){
    	if(strlen(info) > 0){
        	kernel=0;
    	}
	}
	fclose(arch);
	return kernel;
}

int main(){
	char proc_path_stat[255];
	char proc_path_cmd[255];
	DIR *pDir;              	//Estructura que representa al directorio abierto
	struct dirent *entry;   	//estructura que Representa la entrada de un directorio
	Inform info;
	int organizar;
	char kernel[10];


	pDir = opendir("/proc");
	if(!pDir){
	printf("No se pudo abrir el directorio /proc\n");
    	return 1;
	}
	printf("PID\t\tNOMBRE\t\t\tESTADO\tPRIORIDAD\tKERNEL/USUARIO\n");
	while((entry = readdir(pDir))!=NULL){
    	if(!isdigit(entry->d_name[0])){
        	continue;
    	}


	sprintf(proc_path_stat,"/proc/%s/stat",entry->d_name);
    	InfoProcess(proc_path_stat,&info);


    	sprintf(proc_path_cmd,"/proc/%s/cmdline",entry->d_name);


   	IsKernel(proc_path_cmd) ? strcpy(kernel,"kernel") : strcpy(kernel,"usuario");


    	printf("%d\t%s",info.process_Id,info.proc_name);
    	organizar = 40 - strlen(info.proc_name);
    	for(int i=0;i<organizar;i++){
        	printf(" ");
    	}
    	printf("%c\t%ld\t%s\n",info.state,info.priority,kernel);


	}
	closedir(pDir);
	return 0;
}
