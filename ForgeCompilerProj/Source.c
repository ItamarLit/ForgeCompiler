#include "HashMap.h"
#include "Lexer.h"
#include "Token.h"
#include "FileReader.h"
#include "GrammarArray.h"
#include "Parser.h"
#include "AST.h"
#include "SymbolTable.h"
#include "SemanticAnalyzer.h"

int main() {

    HashMap* state_machine = NULL;
    // init the state machine for the lexer hashmap
    init_state_machine(&state_machine);
    pTokenArray ptoken_array;
    // init the token array
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
    /* HashMap* goto_table = NULL;

    InitGotoTable(&goto_table, "GotoTable.txt");
    freeHashMap(&goto_table);
    HashMap* action_table = NULL;

    InitActionTable(&action_table, "ActionTable.txt");
    freeHashMap(&action_table);*/
    int errors = 0;
    ASTNode* root = ParseInput(ptoken_array, &errors);
    // compress syntax tree into AST
    root = compressAST(root);
    if (!errors) {
        printf("Input passed parsing");
        puts("\n");
        printAST(root, 0);
        int errorCount = 0;
        SymbolTable* globalTable = createNewScope(NULL);
        createASTSymbolTable(root, globalTable, &errorCount);
        // attach global symbol table to GlobalItemList Node
        root->scope = globalTable;
        printSymbolTables(root);
        puts("\n");
        int errorCount2 = 0;
        analyze(root, &errorCount2);
    }
    freeTokenArray(&ptoken_array);
    freeASTNode(root);
    //grammar array tests
    /*GrammarArray* array = InitGrammarArray("Grammar.txt");
    PrintGrammarRules(array);
    FreeGrammarArray(array);*/

    

    return 0;
}

