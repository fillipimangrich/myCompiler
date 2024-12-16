#ifndef TAC_H
#define TAC_H

#include "types.h"

typedef enum {
    TAC_ADD,
    TAC_SUB,
    TAC_MUL,
    TAC_DIV,
    TAC_MOD,
    TAC_ASSIGN,
    TAC_LABEL,
    TAC_JUMP,
    TAC_JUMPZ,    // Jump if zero
    TAC_JUMPNZ,   // Jump if not zero
    TAC_LT,       // Less than
    TAC_GT,       // Greater than
    TAC_LEQ,      // Less than or equal
    TAC_GEQ,      // Greater than or equal
    TAC_EQ,       // Equal
    TAC_NEQ,      // Not equal
    TAC_AND,
    TAC_OR,
    TAC_NOT,
    TAC_PARAM,    // Function parameter
    TAC_CALL,     // Function call
    TAC_RETURN,   // Return statement
    TAC_LOAD_INT,
    TAC_LOAD_FLOAT,
    TAC_LOAD_STRING
} TACOpType;

typedef struct TACOp {
    char* result;     // Destination operand
    char* arg1;       // First source operand
    char* arg2;       // Second source operand
    TACOpType op;     // Operation type
    struct TACOp* next;
} TACOp;

typedef struct TACContext {
    TACOp* first;
    TACOp* last;
    int temp_count;
    int label_count;
} TACContext;

extern TACContext tac_ctx;

// Initialize TAC context
void init_tac();

// Generate new temporary variable name
char* new_temp();

// Generate new label name
char* new_label();

// Create a new TAC operation
TACOp* create_tac_op(TACOpType op, char* result, char* arg1, char* arg2);

// Add TAC operation to the context
void add_tac_op(TACOp* op);

// Generate TAC from AST
void generate_tac(ASTNode* node);

// Print TAC operations
void print_tac();

void save_tac_to_file(const char* filename);

#endif // TAC_H