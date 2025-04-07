#pragma warning(disable:4996)
#include "AST.h"
#include "Token.h"
#include "SymbolTable.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


static const char* REDUNDANT_NODES[] = {
    "if", "meet", "then", "else", "while", "for", "in", "forge", "return",
    "{", "}", ";", "(", ")", ",", ":", "=>", "remold", "mold"
};

const int REDUNDANT_NODE_COUNT = sizeof(REDUNDANT_NODES) / sizeof(REDUNDANT_NODES[0]);

int isRedundantNode(const char* label) {
    for (int i = 0; i < REDUNDANT_NODE_COUNT; i++) {
        if (strcmp(label, REDUNDANT_NODES[i]) == 0) {
            return 1; 
        }
    }
    return 0; 
}

/// <summary>
/// This function creates an AST node and fills it with children pointers
/// </summary>
/// <param name="children"></param>
/// <param name="childCount"></param>
/// <returns> Returns a pointer to the newly created ast node </returns>
ASTNode* createASTNode(Token* token, const char* lable) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Failded to malloc memory for AST node");
        return NULL;
    }
    node->lable = strdup(lable);
    node->childCount = 0;
    node->token = token;
    node->children = NULL;
    node->scope = NULL;
    return node;
}

void addChild(ASTNode* child, ASTNode* parent) {
    // alocate the array of pointers of children
    ASTNode** temp = (ASTNode**)realloc(parent->children, sizeof(ASTNode*) * (parent->childCount + 1));
    if (!temp) 
    {
        fprintf(stderr, "Failded to calloc memory for AST node children array");
        return;
    }
    parent->children = temp;
    parent->children[parent->childCount] = child;
    parent->childCount++;
    child->parent = parent; // add the parent pointer
}


/// <summary>
/// This function gets an AST node and frees it including freeing its children, but it doesnt touch the tokens
/// </summary>
/// <param name="node"></param>
void freeASTNode(ASTNode* node) {
    if (node == NULL) {
        return;
    }
    // free all the children using recursion
    for (int i = 0; i < node->childCount; i++) {
        freeASTNode(node->children[i]);
    }
    if (node->scope) {
        freeHashMap(&(node->scope->table));
        free(node->scope);
    }
    // free the array of pointers
    free(node->children);
    // free the node lable
    free(node->lable);
    // free the node itself
    free(node);
}

void printAST(ASTNode* root, int tabcount) 
{
    if (root == NULL) {
        return;
    }
    Token* currentToken = root->token;
    for (int i = 0; i < tabcount; i++) {
        printf("\t");
    }
    if (root->token != NULL) {
        printf("Type: %d, Lexeme: %s, Row: %d, Col: %d\n", currentToken->type, currentToken->lexeme, currentToken->tokenRow, currentToken->tokenCol);
    }
    else {
        printf("%s\n", root->lable);
    }
    for (int i = 0; i < root->childCount; i++) {
        printAST(root->children[i], tabcount + 1);
    }
}

/// <summary>
/// This is a helper function that removes now NULL children from the array
/// </summary>
/// <param name="node"></param>
void removeNullChildren(ASTNode* node) {
    int newCount = 0;
    for (int i = 0; i < node->childCount; i++) {
        if (node->children[i] != NULL) {
            node->children[newCount++] = node->children[i];
        }
    }
    node->childCount = newCount;
}

/// <summary>
/// This is a helper function that compresses children of a given node
/// </summary>
/// <param name="node"></param>
void recursivelyCompressChildren(ASTNode* node) {
    for (int i = 0; i < node->childCount; i++) {
        node->children[i] = compressAST(node->children[i]);
        if (node->children[i]) {
            node->children[i]->parent = node; 
        }
    }
    removeNullChildren(node);
}

ASTNode* removeRedundantLabeledNode(ASTNode* node) {
    if (!node) return NULL;

    if (node->token && isRedundantNode(node->token->lexeme)) {
        // if node has no children just free it
        if (node->childCount == 0) {
            free(node->children);
            free(node->lable);
            free(node);
            return NULL;
        }
    }

    return node;
}

/// <summary>
/// This is a helper function that merges lists of the same lable for example StatementList
/// </summary>
/// <param name="node"></param>
/// <param name="targetLabel"></param>
void mergeNestedLists(ASTNode* node, const char* targetLabel) {
    // check if the node has the targetLable
    if (node->lable && strcmp(node->lable, targetLabel) == 0) {
        // create a merged node arr
        int capacity = node->childCount;
        ASTNode** merged = (ASTNode**)malloc(sizeof(ASTNode*) * capacity);
        if (!merged)
        {
            fprintf(stderr, "Unable to malloc memory for merged AST list\n");
            return;
        }
        int mergedCount = 0;
        // go over all children of current node
        for (int i = 0; i < node->childCount; i++) {
            ASTNode* child = node->children[i];
            // if the child has the same lable as target lable merge its children into the node
            if (child && child->lable && strcmp(child->lable, targetLabel) == 0) {
                // make space
                if (mergedCount + child->childCount > capacity) {
                    capacity = (mergedCount + child->childCount) * 2;
                    ASTNode** tempMerge = (ASTNode**)realloc(merged, sizeof(ASTNode*) * capacity);
                    if (!tempMerge) 
                    {
                        fprintf(stderr, "Unable to realloc memory for merged AST list\n");
                        return;
                    }
                    merged = tempMerge;
                }
                // move the children into the merged array
                for (int c = 0; c < child->childCount; c++) {
                    merged[mergedCount] = child->children[c];
                    if (child->children[c]) {
                        child->children[c]->parent = node;  
                    }
                    mergedCount++;
                }
                // free the node and all its data since the children have been merged
                free(child->children);
                free(child->lable);
                free(child);
            }
            else {
                // if child doesnt have the correct label just merge it into the array
                if (mergedCount >= capacity) {
                    capacity *= 2;
                    ASTNode** tempMerge = (ASTNode**)realloc(merged, sizeof(ASTNode*) * capacity);
                    if (!tempMerge)
                    {
                        fprintf(stderr, "Unable to realloc memory for merged AST list\n");
                        return;
                    }
                    merged = tempMerge;
                }
                merged[mergedCount++] = child;
            }
        }
        // replace the original children array with the merged array
        free(node->children);
        node->children = merged;
        node->childCount = mergedCount;
    }
}

int isKeptSingleNode(const char* label) {
    const char* singles[] = {
        "Block", "ReturnStatement",  "ParamList", "ArgumentList",
        "GlobalItemList", "OptionalElse", "StatementList", "FuncCallExpr",
    };
    for (int i = 0; i < sizeof(singles) / sizeof(singles[0]); i++) {
        if (strcmp(label, singles[i]) == 0) return 1;
    }
    return 0;
}

int isKeptEmptyNode(const char* label) {
    const char* empties[] = { "Block", "ParamList", "ArgumentList", "ReturnStatement" };
    for (int i = 0; i < sizeof(empties) / sizeof(empties[0]); i++) {
        if (strcmp(label, empties[i]) == 0) return 1;
    }
    return 0;
}

/// <summary>
/// This function optimizes the syntax tree by removing uneeded nodes
/// </summary>
/// <param name="node"></param>
/// <returns>Returns an abstract syntax tree</returns>
ASTNode* compressAST(ASTNode* node)
{
    // check if node is null
    if (!node) return NULL;

    recursivelyCompressChildren(node);

    // remove nodes with no children
    if (node->token == NULL && node->childCount == 0) {
        // keep these
        if (isKeptEmptyNode(node->lable)) return node;

        free(node->children);
        free(node->lable);
        free(node);
        return NULL;
    }
    // flatten the nodes with no token but with 1 child
    if (node->token == NULL && node->childCount == 1) {
        // keep these
        if (isKeptSingleNode(node->lable)) return node;

        ASTNode* child = node->children[0];
        // update parent pointer
        child->parent = node->parent;
        free(node->children);
        free(node->lable);
        free(node);
        return child;
        
    }
    node = removeRedundantLabeledNode(node);
    if (!node) return NULL;

    // merge all the nested lists
    mergeNestedLists(node, "GlobalItemList");
    mergeNestedLists(node, "StatementList");
    mergeNestedLists(node, "ParamList");
    mergeNestedLists(node, "ArgumentList");

    return node;
}

/// <summary>
/// This func is used to normalize the AST (add the missing control nodes back)
/// </summary>
/// <param name="node"></param>
void normalizeAST(ASTNode* node) {
    if (!node) return;
    // make sure func dec has a paramList node at index 1 
    if (strcmp(node->lable, "FuncDeclaration") == 0 && strcmp(node->children[1]->lable, "ParamList") != 0) {
        // create an empty paramlist node
        ASTNode* emptyParamList = createASTNode(NULL, "ParamList");
        // move the children forward
        ASTNode** tempChildren = (ASTNode**)realloc(node->children, sizeof(ASTNode*) * (node->childCount + 1));
        if (!tempChildren) {
            fprintf(stderr, "Unable to realloc memory for children in AST\n");
        }
        node->children = tempChildren;
        for (int i = node->childCount; i > 1; i--) {
            node->children[i] = node->children[i - 1];
        }
        // insert the new param list node at index 1
        node->children[1] = emptyParamList;
        emptyParamList->parent = node;
        node->childCount++;
        
    }
    // make sure func calls have an argument list at node 1
    else if (strcmp(node->lable, "FuncCallExpr") == 0 && (node->childCount < 2 || strcmp(node->children[1]->lable, "ArgumentList") != 0)) {
        // create an empty argument list node
        ASTNode* emptyArgList = createASTNode(NULL, "ArgumentList");
        // move the children forward
        ASTNode** tempChildren = (ASTNode**)realloc(node->children, sizeof(ASTNode*) * (node->childCount + 1));
        if (!tempChildren) {
            fprintf(stderr, "Unable to realloc memory for children in AST\n");
        }
        node->children = tempChildren;
        for (int i = node->childCount; i > 1; i--) {
            node->children[i] = node->children[i - 1];
        }
        // insert the node at the correct index
        node->children[1] = emptyArgList;
        emptyArgList->parent = node;
        node->childCount++;
        
    }
    // make sure the block has a statement list node
    else if (strcmp(node->lable, "Block") == 0 && (node->childCount == 0 || strcmp(node->children[0]->lable, "StatementList") != 0)) {
        // create an empty statement list node
        ASTNode* emptyStmtList = createASTNode(NULL, "StatementList");
        // move the children forward
        ASTNode** tempChildren = (ASTNode**)realloc(node->children, sizeof(ASTNode*) * (node->childCount + 1));
        if (!tempChildren) {
            fprintf(stderr, "Unable to realloc memory for children in AST\n");
        }
        node->children = tempChildren;
        for (int i = node->childCount; i > 0; i--) {
            node->children[i] = node->children[i - 1];
        }
        // insert the node at the correct place
        node->children[0] = emptyStmtList;
        emptyStmtList->parent = node;
        node->childCount++;
        
    }

    // go over all the AST
    for (int i = 0; i < node->childCount; i++) {
        normalizeAST(node->children[i]);
    }
}


/// <summary>
/// This is a helper func that can evaluate constant expressions
/// </summary>
/// <param name="node"></param>
/// <returns></returns>
int evaluateExpr(ASTNode* node) {
    if (!node) return 0;

    // literal
    if (node->token && node->token->type == INT_LITERAL) {
        return atoi(node->token->lexeme);
    }
    // get the node lable
    const char* label = node->lable;
    // check if it is an AddExpr
    if (strcmp(label, "AddExpr") == 0) {
        return node->children[1]->token->lexeme[0] == '+'
            ? evaluateExpr(node->children[0]) + evaluateExpr(node->children[2])
            : evaluateExpr(node->children[0]) - evaluateExpr(node->children[2]);
    }
    // check if it is a MulExpr
    if (strcmp(label, "MulExpr") == 0) {
        return node->children[1]->token->lexeme[0] == '*'
            ? evaluateExpr(node->children[0]) * evaluateExpr(node->children[2])
            : evaluateExpr(node->children[0]) / evaluateExpr(node->children[2]);
    }
    // check if it is a unary Expr
    if (strcmp(label, "UnaryExpr") == 0 && node->childCount == 2) {
        return -evaluateExpr(node->children[1]);
    }
    // the child node is an Expr node
    return evaluateExpr(node->children[0]);
}


/// <summary>
/// This func reduces the global vars to just values, ie no expressions
/// </summary>
/// <param name="globalItemList"></param>
void reduceGlobalVars(ASTNode* globalItemList) {
    // go over all the children
    SymbolTable* scope = globalItemList->scope;

    for (int i = 0; i < globalItemList->childCount; i++) {
        ASTNode* varDecl = globalItemList->children[i];
        // found global var
        if (strcmp(varDecl->lable, "VarDeclaration") == 0)
        {
            // get the expr
            ASTNode* exprNode = varDecl->children[3];
            SymbolEntry* entry = lookUpSymbol(varDecl->children[1]->token->lexeme, scope);
            if (entry->type == TYPE_INT) {
                // eval
                int value = evaluateExpr(exprNode);
                // replace old nodes
                varDecl->children[3] = NULL;
                freeASTNode(exprNode);
                // create new node
                char buffer[32];
                sprintf(buffer, "%d", value);
                // create new token
                Token* valueToken = (Token*)malloc(sizeof(Token));
                if (!valueToken) {
                    fprintf(stderr, "Unable to malloc new token\n");
                    return;
                }
                // set node values
                valueToken->type = INT_LITERAL;
                strcpy(valueToken->lexeme, buffer);
                valueToken->tokenRow = 0;
                valueToken->tokenCol = 0;
                ASTNode* constantNode = createASTNode(valueToken, "Const");
                // insert new node
                varDecl->children[3] = constantNode;
                constantNode->parent = varDecl;
            }
            
        }
    }
}