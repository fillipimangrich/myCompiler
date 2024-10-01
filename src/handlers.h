#ifndef HANDLER_H
#define HANDLER_H

#include <ctype.h> 
#include "types.h"


void handle_E0(char * cref, State * state){
    char c = *cref;
    if(isalpha(c)){
        printf(" E0 ");
        *state = E1;
    }
}


void handle_E1(char * cref, State * state){
    char c = *cref;
    if(isalpha(c)){
        printf(" E1 ");
        *state = E1;
    }
}

#endif // HANDLER_H