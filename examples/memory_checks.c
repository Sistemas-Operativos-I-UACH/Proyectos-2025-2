#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
*
* Memory experiments
* 
*
* References:
* https://www.geeksforgeeks.org/c/memory-layout-of-c-program/
* https://users.cs.jmu.edu/bernstdh/web/common/lectures/summary_vulnerabilities_c_buffer-overflows.php
*
*
* Usage: make memory_checks && ./memory_checks 1234567
*
*/

int main(int argc, char *argv[]) {
    char buf2[] = "testbuf";
    char buf[8];
    if (argc <= 1) {
        printf("Usage: %s <attack string>\n", argv[0]);
        exit(1);
    }
    strcpy(buf, argv[1]);
    printf("Arg String: %s\n", buf);
    printf("Buf2: %s\n", buf2);

    printf("buf address: %lu\n", (unsigned long)buf);
    printf("buf2 address: %lu\n", (unsigned long)buf2);
}
