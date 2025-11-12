#include <stdio.h>   // Para printf()
#include <stdlib.h>  // Para malloc(), free(), exit()
#include <string.h>  // Para strncpy()

int main(int argc, char *argv[]) {
    // argc: número de argumentos, argv: array con los argumentos
    
    // Variables locales
    char buf2[] = "testbuf";  // En el stack (para comparar con heap)
    char *buf;                 // Puntero que apuntará a memoria en el heap
    size_t buf_size = 50;      // Tamaño del buffer (size_t es tipo para tamaños)
    
    // Validar que el usuario pasó un argumento
    if (argc <= 1) {
        printf("Usage: %s <string>\n", argv[0]);
        exit(1);
    }
    
    // CORRECCIÓN #1: Asignar memoria dinámicamente en el heap
    buf = (char *) malloc(sizeof(char) * buf_size);  // Pide 50 bytes al sistema
    
    //  CORRECCIÓN #2: Verificar que malloc funcionó
    if (buf == NULL) {  // malloc retorna NULL si no hay memoria disponible
        printf("Error: malloc failed\n");
        exit(1);  // Terminar si no hay memoria
    }
    
    //  CORRECCIÓN #3: Usar strncpy para limitar la copia
    strncpy(buf, argv[1], buf_size - 1);  // Copia máximo 49 caracteres
    buf[buf_size - 1] = '\0';              // Garantiza terminador nulo en posición 49
    
    // Imprimir resultados
    printf("Arg String: %s\n", buf);    // Muestra el string (truncado si era > 49)
    printf("Buf2: %s\n", buf2);          // buf2 está en stack, no se corrompe en heap overflow
    
    // Imprimir direcciones (buf en heap, buf2 en stack)
    printf("buf address: %p\n", (void*)buf);    // Dirección en el heap
    printf("buf2 address: %p\n", (void*)buf2);  // Dirección en el stack
    
    //  CORRECCIÓN #4: Liberar memoria dinámica
    free(buf);  // Devuelve los 50 bytes al sistema (previene memory leak)
    
    return 0;  // Termina exitosamente
}