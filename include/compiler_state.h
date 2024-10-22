#ifndef COMPILER_STATE_H
#define COMPILER_STATE_H
#include "types.h"

typedef struct {
    //file name
    char * current_file_name;

    //file buffer
    char * buffer;
    long buffer_size;
    int cursor;

    //token
    Token current_token;

    //position
    int column;
    int line;
} CompilerState;

extern CompilerState cs;

#endif // COMPILER_STATE_H

