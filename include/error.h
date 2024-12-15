#ifndef ERROR_H
#define ERROR_H

#include "types.h"
#include "compiler_state.h"

typedef enum {
    ERROR_LEXICAL,
    ERROR_SYNTAX,
    ERROR_SEMANTIC,
    ERROR_TYPE,
    WARNING
} ErrorType;

// Store error context for better reporting
typedef struct {
    const char* filename;
    const char* source_line;
    int line;
    int column;
    int error_count;
    int warning_count;
} ErrorContext;

extern ErrorContext error_ctx;

// Initialize error context
void init_error_context(const char* filename, const char* source);

// Main error reporting function
void report_error(
    ErrorType type,
    const char* message,
    Token* token,
    const char* expected
);

// Helper functions for specific error types
void syntax_error(const char* message, Token* token, const char* expected);
void semantic_error(const char* message, Token* token);
void type_error(const char* message, Token* token);
void lexical_error(const char* message, int line, int column);
void warning(const char* message, Token* token);

// Get the line from source for error reporting
char* get_line_from_source(const char* source, int line);

// Print the error location with a caret pointing to the error
void print_error_location(const char* line, int column);

#endif // ERROR_H
