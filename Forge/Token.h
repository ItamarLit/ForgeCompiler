#ifndef TOKEN_H // include gaurds
#define TOKEN_H

#include <stdio.h>

#define INIT_TOKEN_COUNT 3
#define MAX_LEXEME_LEN 64

typedef enum {
    ERROR = -1,
    IDENTIFIER = 0,
    INT_LITERAL,
    FLOAT_LITERAL,
    OPERATOR,
    STRING_LITERAL,
    THEN,
    FORGE,
    MOLD,
    MEET,
    FOR,
    WHILE,
    IN,
    INT,
    FLOAT,
    STRING,
    BOOL,
    RETURN,
    VOID,
    DOUBLE_OPERATOR,
    OPEN_PAREN,
    CLOSED_PAREN,
    OPEN_BRACE,
    CLOSE_BRACE,
    SEMI_COLON,
    COMMA,
    SMALLER_THAN,
    SMALLER_EQUAL,
    LARGER_THAN,
    LARGER_EQUAL,
    PLUS,
    MINUS,
    DIV,
    MUL,
    EQUAL,
    PLUS_EQUAL,
    MINUS_EQUAL,
    DIV_EQUAL,
    MUL_EQUAL,
    EQUAL_EQUAL,
    FUNC_RET_TYPE,
    COLON,


}TokenType;

typedef struct {
    char lexeme[MAX_LEXEME_LEN];
    TokenType type;
}Token;

typedef struct {
    Token* tokens;
    int size;
    int count;
}TokenArray, * pTokenArray;

// function to add a token to an arr of tokens 
void addToken(pTokenArray* ptoken_array, TokenType token_type, const char* lexeme);
// func that prints the tokens
void printTokens(pTokenArray ptoken_array);
// func that creates a token arr in the heap
Token* createTokens(int size);
// func that will enlarge the token arr
void resizeTokenArr(pTokenArray* ptoken_array);
// func to set a new TokenArray
void initTokenArray(pTokenArray* ptoken_array);
// free the token array struct
void freeTokenArray(pTokenArray* ptoken_array);

#endif