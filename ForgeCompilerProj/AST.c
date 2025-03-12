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
/// This function optimizes the syntax tree by removing uneeded nodes
/// </summary>
/// <param name="node"></param>
/// <returns>Returns an abstract syntax tree</returns>
ASTNode* compressAST(ASTNode* node)
{
    // check if node is null
    if (!node) return NULL;
    // recursively compress the children
    for (int i = 0; i < node->childCount; i++) {
        node->children[i] = compressAST(node->children[i]);
    }
    // some children reduce to null so we count the non null 
    int newCount = 0;
    for (int i = 0; i < node->childCount; i++) {
        if (node->children[i] != NULL) {
            node->children[newCount++] = node->children[i];
        }
    }
    // set the new count
    node->childCount = newCount;
    // if a node has no token and no children remove it
    if (node->token == NULL && node->childCount == 0) {
        free(node->children);
        free(node->lable);
        free(node);
        return NULL;
    }
    // if a node has no token but is a block lable keep it
    if (node->token == NULL && node->childCount == 1) {
        if (strcmp(node->lable, "Block") == 0) {
            return node;
        }
        // replace the node with its child ( flatten it )
        else {
            ASTNode* child = node->children[0];
            free(node->children);
            free(node->lable);
            free(node);
            return child;
        }
    }
    // keep the global item list, merge all global item list and global items
    // this is done because of how the grammar is set "GlobalItemList -> GlobalItemList GlobalItem" there will always be a 
    // global item after the list and I want them to be together
    if (node->lable && strcmp(node->lable, "GlobalItemList") == 0) {
        // alocate a starting array for children
        int capacity = node->childCount;
        ASTNode** merged = (ASTNode**)malloc(sizeof(ASTNode*) * capacity);
        int mergedCount = 0;
        // go over all children
        for (int i = 0; i < node->childCount; i++) {
            ASTNode* child = node->children[i];
            // get all children that are global item list and add there children to the current node
            if (child && child->lable && strcmp(child->lable, "GlobalItemList") == 0)
            {
                // add to children capacity
                if (mergedCount + child->childCount > capacity) {
                    capacity = (mergedCount + child->childCount) * 2;
                    merged = (ASTNode**)realloc(merged, sizeof(ASTNode*) * capacity);
                }
                // move children to current node
                for (int c = 0; c < child->childCount; c++) {
                    merged[mergedCount++] = child->children[c];
                }
                // free the child node
                free(child->children);
                free(child->lable);
                free(child);
            }
            else {
                // if needed realloc the array
                if (mergedCount >= capacity) {
                    capacity *= 2;
                    merged = (ASTNode**)realloc(merged, sizeof(ASTNode*) * capacity);
                }
                merged[mergedCount++] = child;
            }
        }
        // replace the old array and count
        free(node->children);
        node->children = merged;
        node->childCount = mergedCount;
    }
    // return the final root
    return node;
}

