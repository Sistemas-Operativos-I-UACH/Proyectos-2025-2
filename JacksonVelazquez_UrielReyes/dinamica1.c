#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char *buf;
    char buf2[] = "testbuf";
    if (argc <= 1) {
        printf("Usage: %s <attack string>\n", argv[0]);
        exit(1);
    }

    buf = (char *) malloc(sizeof(char) * 50);
    strcpy(buf, argv[1]);
    printf("Arg String: %s\n", buf);
    printf("Buf2: %s\n", buf2);

    printf("buf address: %p\n", buf);
    printf("buf2 address: %p\n", buf2);

    return 0;
}
