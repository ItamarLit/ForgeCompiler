#pragma warning (disable:4996)
#include "GrammarArray.h"
#include "FileReader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// <summary>
/// Helper function that counts tokens
/// </summary>
/// <param name="rhs"></param>
/// <returns>TokenCount</returns>
int count_tokens(const char* rhs) {
    // at least one token exists
    int token_count = 1;
    for (int i = 0; rhs[i] != '\0'; i++) {
        if (isspace(rhs[i])) {
            token_count++;
        }
    }
    return token_count;
}


/// <summary>
/// This func is used to insert a grammar rule into the array
/// </summary>
/// <param name="grammar"></param>
/// <param name="index"></param>
/// <param name="leftRule"></param>
/// <param name="rightRule"></param>
void insert_rule(GrammarArray* grammar, int index, const char* leftRule, const char* rightRule) {
	grammar->rules[index] = (GrammarRule*)malloc(sizeof(GrammarRule));
	if (!grammar->rules[index]) {
		fprintf(stderr, "Failed to allocate GrammarRule\n");
		return;
	}
	grammar->rules[index]->leftRule = strdup(leftRule);
	grammar->rules[index]->rightRule = strdup(rightRule);
    grammar->rules[index]->rightWordCount = count_tokens(rightRule);
}

/// <summary>
/// This func creates a new grammar array and fills it with the rules from the file
/// </summary>
/// <param name="filename"></param>
/// <returns>Returns a grammar array</returns>
GrammarArray* init_grammar_array(const char* filename) {
	// malloc the main struct
	GrammarArray* grammar = (GrammarArray*)malloc(sizeof(GrammarArray));
	if (!grammar) {
		fprintf(stderr, "Failed to allocate GrammarArray\n");
		return NULL;
	}
	// malloc the rule pointers 
	grammar->rules = (GrammarRule**)malloc(sizeof(GrammarRule*) * GRAMMAR_SIZE);
	if (!grammar->rules) {
		printf(stderr, "Failed to allocate rules array\n");
		free(grammar);
		return NULL;
	}
    grammar->count = 0;
	// setup the rules into the array
    char* buffer = read_file(filename);
    if (!buffer) {
        fprintf(stderr, "Failed to read file: %s\n", filename);
        free(grammar->rules);
        free(grammar);
        return NULL;
    }

    // Tokenize the buffer line by line using "\n" as the delimiter.
    char* line = strtok(buffer, "\n");
    while (line != NULL) {
        // skip empty lines
        if (strlen(line) == 0) {
            line = strtok(NULL, "\n");
            continue;
        }
        // look for -> in the line
        char* arrowPos = strstr(line, "->");
        if (!arrowPos) {
            fprintf(stderr, "Invalid grammar line (missing '->'): %s\n", line);
            line = strtok(NULL, "\n");
            continue;
        }
        // split the line into left and right rule parts
        *arrowPos = '\0';
        char* left = trim(line);
        char* right = trim(arrowPos + 2);
        // insert the rule
        insert_rule(grammar, grammar->count, left, right);
        grammar->count++;
        // get next line
        line = strtok(NULL, "\n");
    }
    // free the buffer
    free(buffer);
    return grammar;
}

/// <summary>
/// This func is used to free all the allocated memory of a given grammar array
/// </summary>
/// <param name="grammar"></param>
void free_grammar_array(GrammarArray* grammar) {
	for (int i = 0; i < grammar->count; i++) {
		if (grammar->rules[i]) {
			free(grammar->rules[i]->leftRule);
			free(grammar->rules[i]->rightRule);
			free(grammar->rules[i]);
		}
	}
	free(grammar->rules);
	free(grammar);
}

/// <summary>
/// This is a debug func used to print a given grammar
/// </summary>
/// <param name="grammar"></param>
void print_grammar_rules(GrammarArray* grammar) {
    if (!grammar) {
        printf("GrammarArray is NULL.\n");
        return;
    }
    printf("Grammar Rules:\n");
    for (int i = 0; i < grammar->count; i++) {
        if (grammar->rules[i]) {
            printf("[%d] %s -> %s\n", i, grammar->rules[i]->leftRule, grammar->rules[i]->rightRule);
        }
    }
}
