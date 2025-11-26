#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char buf2[] = "testbuf";
    char buf[50];
    if (argc <= 1) {
        printf("Usage: %s <attack string>\n", argv[0]);
        exit(1);
    }
    strcpy(buf, argv[1]);
    printf("Arg String: %s\n", buf);
    printf("Buf2: %s\n", buf2);

    printf("buf address: %lu\n", (unsigned long)buf);
    printf("buf2 address: %lu\n", (unsigned long)buf2);

    return 0;
}