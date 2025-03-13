#pragma warning(disable:4996)
#include "AST.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


/// <summary>
/// This function creates an AST node and fills it with children pointers
/// </summary>
/// <param name="children"></param>
/// <param name="childCount"></param>
/// <returns> Returns a pointer to the newly created ast node </returns>
ASTNode* createASTNode(Token* token, const char* lable) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) {
        printf("Failded to malloc memory for AST node");
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
    parent->children = (ASTNode**)realloc(parent->children, sizeof(ASTNode*) * (parent->childCount + 1));
    if (!parent->children) {
        printf("Failded to calloc memory for AST node children array");
    }
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



/// <summary>
/// This is a helper function that merges lists of the same lable for example StatementList
/// </summary>
/// <param name="node"></param>
/// <param name="targetLabel"></param>
void mergeNestedLists(ASTNode* node, const char* targetLabel) {
    if (node->lable && strcmp(node->lable, targetLabel) == 0) {
        int capacity = node->childCount;
        ASTNode** merged = (ASTNode**)malloc(sizeof(ASTNode*) * capacity);
        int mergedCount = 0;

        for (int i = 0; i < node->childCount; i++) {
            ASTNode* child = node->children[i];
            if (child && child->lable && strcmp(child->lable, targetLabel) == 0) {
                if (mergedCount + child->childCount > capacity) {
                    capacity = (mergedCount + child->childCount) * 2;
                    merged = (ASTNode**)realloc(merged, sizeof(ASTNode*) * capacity);
                }
                for (int c = 0; c < child->childCount; c++) {
                    merged[mergedCount] = child->children[c];
                    if (child->children[c]) {
                        child->children[c]->parent = node;  
                    }
                    mergedCount++;
                }
                free(child->children);
                free(child->lable);
                free(child);
            }
            else {
                if (mergedCount >= capacity) {
                    capacity *= 2;
                    merged = (ASTNode**)realloc(merged, sizeof(ASTNode*) * capacity);
                }
                merged[mergedCount++] = child;
            }
        }
        free(node->children);
        node->children = merged;
        node->childCount = mergedCount;
    }
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
        if (strcmp(node->lable, "Block") == 0 || strcmp(node->lable, "ParamList") == 0 || strcmp(node->lable, "ArgumentList") == 0) {
            return node; 
        }
        free(node->children);
        free(node->lable);
        free(node);
        return NULL;
    }
    // flatten the nodes with no token but with 1 child
    if (node->token == NULL && node->childCount == 1) {
        // keep these
        if (strcmp(node->lable, "Block") == 0 || strcmp(node->lable, "ParamList") == 0 ||strcmp(node->lable, "ArgumentList") == 0 || strcmp(node->lable, "GlobalItemList") == 0 || strcmp(node->lable, "StatementList") == 0) {
            return node; 
        }
        else {
            ASTNode* child = node->children[0];
            // update parent pointer
            child->parent = node->parent;
            free(node->children);
            free(node->lable);
            free(node);
            return child;
        }
    }
    // merge all the nested lists
    mergeNestedLists(node, "GlobalItemList");
    mergeNestedLists(node, "StatementList");
    mergeNestedLists(node, "ParamList");
    mergeNestedLists(node, "ArgumentList");

    return node;
}