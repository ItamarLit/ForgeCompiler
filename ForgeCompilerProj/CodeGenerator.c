#pragma warning (disable:4996)
#include <stdio.h>
#include <string.h>
#include "Types.h"
#include "ExprCodeGen.h"
#include "SymbolTable.h"
#include "StmtCodeGen.h"
#include "CodeGenerator.h"
#include "HashMap.h"
#include "StringTable.h"
#include "CodeGenUtils.h"

void gen_function(ASTNode* node, HashMap* stringTable);
void gen_readOnly_data(HashMap* stringTable);

/// <summary>
/// This function will go over the root and generate the global variables
/// </summary>
/// <param name="root"></param>
void gen_data_seg(ASTNode* root, HashMap* stringTable)
{
	// gen the lable
    insert_line(".data\n");
    // gen read only vars
    gen_readOnly_data(stringTable);
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
            switch (entry->type)
            {
            case TYPE_INT:
                insert_line("%s dq %s\n",name, node->children[3]->token->lexeme);
                break;
            case TYPE_STRING:
                // strings will refrence the string in the read only segment
                insert_line("%s dq offset %s\n",name,  lookUpString(node->children[3]->token->lexeme, stringTable));
                break;
            case TYPE_BOOL:
                insert_line("%s db %s\n", name, strcmp(node->children[3]->token->lexeme, "true")  == 0 ? "1" : "0");
                break;
            default:
                insert_line("Invalid global var type: %s\n", convertTypeToString(entry->type));
                break;
            }
        }
    }

}

void gen_readOnly_data(HashMap* stringTable) {
    int index;
    for (index = 0; index < stringTable->map_size; index++) {
        if (stringTable->arr[index] != NULL) {
            HashNode* cur = stringTable->arr[index];
            while (cur != NULL) {
                insert_line("%s db %s,0\n",((StringEntry*)cur->value)->label, (char*)cur->key);
                cur = cur->next;
            }
        }
    }
}



/// <summary>
/// This is the main code gen function, it will preform a Postorder traversel
/// </summary>
/// <param name="node"></param>
void gen_code(ASTNode* node, HashMap* stringTable)
{
    if (!node) return;
    // go over children
    for (int i = 0; i < node->childCount; i++) {
        gen_code(node->children[i], stringTable); 
    }
    if (strcmp(node->lable, "FuncDeclaration") == 0) {
        gen_function(node, stringTable);
        return;
    }
}

static int count_local_vars(ASTNode* node)
{
    if (!node) return 0;
    int counter = 0;
    // count local var
    if (strcmp(node->lable, "VarDeclaration") == 0)
        counter++;
    // go over all children
    for (int i = 0; i < node->childCount; i++) {
        counter += count_local_vars(node->children[i]);
    }
    return counter;
}

void gen_function(ASTNode* node, HashMap* stringTable)
{
    if (!node) return;
    // get func name
    const char* funcName = node->children[0]->token->lexeme;
    // start of func
    insert_line("%s Proc\n", funcName);
    insert_line("push rbp\n");
    insert_line("mov rbp, rsp\n");
    ASTNode* blockNode = node->children[3];
    // count the amount of local vars in the func and save space for them
    int localSize = count_local_vars(node) * 8;
    insert_line("sub rsp, %d\n", localSize);
    // gen func body
    gen_statement_list(blockNode->children[0], stringTable);
    // setup return label
    insert_line("_%s_ret:\n", funcName);
    // end of func
    insert_line("add rsp, %d\n", localSize);
    insert_line("pop rbp\n");
    insert_line("ret\n");
    insert_line("%s Endp\n", funcName);
}

void gen_start_label()
{
    insert_line("start:\n");
    insert_line("call Main\n");
    insert_line("mov eax, 0\n");
    insert_line("ret\n");
}

void gen_asm(ASTNode* root, HashMap* stringTable) 
{
    createAsmFile();
    // gen data seg
    gen_data_seg(root, stringTable);
    // gen code seg
    insert_line(".code\n");
    //gen_start_label();
    gen_code(root, stringTable);
    insert_line("END\n");

}