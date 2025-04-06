#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

// Error types in the compiler
typedef enum  {
    LEXICAL,
    SYNTAX,
    SEMANTIC,
} ErrorType;

void output_error(ErrorType type, char* fmt, ...);

#endif

