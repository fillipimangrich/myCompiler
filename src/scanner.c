#include "scanner.h"

Token getNextToken() {
    Token token;
    token.lexeme[0] = '\0';
    int previous_cursor = cs.cursor;

    while (cs.buffer[cs.cursor] != '\0') {
        char c = cs.buffer[cs.cursor];

        if (isspace(c)) {
            if (c == '\n') {
                ++cs.line;
                cs.column = 0;  // Reiniciar coluna em nova linha
            }
            ++cs.cursor;
            continue;
        }

        if (isalpha(c)) {
            return scanIdent();
        }

        if (isdigit(c)) {
            return scanNumber();
        }

        switch (c) {
            ++cs.cursor;
            case '=':
                if (cs.buffer[cs.cursor + 1] == '=') {
                    ++cs.cursor;
                    token.type = EQ;
                    strcpy(token.lexeme, "==");
                } else {
                    token.type = ASSIGN;
                    strcpy(token.lexeme, "=");
                }
                return token;
            case '+':
                token.type = PLUS;
                strcpy(token.lexeme, "+");
                return token;
            case '-':
                token.type = MINUS;
                strcpy(token.lexeme, "-");
                return token;
            case '*':
                token.type = MULT;
                strcpy(token.lexeme, "*");
                return token;
            case '/':
                token.type = DIV;
                strcpy(token.lexeme, "/");
                return token;
            case '%':
                token.type = MOD;
                strcpy(token.lexeme, "%");
                return token;
            case '<':
                if (cs.buffer[cs.cursor + 1] == '=') {
                    ++cs.cursor;
                    token.type = LEQ;
                    strcpy(token.lexeme, "<=");
                } else {
                    token.type = LT;
                    strcpy(token.lexeme, "<");
                }
                return token;
            case '>':
                if (cs.buffer[cs.cursor + 1] == '=') {
                    ++cs.cursor;
                    token.type = GEQ;
                    strcpy(token.lexeme, ">=");
                } else {
                    token.type = GT;
                    strcpy(token.lexeme, ">");
                }
                return token;
            case '{':
                token.type = LBRACE;
                strcpy(token.lexeme, "{");
                return token;
            case '}':
                token.type = RBRACE;
                strcpy(token.lexeme, "}");
                return token;
            case '(':
                token.type = LPAREN;
                strcpy(token.lexeme, "(");
                return token;
            case ')':
                token.type = RPAREN;
                strcpy(token.lexeme, ")");
                return token;
            case '[':
                token.type = LBRACK;
                strcpy(token.lexeme, "(");
                return token;
            case ']':
                token.type = RBRACK;
                strcpy(token.lexeme, ")");
                return token;
            case ';':
                token.type = SEMICOLON;
                strcpy(token.lexeme, ";");
                return token;
            case ',':
                token.type = COMMA;
                strcpy(token.lexeme, ",");
                return token;
        }

        token.type = UNKNOWN;
        token.lexeme[0] = c;
        token.lexeme[1] = '\0';
        return token;
    }

    token.type = END_OF_FILE;
    return token;
}

Token scanIdent(){
    Token token;
    int start = cs.cursor;

    while (isalnum(cs.buffer[cs.cursor])) cs.cursor++;

    int length = cs.cursor - start;
    strncpy(token.lexeme, &cs.buffer[start], length);
    token.lexeme[length] = '\0';

    if (strcmp(token.lexeme, "def") == 0) token.type = DEF;
    else if (strcmp(token.lexeme, "return") == 0) token.type = RETURN;
    else if (strcmp(token.lexeme, "if") == 0) token.type = IF;
    else if (strcmp(token.lexeme, "else") == 0) token.type = ELSE;
    else if (strcmp(token.lexeme, "for") == 0) token.type = FOR;
    else if (strcmp(token.lexeme, "break") == 0) token.type = BREAK;
    else if (strcmp(token.lexeme, "int") == 0) token.type = INT;
    else if (strcmp(token.lexeme, "float") == 0) token.type = FLOAT;
    else if (strcmp(token.lexeme, "string") == 0) token.type = STRING;
    else if (strcmp(token.lexeme, "not") == 0) token.type = NOT;
    else if (strcmp(token.lexeme, "and") == 0) token.type = AND;
    else if (strcmp(token.lexeme, "or") == 0) token.type = OR;
    else{
        
        while (isspace(cs.buffer[cs.cursor])){
            ++cs.cursor; 
            ++cs.line;
        } 
        
        if (cs.buffer[cs.cursor] == '(') {
            token.type = FUNC_IDENT; 
        } else {
            token.type = IDENT;
        }
                
    } 
    
    cs.column += length;

    return token;
}

Token scanNumber(){
    Token token;
    int start = cs.cursor;

    while (isdigit(cs.buffer[cs.cursor])) cs.cursor++;

    if (cs.buffer[cs.cursor] == '.') {
        cs.cursor++;
        while (isdigit(cs.buffer[cs.cursor])) cs.cursor++;
        token.type = FLOAT_CONST;
    } else {
        token.type = INT_CONST;
    }

    int length = cs.cursor - start;
    strncpy(token.lexeme, &cs.buffer[start], length);
    token.lexeme[length] = '\0';
    return token;
}