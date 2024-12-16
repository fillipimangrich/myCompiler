
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

#define RED     "\x1b[31m"
#define RESET   "\x1b[0m"

ErrorContext error_ctx = {0};

void init_error_context(const char* filename, const char* source) {
    error_ctx.filename = filename;
    error_ctx.source_line = source;
    error_ctx.error_count = 0;
    error_ctx.warning_count = 0;
}

char* get_line_from_source(const char* source, int cursor_pos) {
    // Find start of current line by going backwards until newline
    const char* line_start = source + cursor_pos;
    while (line_start > source && *(line_start - 1) != '\n') {
        line_start--;
    }
    
    // Find end of current line
    const char* line_end = source + cursor_pos;
    while (*line_end && *line_end != '\n') {
        line_end++;
    }
    
    // Create copy of current line
    int line_length = line_end - line_start;
    char* line_copy = (char*)malloc(line_length + 1);
    strncpy(line_copy, line_start, line_length);
    line_copy[line_length] = '\0';
    
    return line_copy;
}

void print_error_location(const char* line, int column) {
    // Print the line
    printf("%s%s%s\n", RED, line, RESET);
    
    // Print the caret pointing to the error
    for (int i = 0; i < column; i++) {
        if (isspace(line[i])) {
            printf("%c", line[i]); // Preserve tab alignment
        } else {
            printf(" ");
        }
    }
    printf("^\n");
}

void report_error(ErrorType type, const char* message, Token* token, const char* expected) {
    const char* error_type_str;
    switch (type) {
        case ERROR_LEXICAL:
            error_type_str = "Lexical Error";
            break;
        case ERROR_SYNTAX:
            error_type_str = "Syntax Error";
            break;
        case ERROR_SEMANTIC:
            error_type_str = "Semantic Error";
            break;
        case ERROR_TYPE:
            error_type_str = "Type Error";
            break;
        case WARNING:
            error_type_str = "Warning";
            break;
    }
    
    printf("\n%s:%d:%d: ", 
           error_ctx.filename, 
           token->line,
           token->column);
           
    printf("%s%s: %s%s\n",
           RED,
           error_type_str,
           message,
           RESET);
    
    char* source_line = get_line_from_source(cs.buffer, cs.cursor);
    if (source_line) {
        print_error_location(source_line, token->column );
        free(source_line);
    }
    
    if (type == ERROR_SYNTAX && expected) {
        printf("Expected: %s\n", expected);
        printf("Found: %s\n", token ? token->lexeme : "end of file");
    }
    
    if (type != WARNING) {
        error_ctx.error_count++;
    } else {
        error_ctx.warning_count++;
    }
    
    // Exit on errors (except warnings)
    if (type != WARNING && error_ctx.error_count > 0) {
        printf("\nToo many errors. Compilation aborted.\n");
        exit(1);
    }
}

void syntax_error(const char* message, Token* token, const char* expected) {
    report_error(ERROR_SYNTAX, message, token, expected);
}

void semantic_error(const char* message, Token* token) {
    report_error(ERROR_SEMANTIC, message, token, NULL);
}

void type_error(const char* message, Token* token) {
    report_error(ERROR_TYPE, message, token, NULL);
}

void lexical_error(const char* message, int line, int column) {
    Token dummy_token = {.line = line, .column = column};
    report_error(ERROR_LEXICAL, message, &dummy_token, NULL);
}

void warning(const char* message, Token* token) {
    report_error(WARNING, message, token, NULL);
}