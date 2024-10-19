#ifndef COMPILER_STATE_H
#define COMPILER_STATE_H
#include "types.h"

typedef struct {
    char * buffer;
    long buffer_size;
    int cursor;
    Token current_token;
} CompilerState;

extern CompilerState cs;

#endif // COMPILER_STATE_H

