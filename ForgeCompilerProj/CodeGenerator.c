#pragma warning (disable:4996)
#include <stdio.h>
#include <string.h>
#include "Types.h"
#include "SymbolTable.h"
#include "CodeGenerator.h"
#include "HashMap.h"
#include "StringTable.h"

/// <summary>
/// This function will go over the root and generate the global variables
/// </summary>
/// <param name="root"></param>
void gen_data_seg(ASTNode* root)
{
	// gen the lable
	printf(".data\n");
    if (!root) return;
    // get the global symb table
    SymbolTable* scope = root->scope;
    // go over all global nodes
    for (int i = 0; i < root->childCount; i++) {
        ASTNode* node = root->children[i];
        // look for global var dec
        if (node->lable && strcmp(node->lable, "VarDeclaration") == 0) {
            ASTNode* nameNode = node->children[1];
            char* name = nameNode->token->lexeme;
            // get the symbol table entry
            SymbolEntry* entry = lookUpSymbol(name, scope);
            printf("%s ", name);
            switch (entry->type)
            {
            case TYPE_INT:
                printf("dq %s\n", node->children[3]->token->lexeme);
                break;
            case TYPE_STRING:
                printf("db %s,0\n", node->children[3]->token->lexeme);
                break;
            case TYPE_BOOL:
                printf("db %s\n", strcmp(node->children[3]->token->lexeme, "true")  == 0 ? "1" : "0");
                break;
            default:
                printf("Invalid global var type: %s\n", convertTypeToString(entry->type));
                break;
            }
        }
    }

}

void gen_readOnly_data_seg(HashMap* stringTable) {
    printf(".rodata\n");
    int index;
    for (index = 0; index < stringTable->map_size; index++) {
        if (stringTable->arr[index] != NULL) {
            HashNode* cur = stringTable->arr[index];
            while (cur != NULL) {
                printf("%s db %s,0\n",((StringEntry*)cur->value)->label, (char*)cur->key);
                cur = cur->next;
            }
        }
    }
}
