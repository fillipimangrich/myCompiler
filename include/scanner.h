#ifndef SCANNER_H
#define SCANNER_H
#include <string.h>
#include <ctype.h>
#include "types.h"
#include "compiler_state.h"

Token getNextToken();

Token scanIdent();

Token scanNumber();

Token scanSymbol();


#endif // SCANNER_H

