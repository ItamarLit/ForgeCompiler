#include "lexer.h"
#include "Token.h"
#include "ErrorHandler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma warning (disable:4996)



static void print_key(void* key)
{
    FsmKey* fkey = (FsmKey*)key;
    printf("[Key: current_state: %d, char: '%c' ", fkey->currentState, fkey->currentChar);
}


static void print_value(void* value) {
    int* fvalue = (int*)value;
    printf("Value: next_state: %d] ", *fvalue);
}



/// <summary>
/// This func is the hash func for the FSM hashmap
/// </summary>
/// <param name="key"></param>
/// <param name="mapSize"></param>
/// <returns>Returns a hashcode based on a given key</returns>
static unsigned long hash_func(void* key, int mapSize) {
    FsmKey* fsmKey = (FsmKey*)key;
    int asciiValue = fsmKey->currentChar;
    // get 2 prime numbers to distribute the hash values more
    int prime1 = 31;
    int prime2 = 37;
    // return the hash for the state char pair
    return ((asciiValue * prime1 + fsmKey->currentState * prime2) % mapSize);
}

/// <summary>
/// This func is the equals func for the FSM hashmap
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <returns></returns>
static int equal_func(void* a, void* b) {
    FsmKey* k1 = (FsmKey*)a;
    FsmKey* k2 = (FsmKey*)b;
    return (k1->currentState == k2->currentState) &&
        (k1->currentChar == k2->currentChar);
}

/// <summary>
/// This func is used to create a new state in the hashmap, it handles all the mallocing
/// </summary>
/// <param name="state"></param>
/// <param name="ch"></param>
/// <param name="nextState"></param>
/// <param name="map"></param>
static void put_state(int state, char ch, int nextState, HashMap* map) {
    // allocate key
    FsmKey* key = malloc(sizeof(FsmKey));
    if (!key) {
        fprintf(stderr, "Unable to malloc key for lexer FSM\n");
        return;
    }
    key->currentState = state;
    key->currentChar = ch;
    // allocate the value
    int* valPtr = malloc(sizeof(int));
    if (!valPtr)
    {
        fprintf(stderr, "Unable to malloc value for lexer FSM\n");
        return;
    }
    *valPtr = nextState;
    // insert the key - val pair
    insert_new_value(key, valPtr, map);
}

/// <summary>
/// This func inits the state machine, it creates the hashmap and fills it with data
/// </summary>
/// <param name="map"></param>
void init_state_machine(HashMap** map) {
    // init the hash map with the starting size and two funcs
    *map = init_hashmap(INITAL_HASHMAP_SIZE, hash_func, equal_func, print_key, print_value, free, free);

    // Transitions for START_STATE
    put_state(START_STATE, '"', STRING_LITERAL_STATE, *map);
    // skip spaces
    put_state(START_STATE, ' ', START_STATE, *map);
    put_state(START_STATE, '\n', START_STATE, *map);
    put_state(START_STATE, '\t', START_STATE, *map);
    put_state(START_STATE, '\r', START_STATE, *map);
    put_state(COMMENT_STATE,' ', COMMENT_STATE, *map);
    put_state(COMMENT_STATE,'"', COMMENT_STATE, *map);
    put_state(COMMENT_STATE, '\t', COMMENT_STATE, *map);


    for (char c = 'a'; c <= 'z'; c++) {
        // for start state
        put_state(START_STATE, c, IDENTIFIER_STATE, *map);
        // for identifier state
        put_state(IDENTIFIER_STATE, c, IDENTIFIER_STATE, *map);
        put_state(STRING_LITERAL_STATE, c, STRING_LITERAL_STATE, *map);
        put_state(COMMENT_STATE, c, COMMENT_STATE, *map);


    }
    for (char c = 'A'; c <= 'Z'; c++) {
        put_state(START_STATE, c, IDENTIFIER_STATE, *map);
        // for identifier state
        put_state(IDENTIFIER_STATE, c, IDENTIFIER_STATE, *map);
        put_state(STRING_LITERAL_STATE, c, STRING_LITERAL_STATE, *map);
        put_state(COMMENT_STATE, c, COMMENT_STATE, *map);

    }
    for (char c = '0'; c <= '9'; c++) {
        // for start state
        put_state(START_STATE, c, INT_LITERAL_STATE, *map);
        put_state(STRING_LITERAL_STATE, c, STRING_LITERAL_STATE, *map);
        // for identifier state
        put_state(IDENTIFIER_STATE, c, IDENTIFIER_STATE, *map);
        // for int state
        put_state(INT_LITERAL_STATE, c, INT_LITERAL_STATE, *map);
        put_state(COMMENT_STATE, c, COMMENT_STATE, *map);

    }

    // Transitions for operators
    put_state(START_STATE, '=', EQUAL_STATE, *map);
    put_state(COMMENT_STATE, '=', COMMENT_STATE, *map);
    put_state(START_STATE, '+', PLUS_STATE, *map);
    put_state(COMMENT_STATE, '+', COMMENT_STATE, *map);
    put_state(START_STATE, '-', MINUS_STATE, *map);
    put_state(COMMENT_STATE, '-', COMMENT_STATE, *map);
    put_state(START_STATE, '*', MUL_STATE, *map);
    put_state(COMMENT_STATE, '*', COMMENT_STATE, *map);
    put_state(START_STATE, '/', DIV_STATE, *map);
    put_state(COMMENT_STATE, '/', COMMENT_STATE, *map);
    put_state(START_STATE, '!', NOT_STATE, *map);
    put_state(COMMENT_STATE, '!', COMMENT_STATE, *map);

    // Transition for double operator
    put_state(EQUAL_STATE, '=', EQUAL_EQUAL_STATE, *map);
    put_state(PLUS_STATE, '=', PLUS_EQUAL_STATE, *map);
    put_state(MINUS_STATE, '=', MINUS_EQUAL_STATE, *map);
    put_state(MUL_STATE, '=', MUL_EQUAL_STATE, *map);
    put_state(DIV_STATE, '=', DIV_EQUAL_STATE, *map);

    put_state(STRING_LITERAL_STATE, ' ', STRING_LITERAL_STATE, *map);

    // Transition for ending string literal
    put_state(STRING_LITERAL_STATE, '"', TERMINATED_STRING_LITERAL_STATE, *map);

    // Transition for the not equal
    put_state(NOT_STATE, '=', NOT_EQUAL_STATE, *map);

    // Transitions for the paren
    put_state(START_STATE, '(', OPEN_PAREN_STATE, *map);
    put_state(COMMENT_STATE, '(', COMMENT_STATE, *map);

    put_state(START_STATE, ')', CLOSED_PAREN_STATE, *map);
    put_state(COMMENT_STATE, ')', COMMENT_STATE, *map);

    // Transitions for the braces
    put_state(START_STATE, '{', OPEN_BRACE_STATE, *map);
    put_state(COMMENT_STATE, '{', COMMENT_STATE, *map);

    put_state(START_STATE, '}', CLOSE_BRACE_STATE, *map);
    put_state(COMMENT_STATE, '}', COMMENT_STATE, *map);

    // Trasition for the semi colon, comma, and colon
    put_state(START_STATE, ';', SEMI_COLON_STATE, *map);
    put_state(COMMENT_STATE, ';', COMMENT_STATE, *map);
    put_state(START_STATE, ',', COMMA_STATE, *map);
    put_state(COMMENT_STATE, ',', COMMENT_STATE, *map);

    // Transaction for the > < state
    put_state(START_STATE, '<', SMALLER_THAN_STATE, *map);
    put_state(COMMENT_STATE, '<', COMMENT_STATE, *map);

    put_state(START_STATE, '>', LARGER_THAN_STATE, *map);
    put_state(COMMENT_STATE, '>', COMMENT_STATE, *map);
    // Transaction for the <= >= states
    put_state(SMALLER_THAN_STATE, '=', SMALLER_EQUAL_STATE, *map);
    put_state(LARGER_THAN_STATE, '=', LARGER_EQUAL_STATE, *map);

    // Transaction for the func ret type arrow =>
    put_state(EQUAL_STATE, '>', FUNC_RET_TYPE_STATE, *map);
    // Transactions for the start & state and start | state
    put_state(START_STATE, '&', START_AND_STATE, *map);
    put_state(COMMENT_STATE, '&', COMMENT_STATE, *map);
    put_state(START_STATE, '|', START_OR_STATE, *map);
    put_state(COMMENT_STATE, '|', COMMENT_STATE, *map);
    // Transactions for the && and || state
    // Transaction for the start & state and start | state
    put_state(START_AND_STATE, '&', AND_STATE, *map);
    put_state(START_OR_STATE, '|', OR_STATE, *map);
    //Transition for the comment state
    put_state(START_STATE, '#', COMMENT_START_STATE, *map);
    put_state(COMMENT_START_STATE, '#', COMMENT_STATE, *map);
    // Transition for finishing comment
    put_state(COMMENT_STATE, '\n', START_STATE, *map);


}

/// <summary>
/// This func is used to get a token type based on a state
/// </summary>
/// <param name="state"></param>
/// <param name="value"></param>
/// <returns>Returns the token type based on the state</returns>
TokenType state_to_token_type(State state, char* value) {
    static const TokenType state_to_token_type_map[] = {
        [ERROR_TOKEN_STATE] = ERROR,
        [IDENTIFIER_STATE] = IDENTIFIER,
        [INT_LITERAL_STATE] = INT_LITERAL,
        [TERMINATED_STRING_LITERAL_STATE] = STRING_LITERAL,
        [OPEN_PAREN_STATE] = OPEN_PAREN,
        [CLOSED_PAREN_STATE] = CLOSED_PAREN,
        [OPEN_BRACE_STATE] = OPEN_BRACE,
        [CLOSE_BRACE_STATE] = CLOSE_BRACE,
        [SEMI_COLON_STATE] = SEMI_COLON,
        [COMMA_STATE] = COMMA,
        [SMALLER_THAN_STATE] = SMALLER_THAN,
        [LARGER_THAN_STATE] = LARGER_THAN,
        [STRING_LITERAL_STATE] = ERROR,
        [PLUS_STATE] = PLUS,
        [PLUS_EQUAL_STATE] = PLUS_EQUAL,
        [MINUS_STATE] = MINUS,
        [MINUS_EQUAL_STATE] = MINUS_EQUAL,
        [DIV_STATE] = DIV,
        [DIV_EQUAL_STATE] = DIV_EQUAL,
        [MUL_STATE] = MUL,
        [MUL_EQUAL_STATE] = MUL_EQUAL,
        [EQUAL_STATE] = EQUAL,
        [EQUAL_EQUAL_STATE] = EQUAL_EQUAL,
        [SMALLER_EQUAL_STATE] = SMALLER_EQUAL,
        [LARGER_EQUAL_STATE] = LARGER_EQUAL,
        [FUNC_RET_TYPE_STATE] = FUNC_RET_TYPE,
        [COLON_STATE] = COLON,
        [AND_STATE] = AND,
        [OR_STATE] = OR,
        [NOT_EQUAL_STATE] = NOT_EQUAL,
    };
    // check if the token is an error token
    if (state >= VALID_STATE && state < sizeof(state_to_token_type_map) / sizeof(state_to_token_type_map[0])) {
        TokenType type = state_to_token_type_map[state];
        if (type == IDENTIFIER) {
            type = identify_keyowrd(value);
        }
        return type;
    }
    // invalid state = error
    return -1;
}

TokenType identify_keyowrd(char* lexeme) {
    static const KeywordMap keywordMap[] = {
        {"mold", MOLD},
        {"while", WHILE},
        {"for", FOR},
        {"then", THEN},
        {"meet", MEET},
        {"forge", FORGE},
        {"in", IN},
        {"int", INT},
        {"string", STRING},
        {"bool", BOOL},
        {"return", RETURN},
        {"void", VOID},
        {"remold", REMOLD},
        {"if", IF},
        {"else", ELSE},
        {"true", BOOL_LITERAL},
        {"false", BOOL_LITERAL},
        {"output", OUTPUT},
        {"input", INPUT},
        {NULL, ERROR}  // ending
    };
    for (int i = 0; keywordMap[i].keyword != NULL; i++) {
        if (strcmp(keywordMap[i].keyword, lexeme) == 0) {
            return keywordMap[i].type;
        }
    }
    return IDENTIFIER;
}

/// <summary>
/// This func adds a token to the token arraya nd resets the lexer
/// </summary>
/// <param name="ptoken_array"></param>
/// <param name="current_state"></param>
/// <param name="last_accepting_state"></param>
/// <param name="current_lexeme"></param>
/// <param name="lexeme_index"></param>
/// <param name="token_state"></param>
static void add_and_reset_lexer(pTokenArray ptoken_array, State* current_state, State* last_accepting_state, char* current_lexeme, int* lexeme_index, State token_state, int row, int* col)
{
    if (*lexeme_index != 0) {
        current_lexeme[*lexeme_index] = '\0';
        // add the token
        add_token(&ptoken_array, state_to_token_type(token_state, current_lexeme), current_lexeme, row, *col);
        // reset
        *current_state = START_STATE;
        *last_accepting_state = START_STATE;
        *lexeme_index = 0;
        (*col)++;
    }
}

/// <summary>
/// This func gets the next state based on the current state and input char
/// </summary>
/// <param name="map"></param>
/// <param name="currentState"></param>
/// <param name="inputChar"></param>
/// <returns>Returns the next state or -1 if no next state avalible</returns>
static int get_next_state(HashMap* map, int currentState, char inputChar)
{
    FsmKey temp;
    temp.currentState = currentState;
    temp.currentChar = inputChar;
    int* nextPtr = (int*)get_hashmap_value(&temp, map);
    return (nextPtr) ? *nextPtr : -1;
}


static void handle_lexeme_too_longError(char** input, char* current_lexeme, int* lexeme_index, pTokenArray ptoken_array, int* errorCount, int* row, int* col, State* current_state, State* last_accepting_state, HashMap* map)
{
    // Null terminate
    current_lexeme[*lexeme_index] = '\0';
    // show error
    output_error(LEXICAL, "Token: '%s' exceeds max len of %d at line %d\n", current_lexeme, MAX_LEXEME_LEN, row);
    (*errorCount)++;
    // skip until next valid start spot
    while (**input != '\0' && get_next_state(map, START_STATE, **input) != START_STATE)
    {
        (*input)++;
        (*col)++;
    }
    // reset
    *lexeme_index = 0;
    *current_state = START_STATE;
    *last_accepting_state = START_STATE;
}








/// <summary>
/// This func handles error tokens, it collects all the char of an error token, adds it to the array and resets the lexer
/// </summary>
/// <param name="map"></param>
/// <param name="input"></param>
/// <param name="current_lexeme"></param>
/// <param name="lexeme_index"></param>
/// <param name="ptoken_array"></param>
/// <param name="current_state"></param>
/// <param name="last_accepting_state"></param>
static void handle_error_token(HashMap* map, char** input, char* current_lexeme, int* lexeme_index, pTokenArray ptoken_array, State* current_state, State* last_accepting_state, int row, int* col, int* errorCount)
{   // flag
    int overflow = 0;
    // go over input while error
    while (**input != '\0' && (get_next_state(map, START_STATE, **input) == -1))
    {
        if (*lexeme_index < MAX_LEXEME_LEN - 1)
        {
            current_lexeme[(*lexeme_index)++] = **input;
        }
        else
        {
            overflow = 1;  
        }
        (*input)++;
        (*col)++;
    }
    current_lexeme[*lexeme_index] = '\0';
    // check if the ERROR token is also overflow
    if (overflow)
    {
        output_error(LEXICAL, "Token: '%s' exceeds max len of %d at line %d\n", current_lexeme, MAX_LEXEME_LEN, row);
        (*errorCount)++;
    }
    // regular error 
    output_error(LEXICAL, "Invalid token: %s encountered at line: %d", current_lexeme, row);
    (*errorCount)++;
    // add error token
    add_and_reset_lexer(ptoken_array, current_state, last_accepting_state, current_lexeme, lexeme_index, ERROR_TOKEN_STATE, row, col);
}

static void check_new_row(char input, int* row, int* col) 
{
    if (input == '\n') {
        // reset col counter on new row
        (*row)++;
        *col = 0;
    }
}

/// <summary>
/// This is the main lex func
/// </summary>
/// <param name="map"></param>
/// <param name="input"></param>
/// <param name="ptoken_array"></param>
void lex(HashMap* map, char* input, pTokenArray ptoken_array, int* errorCount) {
    State current_state = START_STATE;
    State last_accepting_state = -1;
    char current_lexeme[MAX_LEXEME_LEN];
    int lexeme_index = 0;
    int rowCounter = 0;
    int colCounter = 0;
    // go over the whole input
    while (*input != '\0') {
        // get the next state from the hash map o(1)
        current_state = get_next_state(map, current_state, *input);
        // check if there is a valid transition and if so check that it isnt a whitespace skip (Start_State)
        if (current_state != -1 && current_state != START_STATE  && current_state != COMMENT_STATE)
        {
            if (lexeme_index >= MAX_LEXEME_LEN - 1) {
                handle_lexeme_too_longError(&input, current_lexeme, &lexeme_index,ptoken_array, errorCount, &rowCounter, &colCounter, &current_state, &last_accepting_state, map);
            }
            else 
            {
                // add the current char
                current_lexeme[lexeme_index++] = *input;
                input++;
                colCounter++;
                // save current state
                last_accepting_state = current_state;
            }
        }
        else
        {
            // check if the token that is invalid is a whitespace and needs to be skipped or a commnet
            if (current_state == START_STATE || current_state == COMMENT_STATE) {
                input++;
                colCounter++;
                lexeme_index = 0;
            }
            // check that the token is acceptable
            else if (last_accepting_state > START_STATE)
            {
                add_and_reset_lexer(ptoken_array, &current_state, &last_accepting_state, current_lexeme, &lexeme_index, last_accepting_state, rowCounter, &colCounter);
            }
            // this else will happen if there are any unrecognized chars
            else {
                handle_error_token(map, &input, current_lexeme, &lexeme_index, ptoken_array, &current_state, &last_accepting_state, rowCounter, &colCounter, errorCount);
            }
            check_new_row(*input, &rowCounter, &colCounter);
        }
        
     }
    // if we finish the input we add the last token
    add_and_reset_lexer(ptoken_array, &current_state, &last_accepting_state, current_lexeme, &lexeme_index, last_accepting_state, rowCounter, &colCounter);
}