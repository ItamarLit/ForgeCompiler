#ifndef TOKEN_H // include gaurds
#define TOKEN_H

#include <stdio.h>

#define INIT_TOKEN_COUNT 12
#define MAX_LEXEME_LEN 64

typedef enum {
    FINISH_INPUT = -2,
    ERROR = -1,
    IDENTIFIER = 0,
    INT_LITERAL = 1,
    FLOAT_LITERAL = 2,
    OPERATOR = 3,
    STRING_LITERAL = 4,
    THEN = 5,
    FORGE = 6,
    MOLD = 7,
    MEET = 8,
    FOR = 9,
    WHILE = 10,
    IN = 11,
    INT = 12,
    FLOAT = 13,
    STRING = 14,
    BOOL = 15,
    RETURN = 16,
    VOID = 17,
    DOUBLE_OPERATOR = 18,
    OPEN_PAREN = 19,
    CLOSED_PAREN = 20,
    OPEN_BRACE = 21,
    CLOSE_BRACE = 22,
    SEMI_COLON = 23,
    COMMA = 24,
    SMALLER_THAN = 25,
    SMALLER_EQUAL = 26,
    LARGER_THAN = 27,
    LARGER_EQUAL = 28,
    PLUS = 29,
    MINUS = 30,
    DIV = 31,
    MUL = 32,
    EQUAL = 33,
    PLUS_EQUAL = 34,
    MINUS_EQUAL = 35,
    DIV_EQUAL = 36,
    MUL_EQUAL = 37,
    EQUAL_EQUAL = 38,
    FUNC_RET_TYPE = 39,
    COLON = 40,
    REMOLD = 41,
    IF = 42,
    ELSE = 43,
}TokenType;

typedef struct {
    char lexeme[MAX_LEXEME_LEN];
    TokenType type;
    int tokenRow;
    int tokenCol;
}Token;

typedef struct {
    Token** tokens;
    int size;
    int count;
}TokenArray, * pTokenArray;

// function to add a token to an arr of tokens 
void addToken(pTokenArray* ptoken_array, TokenType token_type, const char* lexeme, int row, int col);
// func that prints the tokens
void printTokens(pTokenArray ptoken_array);
// func that creates a token arr in the heap
Token** createTokens(int size);
// func that will enlarge the token arr
void resizeTokenArr(pTokenArray* ptoken_array);
// func to set a new TokenArray
void initTokenArray(pTokenArray* ptoken_array);
// free the token array struct
void freeTokenArray(pTokenArray* ptoken_array);

#endif