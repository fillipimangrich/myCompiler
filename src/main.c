#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "compiler_state.h"
#include "parser.h"

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
    long file_size = ftell(fp);  
    rewind(fp);

    char * buffer = (char*) malloc(file_size + 1);
    if (!buffer) {
        fprintf(stderr, "Erro ao alocar memória\n");
        fclose(fp);
        exit(1);
    }
    
    fread(buffer, 1, file_size, fp); 
    buffer[file_size] = '\0';

    fclose(fp); 

    cs.current_file_name = argv[2];

    cs.buffer = buffer;
    cs.buffer_size = file_size;
    cs.cursor = 0;

    cs.column = 0;
    cs.line = 0;

    advance();
    program();
    
    free(buffer);
    
    return 0;
}

