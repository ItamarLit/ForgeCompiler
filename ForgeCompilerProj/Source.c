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
    // set up data structures for the lexer
    HashMap* state_machine = NULL;
    init_state_machine(&state_machine);
    //printHashMap(state_machine);
    pTokenArray ptoken_array;
    init_state_machine(&state_machine);
    initTokenArray(&ptoken_array);
    // read the input code
    char* inputStr = readFile("Code.txt");
    if (!inputStr) {
        printf("Failed to read input file.\n");
        freeHashMap(&state_machine);
        freeTokenArray(&ptoken_array);
        return -1;
    }
    // Lexical analysis
    lex(state_machine, inputStr, ptoken_array);
    free(inputStr);
    freeHashMap(&state_machine);
    //printTokens(ptoken_array);
    // Syntax analysis
    int parseErrors = 0;
    ASTNode* root = ParseInput(ptoken_array, &parseErrors);
    if (parseErrors || !root) {
        printf("Parsing failed with %d errors.\n", parseErrors);
        freeTokenArray(&ptoken_array);
        return -1;
    }
    // AST compression
    root = compressAST(root);
    // AST normalization
    normalizeAST(root);
    printf("Input passed parsing.\n\n");
    printAST(root, 0);
    // Semantic analysis phase
    int semErrors = 0;
    SymbolTable* globalTable = createNewScope(NULL);
    if (!globalTable) {
        printf("Failed to create global symbol table.\n");
        freeASTNode(root);
        freeTokenArray(&ptoken_array);
        return -1;
    }
    // create the symbol table
    createASTSymbolTable(root, globalTable, &semErrors);
    root->scope = globalTable;
    //printSymbolTables(root);
    int analyzeErrors = 0;
    analyze(root, &analyzeErrors);
    if (semErrors || analyzeErrors) {
        printf("Semantic analysis failed with %d symbol errors and %d analysis errors.\n", semErrors, analyzeErrors);
        freeTokenArray(&ptoken_array);
        freeASTNode(root);
        return -1;
    }
    printf("Input passed semantic analysis.\n\n");
    // Cleanup
    freeTokenArray(&ptoken_array);
    freeASTNode(root);
    return 0;
}


