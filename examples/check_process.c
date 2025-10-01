#include <stdio.h>
#include <dirent.h>
#include <ctype.h>
#include <stdlib.h>

typedef struct {
    int   process_id;
    char  process_name[256];
    char  state;
    long  priority;
    int   is_kernel;
} proc_info;

int check_process_type(char *filename, char *procname);
void get_process_info(char *filename, proc_info *info);

int main (int argc, char *argv[]) {
    struct dirent *entry;
    DIR *pDir;
    char proc_dir[] = "/proc";
    char proc_id_dir[255];
    char cmdline[255];
    proc_info info;

    // Ensure we can open directory.

    pDir = opendir (proc_dir);
    if (pDir == NULL) {
        printf ("Cannot open directory '%s'\n", argv[1]);
        return 1;
    }

    // Process each entry.

    while ((entry = readdir(pDir)) != NULL) {
        if ( ! isdigit(entry->d_name[0]) )
            continue;

        sprintf(proc_id_dir, "/proc/%s/cmdline", entry->d_name);
        //get_process_info(proc_id_dir, &info);
        info.is_kernel = check_process_type(proc_id_dir, cmdline);
        printf("%s Is kernel process: %d\n", cmdline, info.is_kernel);
        //printf ("%s:%s\n", entry->d_name, cmdline);
    }

    // Close directory and exit.

    closedir (pDir);
    return 0;
}

/*
*
* Check if a process is a kernel or a user process
* Returns:
* 0 if the process is user process
* 1 if the process is a kernel process
* Puts the content of the checked filename in the procname variable
*
*/
int check_process_type(char *filename, char *procname) {
    FILE *fp;
    char *line = NULL;
    size_t len = 0;

    procname[0] = '\0';

    // Open file in read-only mode
    fp = fopen(filename,"r");

    // If file opened successfully, then print the contents
    if ( fp == NULL ) {
        printf("Error abriendo archivo %s\n", filename);
        exit(2);
    }

    while(getline(&line, &len, fp) != -1) {
        sprintf(procname, "%s%s", procname, line);
    }

    fclose(fp);
    free(line);

    if (procname[0] == '\0')
        return 1;

    return 0;
}

/*
void get_process_info(char *filename, proc_info *info) {
  FILE *fp;
  char *line = NULL;
  size_t len = 0;
  procname[0] = '\0';

  // Open file in read-only mode
  fp = fopen(filename,"r");

  // If file opened successfully, then print the contents
  if ( fp == NULL ) {
    printf("Error abriendo archivo %s\n", filename);
    exit(2);
   }

  while(getline(&line, &len, fp) != -1) {
    sprintf(procname, "%s%s", procname, line);
  }

  fclose(fp);
  free(line);
}*/
