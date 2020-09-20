#include <stdio.h>
#include <stdlib.h>

void salir() {
    printf("Saliendo del programa\n");
    exit(EXIT_SUCCESS);
}

int main() {
    printf("Antes de salir()\n");
    
    salir();
    
    printf("Después de salir()\n");
    
    return EXIT_SUCCESS;
}
