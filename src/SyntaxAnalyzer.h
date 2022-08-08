//
// Created by Col_lin on 2022/8/4.
//

#ifndef SOURCE_PROGRAM_FORMATTING_SYNTAXANALYZER_H
#define SOURCE_PROGRAM_FORMATTING_SYNTAXANALYZER_H

#include "HEAD.h"
#include "WordAnalyzer.h"
#include "AST.h"

struct WORD w;
struct List* func;

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
    func = CreatList();
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
struct TreeNode* ExtDef() {
    struct TreeNode* p =NULL;
    //处理预编译部分
    while(w.kind == PRE) {          // #
        w = GetToken(token_list);
        if(w.kind != 8 && w.kind != 18) {    //8:define 18:include
            printf("Precompiled Error on line: %d\n", w.pre_line);
            return NULL;
        }
        if(w.kind == 8) {           // define
            w = GetToken(token_list);
            if(w.kind != IDENT) {
                printf("Not Identifier on line: %d\n",w.pre_line);
                return NULL;
            }
            w = GetToken(token_list);
            if(w.kind != INT_CONST && w.kind != DOUBLE_CONST) {
                printf("Precomplied Error on line: %d\n",w.pre_line);
                return NULL;
            }
            struct TreeNode *p = NewTreeNode();
            w = GetToken(token_list);
            return p;
        } else {                // include
            w = GetToken(token_list);
            if(w.kind != LESS){
                printf("Precomplied Error on line: %d\n",w.pre_line);
                return NULL;
            }
            char* s = (char*) calloc(32,sizeof (char));
            w = GetToken(token_list);       //文件名
            strcat(s, w.text);
            w = GetToken(token_list);       // .
            strcat(s, w.text);
            if(w.kind != DOT) {
                printf("Precomplied Error on line: %d\n",w.pre_line);
                free(s);
                return NULL;
            }
            w = GetToken(token_list);       //拓展名
            strcat(s, w.text);
            w = GetToken(token_list);
            if(w.kind != MORE) {
                printf("Precomplied Error on line: %d\n",w.pre_line);
                free(s);
                return NULL;
            }
            struct TreeNode* p = NewTreeNode();
            return p;
        }
    }
    //处理函数调用或变量初始化
    while(w.kind == IDENT) {
        //函数调用，查询函数是否已经被定义
        if(Find(func,w.text) == MATCHED) {
            w = GetToken(token_list);
            if(w.kind != LP) {
                printf("Expected \'(\' on line: %d\n",w.pre_line);
                return NULL;
            }
            w = GetToken(token_list);
            if(w.kind != RP && w.kind != IDENT) {
                printf("Not Identifier on line: %d\n",w.pre_line);
                return NULL;
            }
            while(w.kind != RP) {
                w = GetToken(token_list);
                if(w.kind == RP)
                    break;
                if(w.kind != COMMA) {        // , 分隔函数参数
                    printf("Expected \',\' on line: %d\n",w.pre_line);
                    return NULL;
                }
                w = GetToken(token_list);
                if(w.kind != IDENT) {
                    printf("Not Identifier on line: %d\n",w.pre_line);
                    return NULL;
                }
            }
            w = GetToken(token_list);
            if(w.kind != SEMI) {            // ; 语句结尾
                printf("Expected \';\' on line: %d\n",w.pre_line);
                return NULL;
            }
            struct TreeNode* p = NewTreeNode();
            return p;
        }
    }
    return p;
}

// 表达式分析
struct TreeNode* Expression() {
    //处理函数调用

}
#endif //SOURCE_PROGRAM_FORMATTING_SYNTAXANALYZER_H
