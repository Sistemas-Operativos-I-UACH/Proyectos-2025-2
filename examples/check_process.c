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

    // Ensure we can open direc"tory.

    pDir = opendir (proc_dir);
    if (pDir == NULL) {
        printf ("Cannot open directory '%s'\n", argv[1]);
        return 1;
    }

    // Process each entry.

    while ((entry = readdir(pDir)) != NULL) {
        if ( ! isdigit(entry->d_name[0]) )
            continue;

        // Check if it's user or kernel process
        sprintf(proc_id_dir, "/proc/%s/cmdline", entry->d_name);
        info.is_kernel = check_process_type(proc_id_dir, cmdline);

        // Get information from the stat file
        sprintf(proc_id_dir, "/proc/%s/stat", entry->d_name);
        get_process_info(proc_id_dir, &info);
        // Print process information
        printf("\n-------------------------------------\n");
        printf("\nProcess ID: %d\n", info.process_id);
        printf("Process Name: %s\n", info.process_name);
        printf("State: %c\n", info.state);
        printf("Priority: %ld\n", info.priority);
        printf("Is Kernel process: %s\n", info.is_kernel ? "Yes" : "No");
    }
    printf("\n-------------------------------------\n");

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
 * 
 * Reads the /proc/<pid>/stat file and stores: process id, process name,
 * and process state in the info variable of type proc_info
 * Arguments:
 * filename: the path of the /proc/<pid>/stat of the process
 * info: proc_info type variable
 *
 */
void get_process_info(char *filename, proc_info *info) {
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    char stat_content[1024];
    stat_content[0] = '\0';
    unsigned int flags;

    // Open file in read-only mode
    fp = fopen(filename,"r");

    // If file opened successfully, then print the contents
    if ( fp == NULL ) {
        printf("Error abriendo archivo %s\n", filename);
        exit(2);
    }

    while(getline(&line, &len, fp) != -1) {
        sprintf(stat_content, "%s%s", stat_content, line);
    }

    // Read the fields and store them in the info variable
    sscanf(stat_content,
        "%d (%1024[^)]) %c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %*u %*u %*d %*d %ld",
        &info->process_id,
        info->process_name,
        &info->state,
        &info->priority
    );


    fclose(fp);
    free(line);
}
