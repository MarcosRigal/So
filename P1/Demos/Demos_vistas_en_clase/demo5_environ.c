#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char **environ;

int main() {
    for (int i=0; environ[i] != NULL; ++i) {
        printf("%s\n", environ[i]);
    }
    
    return EXIT_SUCCESS;
}
