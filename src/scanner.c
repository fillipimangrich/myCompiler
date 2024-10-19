#include "scanner.h"

Token getNextToken() {
    Token token;
    token.lexeme[0] = '\0';

    while (cs.buffer[cs.cursor] != '\0') {
        char c = cs.buffer[cs.cursor];

        if (isspace(c)) {
            cs.cursor++;
            continue;
        }

        if (isalpha(c)) {
            return scanIdent();
        }

        if (isdigit(c)) {
            return scanNumber();
        }

        switch (c) {
            case '=':
                if (cs.buffer[cs.cursor + 1] == '=') {
                    cs.cursor += 2;
                    token.type = EQ;
                    strcpy(token.lexeme, "==");
                } else {
                    cs.cursor++;
                    token.type = ASSIGN;
                    strcpy(token.lexeme, "=");
                }
                return token;
            case '+':
                cs.cursor++;
                token.type = PLUS;
                strcpy(token.lexeme, "+");
                return token;
            case '-':
                cs.cursor++;
                token.type = MINUS;
                strcpy(token.lexeme, "-");
                return token;
            case '*':
                cs.cursor++;
                token.type = MULT;
                strcpy(token.lexeme, "*");
                return token;
            case '/':
                cs.cursor++;
                token.type = DIV;
                strcpy(token.lexeme, "/");
                return token;
            case '%':
                cs.cursor++;
                token.type = MOD;
                strcpy(token.lexeme, "%");
                return token;
            case '<':
                if (cs.buffer[cs.cursor + 1] == '=') {
                    cs.cursor += 2;
                    token.type = LEQ;
                    strcpy(token.lexeme, "<=");
                } else {
                    cs.cursor++;
                    token.type = LT;
                    strcpy(token.lexeme, "<");
                }
                return token;
            case '>':
                if (cs.buffer[cs.cursor + 1] == '=') {
                    cs.cursor += 2;
                    token.type = GEQ;
                    strcpy(token.lexeme, ">=");
                } else {
                    cs.cursor++;
                    token.type = GT;
                    strcpy(token.lexeme, ">");
                }
                return token;
            case '{':
                cs.cursor++;
                token.type = LBRACE;
                strcpy(token.lexeme, "{");
                return token;
            case '}':
                cs.cursor++;
                token.type = RBRACE;
                strcpy(token.lexeme, "}");
                return token;
            case '(':
                cs.cursor++;
                token.type = LPAREN;
                strcpy(token.lexeme, "(");
                return token;
            case ')':
                cs.cursor++;
                token.type = RPAREN;
                strcpy(token.lexeme, ")");
                return token;
            case '[':
                cs.cursor++;
                token.type = LBRACK;
                strcpy(token.lexeme, "(");
                return token;
            case ']':
                cs.cursor++;
                token.type = RBRACK;
                strcpy(token.lexeme, ")");
                return token;
            case ';':
                cs.cursor++;
                token.type = SEMICOLON;
                strcpy(token.lexeme, ";");
                return token;
            case ',':
                cs.cursor++;
                token.type = COMMA;
                strcpy(token.lexeme, ",");
                return token;
        }

        cs.cursor++;
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
        
        while (isspace(cs.buffer[cs.cursor])) cs.cursor++; 

        int tempCursor = cs.cursor;
        
        if (cs.buffer[tempCursor] == '(') {
            token.type = FUNC_IDENT; 
        } else {
            token.type = IDENT;
        }
                
    } 

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
