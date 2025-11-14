#include <stdio.h>   // Para printf()
#include <stdlib.h>  // Para exit()
#include <string.h>  // Para strncpy()

int main(int argc, char *argv[]) {
    // argc: número de argumentos, argv: array con los argumentos
    
    // Variables en el stack (se crean y destruyen automáticamente)
    char buf2[] = "testbuf";  // Variable para verificar overflow (8 bytes)
    char buf[50];              // Buffer principal de 50 caracteres
    
    // Validar que el usuario pasó un argumento
    if (argc <= 1) {
        printf("Usage: %s <string>\n", argv[0]);
        exit(1);  // Termina con código de error
    }
    
    //  CORRECCIÓN: Usar strncpy para limitar la copia
    strncpy(buf, argv[1], sizeof(buf) - 1);  // Copia máximo 49 caracteres
    buf[sizeof(buf) - 1] = '\0';              // Garantiza terminador nulo en posición 49
    
    // Imprimir resultados
    printf("Arg String: %s\n", buf);    // Muestra el string (truncado si era > 49)
    printf("Buf2: %s\n", buf2);          // Verifica que buf2 no se corrompió
    
    // Imprimir direcciones de memoria en formato hexadecimal
    printf("buf address: %p\n", (void*)buf);    // Dirección donde empieza buf
    printf("buf2 address: %p\n", (void*)buf2);  // Dirección donde empieza buf2
    
    return 0;  // Termina exitosamente
}
