#include "HashMap.h"
#include "Lexer.h"
#include "Token.h"
#include "FileReader.h"
#include "GrammarArray.h"
#include "Parser.h"
#include "AST.h"
#include "SymbolTable.h"
#include "SemanticAnalyzer.h"
#include "CodeGenerator.h"
#include "StringTable.h"

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
        fprintf(stderr, "**************************\n");
        fprintf(stderr, "Failed to read input file.\n");
        fprintf(stderr, "**************************\n");
        freeHashMap(&state_machine);
        freeTokenArray(&ptoken_array);
        return -1;
    }
    int lexErrors = 0;
    // Lexical analysis
    lex(state_machine, inputStr, ptoken_array, &lexErrors);
    free(inputStr);
    freeHashMap(&state_machine);
    if (lexErrors) 
    {
        fprintf(stderr, "**************************\n");
        fprintf(stderr, "Lexing failed with %d errors.\n", lexErrors);
        fprintf(stderr, "**************************\n");
        return -1;
    }
    printTokens(ptoken_array);

    // Syntax analysis
    int parseErrors = 0;
    ASTNode* root = ParseInput(ptoken_array, &parseErrors);
    if (parseErrors || !root) {
        fprintf(stderr, "**************************\n");
        fprintf(stderr, "Parsing failed with %d errors.\n", parseErrors);
        fprintf(stderr, "**************************\n");
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
        fprintf(stderr, "**************************\n");
        fprintf(stderr, "Failed to create global symbol table.\n");
        fprintf(stderr, "**************************\n");
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
        fprintf(stderr, "**************************\n");
        fprintf(stderr, "Semantic analysis failed with %d symbol errors and %d analysis errors.\n", semErrors, analyzeErrors);
        fprintf(stderr, "**************************\n");
        freeTokenArray(&ptoken_array);
        freeASTNode(root);
        return -1;
    }
    printf("Input passed semantic analysis.\n\n");
    // Reduce Global vars
    reduceGlobalVars(root);
    HashMap* stringTable = createStringTable(root);
    gen_asm(root, stringTable);
    // Cleanup
    freeTokenArray(&ptoken_array);
    freeASTNode(root);
    freeHashMap(&stringTable);
    return 0;
}


