#include "lexer.h"
#include "Token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma warning (disable:4996)


// hash func for the FSM hashMap
unsigned long hashFunc(void* key, int mapSize) {
    FsmKey* fsmKey = (FsmKey*)key;
    int asciiValue = fsmKey->currentChar;
    // get 2 prime numbers to distribute the hash values more
    int prime1 = 31;
    int prime2 = 37;
    // return the hash for the state char pair
    return ((asciiValue * prime1 + fsmKey->currentState * prime2) % mapSize);
}

int fsmEqualFunc(void* a, void* b) {
    FsmKey* k1 = (FsmKey*)a;
    FsmKey* k2 = (FsmKey*)b;
    return (k1->currentState == k2->currentState) &&
        (k1->currentChar == k2->currentChar);
}

// func that creates a new state in the hashmap
void putState(int state, char ch, int nextState, HashMap* map) {
    // allocate key
    FsmKey* key = malloc(sizeof(FsmKey));
    key->currentState = state;
    key->currentChar = ch;
    // allocate the value
    int* valPtr = malloc(sizeof(int));
    *valPtr = nextState;
    // insert the key - val pair
    insertNewValue(key, valPtr, map);
}

// Initialize the hash map for the FSM transitions
void init_state_machine(HashMap** map) {
    // init the hash map with the starting size
    *map = initHashMap(INITAL_HASHMAP_SIZE, hashFunc, fsmEqualFunc);

    // Transitions for START_STATE
    putState(START_STATE, '"', STRING_LITERAL_STATE, *map);
    // skip spaces
    putState(START_STATE, ' ', START_STATE, *map);
    putState(START_STATE, '\n', START_STATE, *map);
    putState(START_STATE, '\t', START_STATE, *map);
    putState(START_STATE, '\r', START_STATE, *map);


    for (char c = 'a'; c <= 'z'; c++) {
        // for start state
        putState(START_STATE, c, IDENTIFIER_STATE, *map);
        // for identifier state
        putState(IDENTIFIER_STATE, c, IDENTIFIER_STATE, *map);
        putState(STRING_LITERAL_STATE, c, STRING_LITERAL_STATE, *map);


    }
    for (char c = 'A'; c <= 'Z'; c++) {
        putState(START_STATE, c, IDENTIFIER_STATE, *map);
        // for identifier state
        putState(IDENTIFIER_STATE, c, IDENTIFIER_STATE, *map);
        putState(STRING_LITERAL_STATE, c, STRING_LITERAL_STATE, *map);

    }
    for (char c = '0'; c <= '9'; c++) {
        // for start state
        putState(START_STATE, c, INT_LITERAL_STATE, *map);
        putState(STRING_LITERAL_STATE, c, STRING_LITERAL_STATE, *map);
        // Transaction for the float state
        putState(INT_POINT_STATE, c, FLOAT_STATE, *map);
        putState(FLOAT_STATE, c, FLOAT_STATE, *map);
        // for identifier state
        putState(IDENTIFIER_STATE, c, IDENTIFIER_STATE, *map);
        // for int state
        putState(INT_LITERAL_STATE, c, INT_LITERAL_STATE, *map);


    }

    // Transitions for operators
    putState(START_STATE, '=', EQUAL_STATE, *map);
    putState(START_STATE, '+', PLUS_STATE, *map);
    putState(START_STATE, '-', MINUS_STATE, *map);
    putState(START_STATE, '*', MUL_STATE, *map);
    putState(START_STATE, '/', DIV_STATE, *map);
    // Transition for double operator
    putState(EQUAL_STATE, '=', EQUAL_EQUAL_STATE, *map);
    putState(PLUS_STATE, '=', PLUS_EQUAL_STATE, *map);
    putState(MINUS_STATE, '=', MINUS_EQUAL_STATE, *map);
    putState(MUL_STATE, '=', MUL_EQUAL_STATE, *map);
    putState(DIV_STATE, '=', DIV_EQUAL_STATE, *map);

    putState(STRING_LITERAL_STATE, ' ', STRING_LITERAL_STATE, *map);

    // Transition for ending string literal
    putState(STRING_LITERAL_STATE, '"', TERMINATED_STRING_LITERAL_STATE, *map);



    // Transitions for the paren
    putState(START_STATE, '(', OPEN_PAREN_STATE, *map);
    putState(START_STATE, ')', CLOSED_PAREN_STATE, *map);

    // Transitions for the braces
    putState(START_STATE, '{', OPEN_BRACE_STATE, *map);
    putState(START_STATE, '}', CLOSE_BRACE_STATE, *map);

    // Trasition for the semi colon, comma, and colon
    putState(START_STATE, ';', SEMI_COLON_STATE, *map);
    putState(START_STATE, ',', COMMA_STATE, *map);
    putState(START_STATE, ':', COLON_STATE, *map);

    // Transaction for the > < state
    putState(START_STATE, '<', SMALLER_THAN_STATE, *map);
    putState(START_STATE, '>', LARGER_THAN_STATE, *map);
    // Transaction for the <= >= states
    putState(SMALLER_THAN_STATE, '=', SMALLER_EQUAL_STATE, *map);
    putState(LARGER_THAN_STATE, '=', LARGER_EQUAL_STATE, *map);

    // Transaction for the int to float state
    putState(INT_LITERAL_STATE, '.', INT_POINT_STATE, *map);

    // Transaction for the func ret type arrow =>
    putState(EQUAL_STATE, '>', FUNC_RET_TYPE_STATE, *map);
}






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
        [FLOAT_STATE] = FLOAT_LITERAL,
        [STRING_LITERAL_STATE] = ERROR,
        [INT_POINT_STATE] = ERROR,
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
    };

    if (state >= VALID_STATE && state < sizeof(state_to_token_type_map) / sizeof(state_to_token_type_map[0])) {
        TokenType type = state_to_token_type_map[state];
        if (type == IDENTIFIER) {
            type = identifyKeyowrd(value);
        }
        return type;
    }

    return -1;
}

TokenType identifyKeyowrd(char* lexeme) {
    static const KeywordMap keywordMap[] = {
        {"mold", MOLD},
        {"while", WHILE},
        {"for", FOR},
        {"then", THEN},
        {"meet", MEET},
        {"forge", FORGE},
        {"in", IN},
        {"int", INT},
        {"float", FLOAT},
        {"string", STRING},
        {"bool", BOOL},
        {"return", RETURN},
        {"void", VOID},
        {NULL, ERROR}  // ending
    };
    for (int i = 0; keywordMap[i].keyword != NULL; i++) {
        if (strcmp(keywordMap[i].keyword, lexeme) == 0) {
            return keywordMap[i].type;
        }
    }
    return IDENTIFIER;
}


void addAndResetLexer(pTokenArray ptoken_array, State* current_state, State* last_accepting_state, char* current_lexeme, int* lexeme_index, State token_state)
{
    if (*lexeme_index != 0) {
        current_lexeme[*lexeme_index] = '\0';
        // add the token
        addToken(&ptoken_array, state_to_token_type(token_state, current_lexeme), current_lexeme);
        // reset
        *current_state = START_STATE;
        *last_accepting_state = START_STATE;
        *lexeme_index = 0;
    }
}

void handleErrorToken(HashMap* map, char** input, char* current_lexeme, int* lexeme_index, pTokenArray ptoken_array, State* current_state, State* last_accepting_state)
{
    FsmKey* temp = (FsmKey*)malloc(sizeof(FsmKey));
    temp->currentChar = *input;
    temp->currentState = START_STATE;
    int* statePtr = (int*)getHashMapValue(temp, map);
    if (statePtr) {
        *current_state = *statePtr;
    }
    else {
        *current_state = -1;
    }
    // read all the invalid tokens
    while (*current_state == -1 && **input != '\0')
    {
        current_lexeme[(*lexeme_index)++] = **input;
        (*input)++;
        temp->currentChar = **input;
        temp->currentState = START_STATE;
        int* statePtr = (int*)getHashMapValue(temp, map);
        if (statePtr) {
            *current_state = *statePtr;
        }
        else {
            *current_state = -1;
        }
    }
    // finalize as ERROR token and reset
    addAndResetLexer(ptoken_array, current_state, last_accepting_state, current_lexeme, lexeme_index, ERROR_TOKEN_STATE);
    free(temp);
}


void lex(HashMap* map, char* input, pTokenArray ptoken_array) {
    State current_state = START_STATE;
    State last_accepting_state = -1;
    char current_lexeme[MAX_LEXEME_LEN];
    int lexeme_index = 0;
    FsmKey* temp = (FsmKey*)malloc(sizeof(FsmKey));
    // go over the whole input
    while (*input != '\0') {
        // get the next state from the hash map o(1)
        temp->currentChar = *input;
        temp->currentState = current_state;
        int* statePtr = (int*)getHashMapValue(temp, map);
        if (statePtr) {
            current_state = *statePtr;
        }
        else {
            current_state = -1; 
        }
        // check if there is a valid transition and if so check that it isnt a whitespace skip (Start_State)
        if (current_state != -1 && current_state != START_STATE)
        {
            // add the current char
            current_lexeme[lexeme_index++] = *input;
            input++;
            // save current state
            last_accepting_state = current_state;
        }
        else
        {
            // check if the token that is invalid is a whitespace and needs to be skipped
            if (current_state == START_STATE) {
                input++;
            }
            // check that the token is acceptable
            else if (last_accepting_state > START_STATE)
            {
                addAndResetLexer(ptoken_array, &current_state, &last_accepting_state, current_lexeme, &lexeme_index, last_accepting_state);

            }
            // this else will happen if there are any unrecognized chars
            else {
                handleErrorToken(map, &input, current_lexeme, &lexeme_index, ptoken_array, &current_state, &last_accepting_state);
            }
        }
    }
    // if we finish the input we add the last token
    addAndResetLexer(ptoken_array, &current_state, &last_accepting_state, current_lexeme, &lexeme_index, last_accepting_state);
    free(temp);
}