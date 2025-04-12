#ifndef GRAMMAR_ARRAY_H
#define GRAMMAR_ARRAY_H

// const grammar size
#define GRAMMAR_SIZE 80

typedef struct GrammarRule {
	char* leftRule;
	char* rightRule;
	int rightWordCount;
} GrammarRule;

typedef struct {
    GrammarRule** rules;
	int count;
}GrammarArray, * pGrammarArray;

// func that will insert a grammar rule into the array
void insert_rule(GrammarArray* grammar, int index, const char* leftRule, const char* rightRule);
// func that will setup the grammar array from the Grammar.txt file
GrammarArray* init_grammar_array(const char* filename);
// frees all memory from the array after use
void free_grammar_array(GrammarArray* grammar);
// Print func used in debugging
void print_grammar_rules(GrammarArray* grammar);

#endif

