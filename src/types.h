#ifndef TYPES_H
#define TYPES_H

typedef enum {
    E0,
    E1,
    E2,
    E3,
    E4
} State;

typedef void(*Handler)(char *, State *);

#endif // TYPES_H

