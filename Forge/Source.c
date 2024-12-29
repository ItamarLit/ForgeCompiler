#include "HashMap.h"
#include "Lexer.h"
#include "Token.h"
#include "FileReader.h"

int main() {
    HashMap* state_machine = NULL;
    init_state_machine(&state_machine);
    pTokenArray ptoken_array;
    initTokenArray(&ptoken_array);
    if (ptoken_array->tokens == NULL) {
        return -1;
    }
    char* inputStr = NULL;
    // read the file
    inputStr = readFile("Code.txt");
    lex(state_machine, inputStr, ptoken_array);

    //printHashMap(state_machine);
    // free the input string
    free(inputStr);
    inputStr = NULL;
    // Free the hash map
    freeHashMap(&state_machine);
    printTokens(ptoken_array);
    freeTokenArray(&ptoken_array);
    return 0;
}

