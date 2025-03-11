#ifndef STATEMENT_RULE_H
#define STATEMENT_RULE_H

typedef struct StatementRule {
    const char* token;   
    int ruleIndex;       
} StatementRule;

extern StatementRule statementTable[];
int GetStatementRuleIndex(const char* token);


#endif 
