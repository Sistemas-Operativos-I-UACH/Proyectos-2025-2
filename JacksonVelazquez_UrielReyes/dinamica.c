#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 50

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

// int main(int argc, char *argv[]) {
//     char *buf;
//     char buf2[] = "testbuf";
//     if (argc <= 1) {
//         printf("Usage: %s <attack string>\n", argv[0]);
//         exit(1);
//     }

//     buf = (char *) malloc(sizeof(char) * 50);
//     strcpy(buf, argv[1]);
//     printf("Arg String: %s\n", buf);
//     printf("Buf2: %s\n", buf2);

//     printf("buf address: %p\n", buf);
//     printf("buf2 address: %p\n", buf2);

//     return 0;
// }

int main (int argc, char *argv[]) {

    char *buf;
    char buf2[] = "testbuf";

    char entrada[256]; //este simplemente lo usaremos para validar tamaños de cadenas, SOLO VALIDACIÓN

    buf = (char *) malloc (sizeof(char) * BUF_SIZE); //Ahora asignamos espacio de memoria 

        if (buf == NULL) 
        { //checa que la asignación se hizo correctamente
            fprintf(stderr, "Error: malloc falló\n");
            return 1;
        }

    //Primero checamos si el usuario ingresó o no argumentos en consola
    if (argc <= 1) //si el argumento es 1 o 0 significa que no introdujo nada más que la ruta del programa a ejecutar
    { 
        
        do{
            printf("Ingresa un cadena (max %d caracteres): ", BUF_SIZE - 1);
            
                if (fgets(entrada, sizeof(entrada), stdin) == NULL) //verificamos si hubo un error al leer la entrada del usuario
                {
                    fprintf(stderr, "Error leyendo entrada\n");
                    free(buf);
                    exit(1);
                }

            entrada[strcspn(entrada, "\n")] = '\0';

                if (strlen(entrada) >= BUF_SIZE) 
                {
                    printf("PROBLEMA: La entrada es mayor a %d.\nVuelva a ingresar.\n", BUF_SIZE);
                }

        }while(strlen(entrada) >= BUF_SIZE);

    } 
    else 
    { //si sí agregaron argumento de cadena, entonces, nada más comprobamos que no sobrepase 'BUF_SIZE'
        argv[1][strcspn(argv[1], "\n")] = '\0';
        strcpy(entrada, argv[1]);

        if (strlen(entrada) >= (size_t)BUF_SIZE) 
        {
            printf("PROBLEMA: La entrada es mayor a %d.\nVuelva a ingresar.\n", BUF_SIZE);
            //return 1;
                do //volvemos a pedir los datos para no solamente darle término al programa con el 'return 1;'
                {
                    printf("Ingresa un cadena (max %d caracteres): ", BUF_SIZE - 1);

                        if (fgets(entrada, sizeof(entrada), stdin) == NULL) //verificamos si hubo un error al leer la entrada del usuario
                        {
                            fprintf(stderr, "Error leyendo entrada\n");
                            free(buf);
                            exit(1);
                        }

                    entrada[strcspn(entrada, "\n")] = '\0';

                        if (strlen(entrada) >= (size_t)BUF_SIZE) 
                        {
                            printf("PROBLEMA: La entrada es mayor a %d.\nVuelva a ingresar.\n", BUF_SIZE - 1);
                        }
                    
                }while(strlen(entrada) >= (size_t)BUF_SIZE);
        }
    }

    //impresión de las cadenas
    strncpy(buf, entrada, 50);
    printf("Arg String: %s\n", buf);
    printf("Buf2: %s\n", buf2);

    printf("buf address: %p\n", (void*)buf); //usamos %p por portabilidad
    printf("buf2 address: %p\n", (void*)buf2);

    free(buf);

    return 0;
}