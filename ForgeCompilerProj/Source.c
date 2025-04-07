#pragma warning (disable:4996)
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
#include "CompilerUtils.h"
#include <string.h>
#include <stdio.h>

/// <summary>
/// This helper function will create a path to an output.asm file in the same path as the input file
/// </summary>
/// <param name="filepath"></param>
/// <param name="new_filename"></param>
/// <returns>Returns the new path</returns>
char* change_extension(const char* filepath, const char* new_filename) {
    // find last backslash
    const char* last_backslash = strrchr(filepath, '\\');
    // calc dir len 
    int dir_len = last_backslash ? (last_backslash - filepath + 1) : 0;
    // create string
    char* output = malloc(dir_len + strlen(new_filename) + 1);
    if (!output) return NULL;
    if (dir_len > 0) {
        strncpy(output, filepath, dir_len);
    }
    output[dir_len] = '\0';
    strcat(output, new_filename);
    return output;
}

int main(int argc, char* argv[]) 
{
    // create flags 
    int tokensFlag = 0;
    int astFlag = 0;
    int symbolFlag = 0;
    int asmFlag = 0;
    flagEntry flagTable[] = {
        {&tokensFlag, "-T"},
        {&astFlag, "-A"},
        {&symbolFlag, "-S"},
        {&asmFlag, "-O"},
        {NULL, NULL},
    };
   
    if (argc < 2) {
        fprintf(stderr, "Usage: <batch file> <input file> [flags]\n");
        return 1;
    }
    const char* inputFile = argv[1];
    char* outputFile = change_extension(inputFile, "output.asm");
    printf("Input: %s\n", inputFile);
    printf("Output: %s\n", outputFile);
    // read the input code
    char* inputStr = read_file(inputFile);
    if (!inputStr) {
        fprintf(stderr, "**************************\n");
        fprintf(stderr, "Failed to read input file.\n");
        fprintf(stderr, "**************************\n");
        return 1;
    }
    // set flags
    for (int i = 2; i < argc; i++) {
        for (int j = 0; flagTable[j].label != NULL; j++) 
        {
            if (strcmp(flagTable[j].label, argv[i]) == 0) *(flagTable[j].flag) = 1;
        }
    }
    // run compiler
    int exitCode = compile(outputFile,inputStr, tokensFlag, astFlag, symbolFlag, asmFlag);
    free(outputFile);
    return exitCode;
}

/// <summary>
/// This is the main compiler function it runs the compiler and returns an exit code
/// </summary>
/// <param name="outputPath"></param>
/// <param name="inputStr"></param>
/// <param name="tokensFlag"></param>
/// <param name="astFlag"></param>
/// <param name="symbolFlag"></param>
/// <param name="asmFlag"></param>
/// <returns>Exit code</returns>
int compile(const char* outputPath, const char* inputStr, int tokensFlag, int astFlag, int symbolFlag, int asmFlag)
{
    // set up data structures for the lexer
    HashMap* state_machine = NULL;
    init_state_machine(&state_machine);
    pTokenArray ptoken_array;
    init_token_array(&ptoken_array);
    int lexErrors = 0;
    // Lexical analysis
    lex(state_machine, inputStr, ptoken_array, &lexErrors);
    free(inputStr);
    free_hashmap(&state_machine);
    if (lexErrors)
    {
        fprintf(stderr, "**************************\n");
        fprintf(stderr, "Lexing failed with %d errors.\n", lexErrors);
        fprintf(stderr, "**************************\n");
        return 1;
    }
    printf("\n\nInput passed lexing.\n\n");
    if(tokensFlag) print_tokens(ptoken_array);
    // Syntax analysis
    int parseErrors = 0;
    ASTNode* root = parse(ptoken_array, &parseErrors);
    if (parseErrors || !root) {
        fprintf(stderr, "**************************\n");
        fprintf(stderr, "Parsing failed with %d errors.\n", parseErrors);
        fprintf(stderr, "**************************\n");
        free_token_array(&ptoken_array);
        return 1;
    }
    // AST compression
    root = compress_AST(root);
    // AST normalization
    normalize_AST(root);
    printf("\n\nInput passed parsing.\n\n");
    if (astFlag) {
        printf("Here is the AST:\n");
        print_AST(root, 0);
    }
    // Semantic analysis phase
    int semErrors = 0;
    SymbolTable* globalTable = create_new_scope(NULL);
    if (!globalTable) {
        fprintf(stderr, "**************************\n");
        fprintf(stderr, "Failed to create global symbol table.\n");
        fprintf(stderr, "**************************\n");
        free_AST_node(root);
        free_token_array(&ptoken_array);
        return 1;
    }
    // create the symbol table
    create_AST_symbol_table(root, globalTable, &semErrors);
    root->scope = globalTable;
    if(symbolFlag) print_symbol_tables(root);
    int analyzeErrors = 0;
    analyze(root, &analyzeErrors);
    if (semErrors || analyzeErrors) {
        fprintf(stderr, "**************************\n");
        fprintf(stderr, "Semantic analysis failed with %d symbol errors and %d analysis errors.\n", semErrors, analyzeErrors);
        fprintf(stderr, "**************************\n");
        free_token_array(&ptoken_array);
        free_AST_node(root);
        return 1;
    }
    printf("\n\nInput passed semantic analysis.\n\n");
    // reduce Global vars
    reduce_global_vars(root);
    HashMap* stringTable = create_string_table(root);
    gen_asm(outputPath, root, stringTable, asmFlag);
    // Cleanup
    free_token_array(&ptoken_array);
    free_AST_node(root);
    free_hashmap(&stringTable);
    printf("\n\nFinished compiling!!\n\n");
    return 0;
}