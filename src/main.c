#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "handlers.h"


char *buffer = NULL;
long file_size;
int cursor = 0;
State state = 0;

Handler handler[] = {
    handle_E0,
    handle_E1
};


void lexical_analyzer() {
    while (cursor < file_size) {
        char c = buffer[cursor];
        
        handler[state](&c, &state);

        cursor++;
    }
}


int main(int argc, char **argv) 
{
    FILE *fp = NULL;
    
    if (argc != 2) {
        fprintf(stderr, "Usage: %s file.txt\n", argv[0]);
        exit(1);
    }
    
    if (!(fp = fopen(argv[1], "rb"))) {
        perror(argv[1]);
        exit(1);
    }
    
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);  
    rewind(fp);

    buffer = (char*) malloc(file_size + 1);
    if (!buffer) {
        fprintf(stderr, "Erro ao alocar memória\n");
        fclose(fp);
        exit(1);
    }
    
    fread(buffer, 1, file_size, fp); 
    buffer[file_size] = '\0';

    fclose(fp); 

    lexical_analyzer();

    free(buffer);
    
    return 0;
}


