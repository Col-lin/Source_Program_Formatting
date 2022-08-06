//
// Created by Col_lin on 2022/8/4.
//

#ifndef SOURCE_PROGRAM_FORMATTING_SYNTAXANALYZER_H
#define SOURCE_PROGRAM_FORMATTING_SYNTAXANALYZER_H

#include "HEAD.h"
#include "WordAnalyzer.h"
#include "AST.h"

struct WORD w;

SITUATION SyntaxAnalyzer() {
    if(Program(NULL) == NULL) return SYNTAX_ERROR;
    return CORRECT;
}

//程序分析
struct TreeNode* Program() {
    if(cnt_lbp != cnt_rbp) {
        printf("Missing \'{\' or \'}\'\n");
        return NULL;
    }
    if(cnt_lba != cnt_rba) {
        printf("Missing \'/*\' or \'*/\'\n");
        return NULL;
    }
    if(cnt_lp != cnt_rp) {
        printf("Missing \'(\' or \')\'\n");
        return NULL;
    }
    if(w.kind == eof) {
        printf("EOF\n");
        return NULL;
    }
    w = GetToken(token_list);
    struct TreeNode* p = ExtDefList(NULL);
    if(p != NULL || w.kind == eof)
        return p;
    return NULL;
}

//外部定义序列
struct TreeNode* ExtDefList() {
    struct TreeNode* p = NewTreeNode();
    p->son[0] = ExtDef();
    if(p->son[0] == NULL) {
        DeleteTree(p);
        return NULL;
    }
    p->son[1] = ExtDefList();
    if(p->son[1] == NULL) {
        DeleteTree(p);
        return NULL;
    }
    return p;
}

//外部定义
struct TreeNode* Extdef() {
    struct TreeNode* p =NULL;
    while(w.kind == PRE) {          // #
        w = GetToken(token_list);
        if(w.kind != 8 && w.kind != 18) {    //8:define 18:include
            printf("Precompiled Error on line: %d\n", w.line);
            return NULL;
        }
        if(w.kind == 8) {           // define
            w = GetToken(token_list);
            if(w.kind != IDENT) {
                printf("Not Identifier on line: %d\n",w.line);
                return NULL;
            }
        }
    }
    return p;
}
#endif //SOURCE_PROGRAM_FORMATTING_SYNTAXANALYZER_H
