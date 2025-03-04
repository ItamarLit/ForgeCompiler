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
void InsertRule(GrammarArray* grammar, int index, const char* leftRule, const char* rightRule);
// func that will setup the grammar array from the Grammar.txt file
GrammarArray* InitGrammarArray(const char* filename);
// frees all memory from the array after use
void FreeGrammarArray(GrammarArray* grammar);
// Print func used in debugging
void PrintGrammarRules(GrammarArray* grammar);

#endif

