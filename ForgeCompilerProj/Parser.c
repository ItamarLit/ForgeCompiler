#pragma warning (disable:4996)
#include "Parser.h"
#include "FileReader.h"
#include "HashMap.h"
#include "Terminals.h"
#include "NonTerminals.h"
#include "ParseStack.h"
#include "GrammarArray.h"
#include "StatementRule.h"
#include "AST.h"
#include <string.h>
#include <stdio.h>  
#include <stdlib.h>

void freeKey(void* key);
void GetData(char** dataArr, char* line, int isAction);
unsigned long djb2Hash(const char* str);
unsigned long hashFunc(void* key, int map_size);
void FillTable(HashMap** map, char* filename, char** symbolArray, int symbolCount, int isAction, void (*PutRow)(int state, const char* symbol, const char* value, HashMap* map));
static int equalFunc(void* a, void* b);
static void printKey(void* key);


MapKey* getKey(int state, char* symbol) {
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
static void putState(int state,const char* terminal, const char* action, HashMap* map) {
    // allocate key
    MapKey* key = getKey(state, terminal);
    // allocate the value
    char* val = strdup(action);
    // insert the key - val pair
    insertNewValue(key, val, map);
}

static void actionPrintValue(void* value) {
    char* fvalue = (char*)value;
    printf("Value: action: %s] ", fvalue);
}

/// <summary>
/// This func will setup the action table hashmap
/// </summary>
/// <param name="map"></param>
/// <param name="filename"></param>
void InitActionTable(HashMap** map, char* filename)
{
    *map = initHashMap(INITAL_HASHMAP_SIZE, hashFunc, equalFunc, printKey, actionPrintValue, freeKey, free);
    FillTable(map, filename, Terminals, TerminalCount, 1, putState);
}



static void gotoPrintValue(void* value) {
    int* fvalue = (int*)value;
    printf("Value: nextState: %d] ", *fvalue);
}


void InitGotoTable(HashMap** map, char* filename) {
    *map = initHashMap(INITAL_HASHMAP_SIZE, hashFunc, equalFunc, printKey, gotoPrintValue, freeKey, free);
    FillTable(map, filename, NonTerminals, NonTerminalCount, 0, putState);
}

void freeKey(void* key)
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
void GetData(char** dataArr, char* line, int isAction)
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
/// This is the hash func for the action string, it is a djb2 hash func
/// </summary>
/// <param name="str"></param>
/// <returns></returns>
unsigned long djb2Hash(const char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

/// <summary>
/// This is the hash func for the action table
/// </summary>
/// <param name="key"></param>
/// <param name="map_size"></param>
/// <returns></returns>
unsigned long hashFunc(void* key, int map_size) {
    MapKey* fkey = (MapKey*)key;
    unsigned long terminalHash = djb2Hash(fkey->symbol);
    return (terminalHash + (fkey->currentState * 31)) % map_size;
}


void FillTable(HashMap** map, char* filename, char** symbolArray, int symbolCount, int isAction,
    void (*PutRow)(int state, const char* symbol, const char* value, HashMap* map))
{
    char* fileData = readFile(filename);
    char* saveptr1;
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
        char* lineCopy = strdup(line);
        GetData(tempData, lineCopy, isAction);
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

static int equalFunc(void* a, void* b) {
    MapKey* k1 = (MapKey*)a;
    MapKey* k2 = (MapKey*)b;
    return (k1->currentState == k2->currentState) &&
        (strcmp(k1->symbol, k2->symbol) == 0);
}

static void printKey(void* key)
{
    MapKey* fkey = (MapKey*)key;
    printf("[Key: current_state: %d, symbol: '%s' ", fkey->currentState, fkey->symbol);
}

static char* getMapValue(HashMap* map, int currentState, char* symbol, int isAction)
{
    MapKey temp;
    temp.currentState = currentState;
    temp.symbol = symbol;
    char* valPtr = (char*)getHashMapValue(&temp, map);
    return (valPtr) ? strdup(valPtr) : NULL;
   
}

const char* token_type_to_terminal(Token* token) {
    switch (token->type) {
    case IDENTIFIER: return "IDENTIFIER";
    case INT_LITERAL: return "INT_LITERAL";
    case FLOAT_LITERAL: return "FLOAT_LITERAL";
    case STRING_LITERAL: return "STRING_LITERAL";
    default: return token->lexeme;
    }
}

void Shift(Stack* s, int nextState, Token* token, const char* nodeLable)
{
    StackData tempData;
    tempData.node = createASTNode(token, nodeLable);
    PushStack(s, tempData, NODE);
    tempData.node = NULL;
    tempData.state = nextState;
    PushStack(s, tempData, STATE);
}

int RecoverFromError(pTokenArray tokenArray, int currentIndex) {
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

void HandleSyntaxError(int* errorCount,Stack** s,int* currentIndex,pTokenArray tokenArray,int* finishedParsing)
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
    printf("Syntax Error at token '%s' on line %d, col %d.\n",errorToken->lexeme,errorToken->tokenRow,errorToken->tokenCol);
    // recover
    *currentIndex = RecoverFromError(tokenArray, *currentIndex);
    if (*currentIndex == tokenArray->count - 1) {
        *finishedParsing = 1;
    }
    // reset the stack
    FreeStack(*s);
    *s = InitStack();
    StackData tempData;
    tempData.state = 0;
    PushStack(*s, tempData, STATE);
}

void Reduce(int ruleIndex, Stack** s, GrammarArray* array, HashMap* gotoTable, pTokenArray tokenArray, int* i, int* errorCount, int* finishedParsing)
{
    GrammarRule* rule = array->rules[ruleIndex];
    // get the amount of children in the new node
    int childrenCount = rule->rightWordCount;
    // create the newNode
    ASTNode* newNode = createASTNode(NULL, rule->leftRule);
    // create the temp children array
    ASTNode** nodeArr = (ASTNode**)malloc(sizeof(ASTNode*) * childrenCount);
    // pop from the stack
    for (int j = 0; j < rule->rightWordCount * 2; j++) {
        StackEntry* curData = PopStack(*s);
        // if the data is a state we can free it
        if (curData->type == NODE) {
            // data is a node save it
            nodeArr[j / 2] = (ASTNode*)curData->data.node;
        }
        free(curData);
    }
    // now the nodeArr is in reverse order
    for (int i = 0; i < childrenCount; i++) {
        addChild(nodeArr[i], newNode);
    }
    free(nodeArr);
    // get the goto state
    char* gotoState = getMapValue(gotoTable, (TopStack(*s))->data.state, rule->leftRule, 0);
    if (gotoState == NULL) {
        HandleSyntaxError(&errorCount, &s, &i, tokenArray, &finishedParsing);
        return;
    }
    // preform the reduce
    StackData tempData;
    tempData.node = newNode;
    PushStack(*s, tempData, NODE);
    tempData.state = atoi(gotoState);
    PushStack(*s, tempData, STATE);
}

void FreeParserResources(GrammarArray* array, HashMap** actionTable, HashMap** gotoTable, Stack* s)
{
    // free all the resources used to parse the input
    FreeGrammarArray(array);
    freeHashMap(actionTable);
    freeHashMap(gotoTable);
    FreeStack(s);
}

void ClearStack(Stack* s) {
    while (!IsStackEmpty(s)) {
        PopStack(s);
    }
}

/// <summary>
/// This is the main parse function that checks if the given input is valid 
/// </summary>
/// <param name="tokenArray"></param>
ASTNode* ParseInput(pTokenArray tokenArray, int* errorCount) 
{
    // add $ to the end of the tokenArray
    addToken(&tokenArray, FINISH_INPUT, "$", -1, -1);
    StackData tempData;
    // setup stack
    Stack* s = InitStack();
    // setup action and goto tables
    HashMap* actionTable = NULL;
    HashMap* gotoTable = NULL;
    InitActionTable(&actionTable, "ActionTable.txt");
    InitGotoTable(&gotoTable, "GotoTable.txt");
    // setup grammar array
    GrammarArray* array = InitGrammarArray("Grammar.txt");
    // push initial state onto the stack
    tempData.state = 0;
    PushStack(s, tempData, STATE);
    // main parse loop
    int i = 0;
    int finishedParsing = 0;
    ASTNode* root = NULL;
    while (i < tokenArray->count && !finishedParsing) {
        // get the current state
        StackData* top = TopStack(s);
        int currentState = top->state;
        // get the token
        char* token = token_type_to_terminal(tokenArray->tokens[i]);
        Token* currentToken = tokenArray->tokens[i];
        char* action = getMapValue(actionTable, currentState, token, 1);
        if (action == NULL) {
            HandleSyntaxError(errorCount,&s,&i,tokenArray,&finishedParsing);
        }
        else 
        {
            // check for shift action
            if (!finishedParsing && strncmp(action, "s", 1) == 0) {
                Shift(s, atoi(action + 1), currentToken, token);
                // move to next token
                i++;
            }
            else if (!finishedParsing && strncmp(action, "r", 1) == 0) {
                Reduce(atoi(action + 1), &s, array, gotoTable, tokenArray, &i, errorCount, &finishedParsing);
            }
            else {
                // free the top state
                free(PopStack(s));
                // get the final root node
                StackEntry* topEntry = PopStack(s);
                root = (ASTNode*)topEntry->data.node;
                free(topEntry);
                finishedParsing = 1;
            }
           
        }
       
    }
    // free all the used data structures
    FreeParserResources(array, &actionTable, &gotoTable, s);
    return root;
}

