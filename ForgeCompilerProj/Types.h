#ifndef TYPES_H
#define TYPES_H

// enum for types
typedef enum {
    TYPE_INT,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_VOID,
    TYPE_ERROR,  // for error handling
    TYPE_UNDEFINED // for undefined variables
} Type;

Type convertStringType(char* stringType);
const char* convertTypeToString(Type type);
#endif
