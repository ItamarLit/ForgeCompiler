#include "HashMap.h"
#include "Lexer.h"
#include "Token.h"
#include "FileReader.h"
#include "GrammarArray.h"
#include "Parser.h"

int main() {
    HashMap* state_machine = NULL;
    //// init the state machine for the lexer hashmap
    //init_state_machine(&state_machine);
    //pTokenArray ptoken_array;
    //// init the token array
    //initTokenArray(&ptoken_array);
    //if (ptoken_array->tokens == NULL) {
    //    return -1;
    //}
    //char* inputStr = NULL;
    //// read the file
    //inputStr = readFile("Code.txt");
    //lex(state_machine, inputStr, ptoken_array);

    //printHashMap(state_machine);
    //// free the input string
    //free(inputStr);
    //inputStr = NULL;
    //// Free the hash map
    //freeHashMap(&state_machine);
    //printTokens(ptoken_array);
    //freeTokenArray(&ptoken_array);

    // grammar array tests
    /*GrammarArray* array = InitGrammarArray("Grammar.txt");
    PrintGrammarRules(array);
    FreeGrammarArray(array);*/

    InitActionTable(&state_machine, "ActionTable.txt");
    printHashMap(state_machine);

    return 0;
}

