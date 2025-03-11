#pragma warning(disable:4996)
#include "AST.h"
#include <stdlib.h>
#include <string.h>


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
    return node;
}

void addChild(ASTNode* child, ASTNode* parent) {
    // alocate the array of pointers of children
    parent->children = (ASTNode**)realloc(parent->children, sizeof(ASTNode*) * (parent->childCount + 1));
    if (!parent->children) {
        printf("Failded to calloc memory for AST node children array");
        return NULL;
    }
    parent->children[parent->childCount] = child;
    parent->childCount++;
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
    // free the array of pointers
    free(node->children);
    // free the node lable
    free(node->lable);
    // free the node itself
    free(node);
}