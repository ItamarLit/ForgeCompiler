#ifndef LEXER_H 
#define LEXER_H

#include "HashMap.h"
#include "Token.h"

#define VALID_STATE 2

// states in FSM
typedef enum states {
    // NOT accepting
    START_STATE,
    ERROR_TOKEN_STATE,
    // accepting
    IDENTIFIER_STATE,
    INT_LITERAL_STATE,
    PLUS_STATE,
    MINUS_STATE,
    DIV_STATE,
    MUL_STATE,
    EQUAL_STATE,
    PLUS_EQUAL_STATE,
    MINUS_EQUAL_STATE,
    DIV_EQUAL_STATE,
    MUL_EQUAL_STATE,
    EQUAL_EQUAL_STATE,
    TERMINATED_STRING_LITERAL_STATE,
    OPEN_PAREN_STATE,
    CLOSED_PAREN_STATE,
    OPEN_BRACE_STATE,
    CLOSE_BRACE_STATE,
    SEMI_COLON_STATE,
    COMMA_STATE,
    SMALLER_THAN_STATE,
    LARGER_THAN_STATE,
    STRING_LITERAL_STATE,
    SMALLER_EQUAL_STATE,
    LARGER_EQUAL_STATE,
    FUNC_RET_TYPE_STATE,
    COLON_STATE,
    START_AND_STATE,
    AND_STATE,
    START_OR_STATE,
    OR_STATE,
    NOT_STATE,
    NOT_EQUAL_STATE,
} State;

typedef struct {
    const char* keyword;
    TokenType type;
} KeywordMap;

// struct for the hashmap key
typedef struct FsmKey {
    int currentState;
    char currentChar;
} FsmKey;


// func that inits the state machine
void init_state_machine(HashMap** map);
// func that preforms the lexing
void lex(HashMap* map, char* input, pTokenArray ptoken_array);
// func that will identifiy the keywords
TokenType identifyKeyowrd(char* lexeme);
// func that will identify the token type
TokenType state_to_token_type(State state, char* value);

#endif