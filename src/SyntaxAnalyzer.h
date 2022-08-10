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
int KeyType;
int ConstType;
char* text;

int ToInt(char* s) {
    int k = 0;
    while(s) {
        k *= 10;
        k += (*s) - 48;
        s++;
    }
    return k;
}

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
    text = (char*) calloc(100,sizeof (char));
    w = GetToken(token_list);
    struct TreeNode* p = ExtDefList(NULL);
    free(text);
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
            w = GetToken(token_list);
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
            w = GetToken(token_list);
            struct TreeNode* p = NewTreeNode();
            return p;
        }
        // 调用表达式处理初始化表达式
        struct TreeNode* p = Expression(SEMI);
        if(p == NULL) {
            printf("Wrong Expression on line: %d\n",w.pre_line);
            return NULL;
        }
        w = GetToken(token_list);
    }
    // 处理行注释和注释块
    while(w.kind == LANNO) {        // 行注释
        char *s = (char*) calloc(100,sizeof (char));
        char *c = (char*) calloc(2,sizeof (char));
        *c = ' ';
        int l = w.line;
        w = GetToken(token_list);
        while(w.line == l) {
            strcat(s, w.text);
            strcat(s, c);
            w = GetToken(token_list);
        }
        free(c);
        free(s);
        struct TreeNode* p = NewTreeNode();
        return p;
    }
    while(w.kind == LBA) {      // 块注释
        char *s = (char*) calloc(1000,sizeof (char));
        char *c = (char*) calloc(2,sizeof (char));
        char *n = (char*) calloc(2,sizeof (char));
        *c = ' ';
        *n = '\n';
        w = GetToken(token_list);
        while(w.kind != RBA) {
            if(w.pre_line < w.line)
                strcat(s, n);
            strcat(s, w.text);
            strcat(s, c);
            w = GetToken(token_list);
        }
        free(s);
        free(c);
        free(n);
        struct TreeNode* p = NewTreeNode();
        return p;
    }
    if(w.kind > IDENT) {
        printf("Not a Key Word on line: %d\n",w.pre_line);
        return NULL;
    }
    // 变量类型
    KeyType = w.kind;
    // 5: const常量
    if(w.kind == 5) {
        w = GetToken(token_list);
        KeyType = w.kind;
        ConstType = 1;
    }
    w = GetToken(token_list);
    if(w.kind != IDENT) {
        printf("Not Identifier on line: %d\n",w.pre_line);
        return NULL;
    }
    // 记录变量名
    strcat(text, w.text);
    struct TreeNode* p = NewTreeNode();
    w = GetToken(token_list);
    // 调用外部变量定义
    if(w.kind != LP)
        p->son[0] = ExtVarDef();
    // 调用函数定义
    else
        p->son[0] = FuncDef();
    if(p->son != NULL)
        return p;
    DeleteTree(p);
        return NULL;
}

// 外部变量定义
struct TreeNode* ExtVarDef() {
    struct TreeNode* root = NewTreeNode();
    root->son[0] = NewTreeNode();
    strcat(root->son[0]->token, kind_name[KeyType - 100]);
    if(ConstType) {
        ConstType = 0;
    }
    root->son[1] = NewTreeNode();
    struct TreeNode* p =root->son[1];
    p->son[0] = NewTreeNode();
    strcat(p->son[0]->token, text);
    *text = 0;
    int flag = 0;
    while(1) {
        if(w.kind == ASSIGN) {                  // 赋值语句 int i = 1;
            w = GetToken(token_list);
            struct TreeNode* q = Sentence();
            if(q == NULL)
                return NULL;
            flag = 1;
        }
        if(w.kind == LK) {                      // 定义数组 int a[1];
            w = GetToken(token_list);
            if(w.kind != INT_CONST) {
                printf("Expected Integer in [] on line: %d\n",w.pre_line);
                return NULL;
            }
            int size = ToInt(w.text);
            w = GetToken(token_list);
            if(w.kind != RK) {
                printf("Expected \']\' after \'[\' on line: %d\n",w.pre_line);
                return NULL;
            }
            w = GetToken(token_list);
        }
        if(!flag && w.kind != SEMI && w.kind != COMMA) {
            printf("Expected \',\' or \';\' on line: %d\n",w.pre_line);
            return NULL;
        }
        if(flag || w.kind == SEMI) {
            if(!flag)
                w = GetToken(token_list);
            return root;
        }
        w = GetToken(token_list);
        if(w.kind != IDENT) {
            printf("Not Identifier ono line: %d\n",w.pre_line);
            return NULL;
        }
        p->son[1] = NewTreeNode();
        p = p->son[1];
        p->son[0] = NewTreeNode();
        strcat(p->son[0]->token, w.text);
        w = GetToken(token_list);
    }
}

// 函数定义
struct TreeNode* FuncDef() {
    
}
#endif //SOURCE_PROGRAM_FORMATTING_SYNTAXANALYZER_H
