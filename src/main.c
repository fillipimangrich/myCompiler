#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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


Token getNextToken() {
    Token token;
    token.lexeme[0] = '\0';

    while (buffer[cursor] != '\0') {
        char c = buffer[cursor];

        if (isspace(c)) {
            cursor++;
            continue;
        }

        if (isalpha(c)) {
            int start = cursor;
            while (isalnum(buffer[cursor])) cursor++;
            int length = cursor - start;
            strncpy(token.lexeme, &buffer[start], length);
            token.lexeme[length] = '\0';

            if (strcmp(token.lexeme, "def") == 0) token.type = DEF;
            else if (strcmp(token.lexeme, "return") == 0) token.type = RETURN;
            else if (strcmp(token.lexeme, "if") == 0) token.type = IF;
            else if (strcmp(token.lexeme, "else") == 0) token.type = ELSE;
            else if (strcmp(token.lexeme, "for") == 0) token.type = FOR;
            else if (strcmp(token.lexeme, "break") == 0) token.type = BREAK;
            else if (strcmp(token.lexeme, "print") == 0) token.type = PRINT;
            else if (strcmp(token.lexeme, "read") == 0) token.type = READ;
            else if (strcmp(token.lexeme, "new") == 0) token.type = NEW;
            else if (strcmp(token.lexeme, "int") == 0) token.type = INT;
            else if (strcmp(token.lexeme, "float") == 0) token.type = FLOAT;
            else if (strcmp(token.lexeme, "string") == 0) token.type = STRING;
            else token.type = IDENT;
            return token;
        }

        if (isdigit(c)) {
            int start = cursor;
            while (isdigit(buffer[cursor])) cursor++;
            if (buffer[cursor] == '.') {
                cursor++;
                while (isdigit(buffer[cursor])) cursor++;
                token.type = FLOAT_CONST;
            } else {
                token.type = INT_CONST;
            }
            int length = cursor - start;
            strncpy(token.lexeme, &buffer[start], length);
            token.lexeme[length] = '\0';
            return token;
        }

        switch (c) {
            case '=':
                if (buffer[cursor + 1] == '=') {
                    cursor += 2;
                    token.type = EQ;
                    strcpy(token.lexeme, "==");
                } else {
                    cursor++;
                    token.type = ASSIGN;
                    strcpy(token.lexeme, "=");
                }
                return token;
            case '+':
                cursor++;
                token.type = PLUS;
                strcpy(token.lexeme, "+");
                return token;
            case '-':
                cursor++;
                token.type = MINUS;
                strcpy(token.lexeme, "-");
                return token;
            case '*':
                cursor++;
                token.type = MULT;
                strcpy(token.lexeme, "*");
                return token;
            case '/':
                cursor++;
                token.type = DIV;
                strcpy(token.lexeme, "/");
                return token;
            case '%':
                cursor++;
                token.type = MOD;
                strcpy(token.lexeme, "%");
                return token;
            case '<':
                if (buffer[cursor + 1] == '=') {
                    cursor += 2;
                    token.type = LEQ;
                    strcpy(token.lexeme, "<=");
                } else {
                    cursor++;
                    token.type = LT;
                    strcpy(token.lexeme, "<");
                }
                return token;
            case '>':
                if (buffer[cursor + 1] == '=') {
                    cursor += 2;
                    token.type = GEQ;
                    strcpy(token.lexeme, ">=");
                } else {
                    cursor++;
                    token.type = GT;
                    strcpy(token.lexeme, ">");
                }
                return token;
            case '{':
                cursor++;
                token.type = LBRACE;
                strcpy(token.lexeme, "{");
                return token;
            case '}':
                cursor++;
                token.type = RBRACE;
                strcpy(token.lexeme, "}");
                return token;
            case '(':
                cursor++;
                token.type = LPAREN;
                strcpy(token.lexeme, "(");
                return token;
            case ')':
                cursor++;
                token.type = RPAREN;
                strcpy(token.lexeme, ")");
                return token;
            case ';':
                cursor++;
                token.type = SEMICOLON;
                strcpy(token.lexeme, ";");
                return token;
            case ',':
                cursor++;
                token.type = COMMA;
                strcpy(token.lexeme, ",");
                return token;
        }

        cursor++;
        token.type = UNKNOWN;
        token.lexeme[0] = c;
        token.lexeme[1] = '\0';
        return token;
    }

    token.type = END_OF_FILE;
    return token;
}


void lexical_analyzer() {
    Token token;
    
    while (cursor < file_size) {
        token = getNextToken();
        printf("Token: %s , Type: %s\n", token.lexeme, TokenString[token.type]);
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

