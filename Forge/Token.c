#include "Token.h"

void addToken(pTokenArray* ptoken_array, TokenType token_type, const char* lexeme)
{
    if ((*ptoken_array)->size == (*ptoken_array)->count) {
        resizeTokenArr(ptoken_array);
    }
    (*ptoken_array)->tokens[(*ptoken_array)->count].type = token_type;
    // copy the lexeme into the tokens[token_count]
    strncpy((*ptoken_array)->tokens[(*ptoken_array)->count].lexeme, lexeme, sizeof((*ptoken_array)->tokens[(*ptoken_array)->count].lexeme) - 1);
    // null terminate
    (*ptoken_array)->tokens[(*ptoken_array)->count].lexeme[sizeof((*ptoken_array)->tokens[(*ptoken_array)->count].lexeme) - 1] = '\0';
    // add one to the token count
    (*ptoken_array)->count++;
}


void printTokens(pTokenArray ptoken_array) {
    // Print tokens
    printf("Tokens:\n");
    for (int i = 0; i < (ptoken_array)->count; i++) {
        printf("Type: %d, Lexeme: %s\n", (ptoken_array)->tokens[i].type, (ptoken_array)->tokens[i].lexeme);
    }
}


Token* createTokens(int size) {
    Token* tokens = (Token*)malloc(size * sizeof(Token));
    if (tokens == NULL) {
        printf("Memory allocation for tokens failed\n");
        return NULL;
    }
    return tokens;
}

void resizeTokenArr(pTokenArray* ptoken_array) {
    (*ptoken_array)->size = (*ptoken_array)->size * 2;
    Token* temp = (Token*)realloc((*ptoken_array)->tokens, (*ptoken_array)->size * sizeof(Token));
    if (temp == NULL) {
        printf("Memory allocation for tokens failed\n");
        return;
    }
    printf("Token array resize to: %d\n", (*ptoken_array)->size);
    (*ptoken_array)->tokens = temp;
}

void initTokenArray(pTokenArray* ptoken_array) {
    *ptoken_array = (pTokenArray)malloc(sizeof(TokenArray));
    if (*ptoken_array == NULL) {
        printf("Memory allocation for TokenArray failed\n");
        return;
    }

    (*ptoken_array)->count = 0;
    (*ptoken_array)->size = INIT_TOKEN_COUNT;
    (*ptoken_array)->tokens = createTokens((*ptoken_array)->size);
}

void freeTokenArray(pTokenArray* ptoken_array) {
    free((*ptoken_array)->tokens);
    free(*ptoken_array);
    ptoken_array = NULL;
}