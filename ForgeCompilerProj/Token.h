#ifndef TOKEN_H // include gaurds
#define TOKEN_H

#include <stdio.h>

#define INIT_TOKEN_COUNT 12
#define MAX_LEXEME_LEN 64

typedef enum {
    FINISH_INPUT = -2,
    ERROR = -1,
    IDENTIFIER = 0,
    INT_LITERAL,
    STRING_LITERAL,
    BOOL_LITERAL,
    OPERATOR,
    THEN,
    FORGE,
    MOLD,
    MEET,
    FOR,
    WHILE,
    IN,
    INT,
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
    REMOLD,
    IF,
    ELSE,
    AND,
    OR,
    OUTPUT,
    INPUT,
    NOT_EQUAL,
    OUTPUT_INLINE,
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
void add_token(pTokenArray* ptoken_array, TokenType token_type, const char* lexeme, int row, int col);
// func that prints the tokens
void print_tokens(pTokenArray ptoken_array);
// func that creates a token arr in the heap
Token** create_tokens(int size);
// func that will enlarge the token arr
void resize_token_arr(pTokenArray* ptoken_array);
// func to set a new TokenArray
void init_token_array(pTokenArray* ptoken_array);
// free the token array struct
void free_token_array(pTokenArray* ptoken_array);

#endif