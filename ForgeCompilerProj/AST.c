#include "AST.h"
#include <stdlib.h>
#include <string.h>


/// <summary>
/// This function creates an AST node and fills it with children pointers
/// </summary>
/// <param name="children"></param>
/// <param name="childCount"></param>
/// <returns></returns>
ASTNode* createASTNode(ASTNode** children, int childCount) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->childCount = childCount;
    // alocate the array of pointers of children
    node->children = (ASTNode**)calloc(childCount, sizeof(ASTNode*));
    for (int i = 0; i < node->childCount; i++) {
        node->children[i] = children[i];
    }
    return node;
}

// this function gets an AST node and frees it including freeing its children, but it doesnt touch the tokens
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
    // free the node itself
    free(node);
}