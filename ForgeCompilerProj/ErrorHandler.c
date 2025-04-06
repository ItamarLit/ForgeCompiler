#include "ErrorHandler.h"
#include <stdarg.h>
#include <stdio.h>

/// <summary>
/// This func will output an error and format it correctly based on its type
/// </summary>
/// <param name="type"></param>
/// <param name="fmt"></param>
/// <param name=""></param>
void output_error(ErrorType type, char* fmt, ...)
{
    const char* prefix;
    // get the correct prefix based on type
    switch (type) {
        case LEXICAL:   prefix = "[ Lexical Error ] "; break;
        case SYNTAX:    prefix = "[ Syntax Error ] ";  break;
        case SEMANTIC:  prefix = "[ Semantic Error ] "; break;
        default:        prefix = "[ Unknown Error ] "; break; 
    }
    // print the prefix
    fprintf(stderr, "%s", prefix);
    // print formated msg
    va_list args;
    va_start(args, fmt);
    // output to stderr
    vfprintf(stderr, fmt, args);
    va_end(args);
    // add newline
    fprintf(stderr, "\n");
}
