#include "Token.h"
#include "ErrorHandler.h"
#include <string.h>
#pragma warning (disable:4996)

/// <summary>
/// This func is used to add a token to the token array
/// </summary>
/// <param name="ptoken_array"></param>
/// <param name="token_type"></param>
/// <param name="lexeme"></param>
void add_token(pTokenArray* ptoken_array, TokenType token_type, const char* lexeme, int row, int col)
{
    if ((*ptoken_array)->size == (*ptoken_array)->count) {
        resize_token_arr(ptoken_array);
    }
    (*ptoken_array)->tokens[(*ptoken_array)->count] = (Token*)malloc(sizeof(Token));
    if ((*ptoken_array)->tokens[(*ptoken_array)->count] == NULL) {
        output_error(GENERAL, "Memory allocation for token failed for token\n");
        return;
    }
    // set the type and value
    (*ptoken_array)->tokens[(*ptoken_array)->count]->type = token_type;
    strncpy(
        (*ptoken_array)->tokens[(*ptoken_array)->count]->lexeme,
        lexeme,
        sizeof((*ptoken_array)->tokens[(*ptoken_array)->count]->lexeme) - 1
    );
    (*ptoken_array)->tokens[(*ptoken_array)->count]->tokenRow = row;
    (*ptoken_array)->tokens[(*ptoken_array)->count]->tokenCol = col - strlen(lexeme);
    (*ptoken_array)->tokens[(*ptoken_array)->count]->lexeme[sizeof((*ptoken_array)->tokens[(*ptoken_array)->count]->lexeme) - 1] = '\0';
    // add one to the token count
    (*ptoken_array)->count++;
}

/// <summary>
/// This func is used to print the token array
/// </summary>
/// <param name="ptoken_array"></param>
void print_tokens(pTokenArray ptoken_array) {
    // Print tokens
    printf("Tokens:\n");
    for (int i = 0; i < (ptoken_array)->count; i++) {
        printf("Type: %d, Lexeme: %s, Row: %d, Col: %d\n", (ptoken_array)->tokens[i]->type, (ptoken_array)->tokens[i]->lexeme, (ptoken_array)->tokens[i]->tokenRow, (ptoken_array)->tokens[i]->tokenCol);
    }
}

/// <summary>
/// This func creates an empty token array based on a given size
/// </summary>
/// <param name="size"></param>
/// <returns>Pointer to the allocated array</returns>
Token** create_tokens(int size) {
    Token** tokens = (Token**)malloc(size * sizeof(Token*));
    if (tokens == NULL) {
        output_error(GENERAL, "Memory allocation for tokens failed for token array\n");
        return NULL;
    }
    return tokens;
}

/// <summary>
/// This func resizes the token array and doubles its size
/// </summary>
/// <param name="ptoken_array"></param>
void resize_token_arr(pTokenArray* ptoken_array) {
    (*ptoken_array)->size = (*ptoken_array)->size * 2;
    Token** temp = (Token**)realloc((*ptoken_array)->tokens, (*ptoken_array)->size * sizeof(Token*));
    if (temp == NULL) {
        output_error(GENERAL, "Memory allocation for tokens failed for token array\n");
        return;
    }
    (*ptoken_array)->tokens = temp;
}

/// <summary>
/// This func inits the token array struct
/// </summary>
/// <param name="ptoken_array"></param>
void init_token_array(pTokenArray* ptoken_array) {
    *ptoken_array = (pTokenArray)malloc(sizeof(TokenArray));
    if (*ptoken_array == NULL) {
        output_error(GENERAL, "Memory allocation for TokenArray failed\n");
        return;
    }
    (*ptoken_array)->count = 0;
    (*ptoken_array)->size = INIT_TOKEN_COUNT;
    (*ptoken_array)->tokens = create_tokens((*ptoken_array)->size);
}

/// <summary>
/// This func is used to free the token array
/// </summary>
/// <param name="ptoken_array"></param>
void free_token_array(pTokenArray* ptoken_array) {
    for (int i = 0; i < (*ptoken_array)->count; i++) {
        // Free each Token

        free((*ptoken_array)->tokens[i]);
    }
    // Free the array of pointers
    free((*ptoken_array)->tokens);
    // Free the TokenArray struct
    free(*ptoken_array);
    *ptoken_array = NULL;
}