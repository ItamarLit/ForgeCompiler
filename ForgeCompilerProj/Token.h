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
    OPERATOR = 2,
    STRING_LITERAL = 3,
    THEN = 4,
    FORGE = 5,
    MOLD = 6,
    MEET = 7,
    FOR = 8,
    WHILE = 9,
    IN = 10,
    INT = 11,
    STRING = 12,
    BOOL = 13,
    RETURN = 14,
    VOID = 15,
    DOUBLE_OPERATOR = 16,
    OPEN_PAREN = 17,
    CLOSED_PAREN = 18,
    OPEN_BRACE = 19,
    CLOSE_BRACE = 20,
    SEMI_COLON = 21,
    COMMA = 22,
    SMALLER_THAN = 23,
    SMALLER_EQUAL = 24,
    LARGER_THAN = 25,
    LARGER_EQUAL = 26,
    PLUS = 27,
    MINUS = 28,
    DIV = 29,
    MUL = 30,
    EQUAL = 31,
    PLUS_EQUAL = 32,
    MINUS_EQUAL = 33,
    DIV_EQUAL = 34,
    MUL_EQUAL = 35,
    EQUAL_EQUAL = 36,
    FUNC_RET_TYPE = 37,
    COLON = 38,
    REMOLD = 39,
    IF = 40,
    ELSE = 41,
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