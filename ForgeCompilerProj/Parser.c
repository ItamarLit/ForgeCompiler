#pragma warning (disable:4996)
#include "Parser.h"
#include "FileReader.h"
#include "HashMap.h"
#include "Terminals.h"
#include "NonTerminals.h"
#include "ParseStack.h"
#include "GrammarArray.h"
#include "AST.h"
#include "ErrorHandler.h"
#include "Paths.h"
#include <string.h>
#include <stdio.h>  
#include <stdlib.h>

void free_key(void* key);
void get_data(char** dataArr, char* line, int isAction);
unsigned long hash_func(void* key, int map_size);
void fill_table(HashMap** map, char* filename, char** symbolArray, int symbolCount, int isAction, void (*PutRow)(int state, const char* symbol, const char* value, HashMap* map));
static int equal_func(void* a, void* b);
static void print_key(void* key);


/// <summary>
/// This func allocates a key for the hashmap and fills it
/// </summary>
/// <param name="state"></param>
/// <param name="symbol"></param>
/// <returns>Returns a pointer to the key</returns>
static MapKey* get_key(int state, char* symbol) {
    // allocate key
    MapKey* key = malloc(sizeof(MapKey));
    if (!key) {
        printf("Memory allocation failed\n");
        return NULL;
    }
    key->currentState = state;
    key->symbol = strdup(symbol);
    return key;
}

/// <summary>
/// This func puts a new state in the action hashmap
/// </summary>
/// <param name="state"></param>
/// <param name="terminal"></param>
/// <param name="action"></param>
/// <param name="map"></param>
static void put_state(int state, char* terminal, const char* action, HashMap* map) {
    // allocate key
    MapKey* key = get_key(state, terminal);
    // allocate the value
    char* val = strdup(action);
    // insert the key - val pair
    insert_new_value(key, val, map);
}

static void action_print_value(void* value) {
    char* fvalue = (char*)value;
    printf("Value: action: %s] ", fvalue);
}

/// <summary>
/// This func will setup the action table hashmap
/// </summary>
/// <param name="map"></param>
/// <param name="filename"></param>
void init_action_table(HashMap** map, char* filename)
{
    *map = init_hashmap(INITAL_HASHMAP_SIZE, hash_func, equal_func, print_key, action_print_value, free_key, free);
    fill_table(map, filename, Terminals, TerminalCount, 1, put_state);
}



static void goto_print_value(void* value) {
    char* fvalue = (char*)value;
    printf("Value: nextState: %s] ", fvalue);
}


/// <summary>
/// This func is used to init the goto table
/// </summary>
/// <param name="map"></param>
/// <param name="filename"></param>
void init_goto_table(HashMap** map, char* filename) {
    *map = init_hashmap(INITAL_HASHMAP_SIZE, hash_func, equal_func, print_key, goto_print_value, free_key, free);
    fill_table(map, filename, NonTerminals, NonTerminalCount, 0, put_state);
}

void free_key(void* key)
{
    MapKey* fkey = (MapKey*)key;
    free(fkey->symbol);
    free(fkey);
}

/// <summary>
/// This func will get the data from the csv file line and fill the dataArr, if there is no action / goto then the arr[i] is empty
/// </summary>
/// <param name="dataArr"></param>
/// <param name="line"></param>
/// <param name="isAction"></param>
void get_data(char** dataArr, char* line, int isAction)
{
    int maxData = isAction ? TerminalCount : NonTerminalCount;
    int count = 0;
    char* start = line;
    char* end = NULL;

    while (count < maxData)
    {
        // find the next ,
        end = strchr(start, ',');
        if (end == NULL)
        {
            // last token
            dataArr[count] = strdup(start);
            count++;
        }
        else
        {
            // get the token data
            int length = (int)(end - start);
            char* token = (char*)malloc(length + 1);
            if (!token) {
                printf("Memory allocation failed\n");
                return;
            }
            memcpy(token, start, length);
            token[length] = '\0';
            dataArr[count] = token;
            count++;
            // move the start
            start = end + 1;
        }
    }
    // if the dataArr isnt filled yet, fill with empty strings
    while (count < maxData)
    {
        dataArr[count++] = strdup("");
    }
}


/// <summary>
/// This is the hash func for the action table
/// </summary>
/// <param name="key"></param>
/// <param name="map_size"></param>
/// <returns></returns>
unsigned long hash_func(void* key, int map_size) {
    MapKey* fkey = (MapKey*)key;
    unsigned long terminalHash = djb2Hash(fkey->symbol);
    return (terminalHash + (fkey->currentState * 31)) % map_size;
}


/// <summary>
/// This func is used to fill the Action and Goto tables
/// </summary>
/// <param name="map"></param>
/// <param name="filename"></param>
/// <param name="symbolArray"></param>
/// <param name="symbolCount"></param>
/// <param name="isAction"></param>
/// <param name="PutRow"></param>
void fill_table(HashMap** map, char* filename,const char** symbolArray, int symbolCount, int isAction,
    void (*PutRow)(int state, const char* symbol, const char* value, HashMap* map))
{
    char* fileData = read_file(filename);
    char* saveptr1 = NULL;
    char* line = strtok_s(fileData, "\n", &saveptr1);
    int stateCount = 0;
    while (line != NULL) {
        // skip empty lines
        if (strlen(line) == 0) {
            line = strtok_s(NULL, "\n", &saveptr1);
            continue;
        }

        // get the line data
        char** tempData = (char**)malloc(symbolCount * sizeof(char*));
        if (!tempData) 
        {
            fprintf(stderr, "Unable to malloc memory for tempData array in fill_table\n");
            return;
        }
        char* lineCopy = strdup(line);
        get_data(tempData, lineCopy, isAction);
        free(lineCopy);
        for (int i = 0; i < symbolCount; i++) {
            if (strcmp(tempData[i], "") != 0) {
                PutRow(stateCount, symbolArray[i], tempData[i], *map);
            }
            // free the duped str
            free(tempData[i]);
        }
        // free the str array
        free(tempData);
        stateCount++;
        // move to the next line
        line = strtok_s(NULL, "\n", &saveptr1);
    }
    // free the buffer
    free(fileData);
}

static int equal_func(void* a, void* b) {
    MapKey* k1 = (MapKey*)a;
    MapKey* k2 = (MapKey*)b;
    return (k1->currentState == k2->currentState) &&
        (strcmp(k1->symbol, k2->symbol) == 0);
}

static void print_key(void* key)
{
    MapKey* fkey = (MapKey*)key;
    printf("[Key: current_state: %d, symbol: '%s' ", fkey->currentState, fkey->symbol);
}

static char* get_map_value(HashMap* map, int currentState, char* symbol, int isAction)
{
    MapKey temp;
    temp.currentState = currentState;
    temp.symbol = symbol;
    char* valPtr = (char*)get_hashmap_value(&temp, map);
    return (valPtr) ? strdup(valPtr) : NULL;
   
}

/// <summary>
/// Helper func to turn token type to terminals
/// </summary>
/// <param name="token"></param>
/// <returns>Returns the terminal of the token</returns>
static char* token_type_to_terminal(Token* token) {
    switch (token->type) {
    case IDENTIFIER: return "IDENTIFIER";
    case INT_LITERAL: return "INT_LITERAL";
    case STRING_LITERAL: return "STRING_LITERAL";
    default: return token->lexeme;
    }
}

/// <summary>
/// This is the shift func in the parser
/// </summary>
/// <param name="s"></param>
/// <param name="nextState"></param>
/// <param name="token"></param>
/// <param name="nodeLable"></param>
static void shift(Stack* s, int nextState, Token* token, const char* nodeLable)
{
    StackData tempData;
    tempData.node = create_AST_node(token, nodeLable);
    push(s, tempData, NODE);
    tempData.node = NULL;
    tempData.state = nextState;
    push(s, tempData, STATE);
}

/// <summary>
/// This is an error recovery func that runs in Panic mode
/// </summary>
/// <param name="tokenArray"></param>
/// <param name="currentIndex"></param>
/// <returns>Returns the next index to start parsing from</returns>
static int recover_from_error(pTokenArray tokenArray, int currentIndex) {
    // array of the possible ending tokens
    const char* endingTokens[] = { ";", "}", "$" };
    int endingTokensCount = 3;
    while (currentIndex < tokenArray->count) {
        char* token = tokenArray->tokens[currentIndex]->lexeme;
        // check if the token is an ending token
        for (int i = 0; i < endingTokensCount; i++) {
            if (strcmp(token, endingTokens[i]) == 0) {
                return currentIndex + 1;
            }
        }
        // keep looking for an ending token
        currentIndex++;
    }
    return currentIndex;
}

/// <summary>
/// This func handles all syntax errors
/// </summary>
/// <param name="errorCount"></param>
/// <param name="s"></param>
/// <param name="currentIndex"></param>
/// <param name="tokenArray"></param>
/// <param name="finishedParsing"></param>
static void handle_syntax_error(int* errorCount,Stack** s,int* currentIndex,pTokenArray tokenArray,int* finishedParsing)
{
    // add error count
    (*errorCount)++;
    Token* errorToken = tokenArray->tokens[*currentIndex];
    // check if finished parsing
    if (*currentIndex == tokenArray->count - 1) {
        *finishedParsing = 1;
        errorToken = tokenArray->tokens[*currentIndex - 1];
    }
    // show the error
    output_error(SYNTAX, "Syntax Error at token '%s' on line %d, col %d.\n", errorToken->lexeme, errorToken->tokenRow, errorToken->tokenCol);
    // recover
    *currentIndex = recover_from_error(tokenArray, *currentIndex);
    Token* t = tokenArray->tokens[*currentIndex];
    if (*currentIndex == tokenArray->count - 1) {
        *finishedParsing = 1;
    }
    // reset the stack
    free_stack(*s);
    *s = init_stack();
    StackData tempData;
    tempData.state = 0;
    push(*s, tempData, STATE);
}

/// <summary>
/// This func handles the reduce in the parsing
/// </summary>
/// <param name="ruleIndex"></param>
/// <param name="s"></param>
/// <param name="array"></param>
/// <param name="gotoTable"></param>
/// <param name="tokenArray"></param>
/// <param name="i"></param>
/// <param name="errorCount"></param>
/// <param name="finishedParsing"></param>
static void reduce(int ruleIndex, Stack** s, GrammarArray* array, HashMap* gotoTable, pTokenArray tokenArray, int* i, int* errorCount, int* finishedParsing)
{
    GrammarRule* rule = array->rules[ruleIndex];
    // get the amount of children in the new node
    int childrenCount = rule->rightWordCount;
    // create the newNode
    ASTNode* newNode = create_AST_node(NULL, rule->leftRule);
    // create the temp children array
    ASTNode** nodeArr = (ASTNode**)malloc(sizeof(ASTNode*) * childrenCount);
    if (!nodeArr)
    {
        fprintf(stderr, "Unable to malloc memory for nodeArr in reduce\n");
        return;
    }
    // pop from the stack
    for (int j = 0; j < rule->rightWordCount * 2; j++) {
        StackEntry* curData = pop(*s);
        // if the data is a state we can free it
        if (curData->type == NODE) {
            // data is a node save it
            nodeArr[j / 2] = (ASTNode*)curData->data.node;
        }
        free(curData);
    }
    // now the nodeArr is in reverse order
    for (int i = childrenCount - 1; i >= 0; i--) {
        add_child(nodeArr[i], newNode);
    }
    free(nodeArr);
    // get the goto state
    char* gotoState = get_map_value(gotoTable, (top(*s))->data.state, rule->leftRule, 0);
    if (gotoState == NULL) {
        handle_syntax_error(errorCount, s, i, tokenArray, finishedParsing);
        return;
    }
    // preform the reduce
    StackData tempData;
    tempData.node = newNode;
    push(*s, tempData, NODE);
    tempData.state = atoi(gotoState);
    push(*s, tempData, STATE);
    if (gotoState) free(gotoState);
}

static void free_parser_resources(GrammarArray* array, HashMap** actionTable, HashMap** gotoTable, Stack* s)
{
    // free all the resources used to parse the input
    free_grammar_array(array);
    free_hashmap(actionTable);
    free_hashmap(gotoTable);
    free_stack(s);
}


/// <summary>
/// This is the main parse function that checks if the given input is valid 
/// </summary>
/// <param name="tokenArray"></param>
ASTNode* parse(pTokenArray tokenArray, int* errorCount) 
{
    // add $ to the end of the tokenArray
    add_token(&tokenArray, FINISH_INPUT, "$", -1, -1);
    StackData tempData;
    // setup stack
    Stack* s = init_stack();
    // setup action and goto tables
    HashMap* actionTable = NULL;
    HashMap* gotoTable = NULL;
    init_action_table(&actionTable, ACTION_TABLE_FILE);
    init_goto_table(&gotoTable, GOTO_TABLE_FILE);
    // setup grammar array
    GrammarArray* array = init_grammar_array(GRAMMAR_FILE);
    // push initial state onto the stack
    tempData.state = 0;
    push(s, tempData, STATE);
    // main parse loop
    int i = 0;
    int finishedParsing = 0;
    ASTNode* root = NULL;
    while (i < tokenArray->count && !finishedParsing) {
        // get the current state
        StackEntry* topstack = top(s);
        int currentState = topstack->data.state;
        // get the token
        char* token = token_type_to_terminal(tokenArray->tokens[i]);
        Token* currentToken = tokenArray->tokens[i];
        char* action = get_map_value(actionTable, currentState, token, 1);
        if (action == NULL)
        {
            handle_syntax_error(errorCount,&s,&i,tokenArray,&finishedParsing);
        }
        else 
        {
            // check for shift action
            if (!finishedParsing && strncmp(action, "s", 1) == 0) {
                shift(s, atoi(action + 1), currentToken, token);
                // move to next token
                i++;
            }
            else if (!finishedParsing && strncmp(action, "r", 1) == 0) {
                reduce(atoi(action + 1), &s, array, gotoTable, tokenArray, &i, errorCount, &finishedParsing);
            }
            else {
                // free the top state
                free(pop(s));
                // get the final root node
                StackEntry* topEntry = pop(s);
                root = (ASTNode*)topEntry->data.node;
                free(topEntry);
                finishedParsing = 1;
            }
           
        }
        if(action) free(action);
    }
    // free all the used data structures
    free_parser_resources(array, &actionTable, &gotoTable, s);
    return root;
}

