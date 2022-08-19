//
// Created by Col_lin on 2022/8/4.
//

#ifndef SOURCE_PROGRAM_FORMATTING_SYNTAXANALYZER_H
#define SOURCE_PROGRAM_FORMATTING_SYNTAXANALYZER_H

#include "HEAD.h"
#include "WordAnalyzer.h"
#include "Stack.h"
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
    struct TreeNode* p = ExtDefList();
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
        if(w.kind != DEFINE && w.kind != INCLUDE) {
            printf("Precompiled Error on line: %d\n", w.pre_line);
            return NULL;
        }
        if(w.kind == DEFINE) {      // define
            w = GetToken(token_list);
            if(w.kind != IDENT) {
                printf("Not a Identifier on line: %d\n",w.pre_line);
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
                printf("Not a Identifier on line: %d\n",w.pre_line);
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
                    printf("Not a Identifier on line: %d\n",w.pre_line);
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
        printf("Not a Identifier on line: %d\n",w.pre_line);
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
    if(p->son[0] != NULL)
        return p;
    DeleteTree(p);
        return NULL;
}

// 外部变量定义
struct TreeNode* ExtVarDef() {
    struct TreeNode* root = NewTreeNode();
    root->son[0] = NewTreeNode();
    strcat(root->son[0]->token, key_name[KeyType]);
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
            if(q == NULL) {
                DeleteTree(root);
                return NULL;
            }
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
                DeleteTree(root);
                return NULL;
            }
            w = GetToken(token_list);
        }
        if(!flag && w.kind != SEMI && w.kind != COMMA) {
            printf("Expected \',\' or \';\' on line: %d\n",w.pre_line);
            DeleteTree(root);
            return NULL;
        }
        if(flag || w.kind == SEMI) {
            if(!flag)
                w = GetToken(token_list);
            return root;
        }
        w = GetToken(token_list);
        if(w.kind != IDENT) {
            printf("Not a Identifier ono line: %d\n",w.pre_line);
            DeleteTree(root);
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
    struct TreeNode* root = NewTreeNode();
    root->son[0] = NewTreeNode();
    strcat(root->son[0]->token, key_name[KeyType]);
    root->son[1] = NewTreeNode();
    strcat(root->son[1]->token, text);
    // 记录已定义函数
    struct WORD wd;
    wd.text = (char*) calloc(32,sizeof (char));
    strcat(wd.text,text);
    wd.kind = KeyType;
    AddList(func, wd);
    // 调用形参
    root->son[1]->son[0] = FormPara();
    if(root->son[1]->son[0] == NULL) {
        DeleteTree(root);
        return NULL;
    }
    w = GetToken(token_list);
    struct TreeNode* p = NULL;
    // 注释
    char *s = (char*) calloc(1000,sizeof (char));
    char *c = (char*) calloc(2,sizeof (char));
    char *n = (char*) calloc(2,sizeof (char));
    *c = ' ';
    *n = '\n';
    if(w.kind == LANNO) {
        w = GetToken(token_list);
        while(w.pre_line == w.line) {
            strcat(s, w.text);
            strcat(s, c);
            w = GetToken(token_list);
        }
    } else if(w.kind == LBA) {
        w = GetToken(token_list);
        while(w.kind != RBA) {
            if(w.pre_line < w.line)
                strcat(s, n);
            strcat(s, w.text);
            strcat(s, c);
        }
    }
    free(s);
    free(c);
    free(n);
    // 函数声明
    if(w.kind == SEMI) {
        w = GetToken(token_list);
        return root;
    } else if(w.kind == LBP) {          //函数定义
        root->son[2] = Compound();
        if(root->son[2] == NULL) {
            DeleteTree(root);
            return NULL;
        }
    } else {
        printf("Wrong Syntax on line: %d\n",w.pre_line);
        DeleteTree(root);
        return NULL;
    }
    return root;
}

// 形式参数序列
struct TreeNode* FormPara() {
    static int CommaFlag = 0;
    w = GetToken(token_list);
    struct TreeNode* root = NewTreeNode();
    struct TreeNode* p = root;
    if(w.kind == RP) {
        if(CommaFlag) {
            CommaFlag = 0;
            printf("Expected Parameter on line: %d\n",w.pre_line);
            DeleteTree(root);
            return NULL;
        } else {
            return root;
        }
    } else if(w.kind < 100) {
        CommaFlag = 0;
        if(w.kind == 5) {       // 5: const 常量
            w = GetToken(token_list);
            if(w.kind >= 100) {
                printf("Not a Key Word on line: %d\n",w.pre_line);
                DeleteTree(root);
                return NULL;
            }
        }
        p->son[0] = NewTreeNode();
        p->son[0]->son[0] = NewTreeNode();
        strcat(p->son[0]->son[0]->token, w.text);
        w = GetToken(token_list);
        if(w.kind != IDENT) {
            printf("Not a Identifier on line: %d\n",w.pre_line);
            DeleteTree(root);
            return NULL;
        }
        p->son[0]->son[1] = NewTreeNode();
        strcat(p->son[0]->son[1]->token, w.text);
        w = GetToken(token_list);
        if(w.kind == RP) {
            p->son[1] = NULL;
            return root;
        }
        if(w.kind != COMMA) {
            printf("Expected \',\' on line: %d\n",w.pre_line);
            DeleteTree(root);
            return NULL;
        }
        CommaFlag = 1;
        p->son[1] = FormPara();
        if(p->son[1] == NULL) {
            DeleteTree(root);
            return NULL;
        }
    } else {
        CommaFlag = 0;
        printf("Not a Key Word on line: %d\n",w.pre_line);
        DeleteTree(root);
        return NULL;
    }
    return root;
}

//复合语句
struct TreeNode* Compound() {
    struct TreeNode* p = NewTreeNode();
    w = GetToken(token_list);
    if(w.kind == 5) {           // const 常量
        ConstType = 1;
        w = GetToken(token_list);
    }
    if(w.kind == CHAR || w.kind == INT || w.kind == FLOAT || w.kind == SHORT || w.kind == DOUBLE || w.kind == LONG) {
        KeyType = w.kind;
        p->son[0] = LocalVar();
    } else {
        p->son[0] = NULL;
    }
    // 调用语句序列
    p->son[1] = SentenList();
    if(p->son[1] == NULL) {
        DeleteTree(p);
        return NULL;
    }
    if(w.kind != RBP) {
        printf("Invalid Syntax on line: %d\n",w.pre_line);
        DeleteTree(p);
        return NULL;
    }
    w = GetToken(token_list);
    return p;
}

// 局部变量定义
struct TreeNode* LocalVar() {
    struct TreeNode* p = NewTreeNode();
    p->son[0] = NewTreeNode();
    p->son[0]->son[0] = NewTreeNode();
    strcat(p->son[0]->son[0]->token, key_name[KeyType]);
    if(ConstType) {
        ConstType = 0;
    }
    w = GetToken(token_list);
    if(w.kind != IDENT) {
        printf("Not a Identifier on line: %d\n",w.pre_line);
        DeleteTree(p);
        return NULL;
    }
    p->son[0]->son[1] = NewTreeNode();
    strcat(p->son[0]->son[1]->token, w.text);
    w = GetToken(token_list);
    int flag = 0;
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
            DeleteTree(p);
            return NULL;
        }
        w = GetToken(token_list);
    }
    if(!flag && w.kind != SEMI && w.kind != COMMA) {
        printf("Expected \',\' or \';\' on line: %d\n",w.pre_line);
        DeleteTree(p);
        return NULL;
    }
    if(flag || w.kind == SEMI) {
        if(!flag)
            w = GetToken(token_list);
        return p;
    }
    // 递归调用局部变量函数
    p->son[1] = LocalVar();
    if(p->son[1] == NULL) {
        DeleteTree(p);
        return NULL;
    }
    return p;
}

// 语句序列
struct TreeNode* SentenList() {
    struct TreeNode* p = Sentence();
    if(p == NULL) {
        if(error_count > 0) {
            printf("Invalid Syntax on line: %d\n",w.pre_line);
            DeleteTree(p);
            return NULL;
        } else {
            return p;
        }
    } else {
        struct TreeNode* q = NewTreeNode();
        q->son[0] = p;
        q->son[1] = SentenList();
        if(q->son[1] == NULL && error_count > 0) {
            DeleteTree(q);
            return NULL;
        } else {
            return q;
        }
    }
}

// 语句分析
struct TreeNode* Sentence() {
    struct TreeNode* root = NULL;
    struct TreeNode* p = NULL;
    struct TreeNode* q = NULL;
    struct TreeNode* r = NULL;
    struct TreeNode* s = NULL;
    char* str = NULL;
    char* c = NULL;
    switch (w.kind) {
        case IF:
            w = GetToken(token_list);
            if(w.kind != LP) {
                printf("Invalid Syntax on line: %d\n", w.pre_line);
                return NULL;
            }
            w = GetToken(token_list);
            p = Expression(RP);         // 条件表达式
            if(p == NULL) {
                printf("Wrong Expression on line: %d\n",w.pre_line);
                return NULL;
            }
            w = GetToken(token_list);
            q = Sentence();
            if(q == NULL) {
                return NULL;
            }
            if(w.kind == ELSE) {
                w = GetToken(token_list);
                r = Sentence();
                root = NewTreeNode();
                root->son[0] = p;
                root->son[1] = q;
                root->son[2] = r;
                return root;
            } else {
                root = NewTreeNode();
                root->son[0] = p;
                root->son[1] = q;
                return root;
            }
        case LBP:               // {
            return Compound();
        case WHILE:
            w = GetToken(token_list);
            if(w.kind != LP) {
                printf("Invalid Syntax on line: %d\n",w.pre_line);
                return NULL;
            }
            w = GetToken(token_list);
            p = Expression(RP);         // 条件表达式
            if(p == NULL) {
                printf("Wrong Expression on line: %d\n",w.pre_line);
                return NULL;
            }
            w = GetToken(token_list);
            q = Sentence();
            if(q == NULL) {
                return NULL;
            }
            root = NewTreeNode();
            root->son[0] = p;
            root->son[1] = q;
            return root;
        case FOR:
            w = GetToken(token_list);
            if(w.kind != LP) {
                printf("Invalid Syntax on line: %d\n",w.pre_line);
                return NULL;
            }
            w = GetToken(token_list);
            p = Expression(SEMI);           // 初始赋值表达式
            if(p == NULL) {
                printf("Wrong Expression on line: %d\n",w.pre_line);
                return NULL;
            }
            w = GetToken(token_list);
            q = Expression(SEMI);           // 条件表达式
            if(q == NULL) {
                printf("Wrong Expression on line: %d\n",w.pre_line);
                return NULL;
            }
            w = GetToken(token_list);
            r = Expression(RP);               // 循环后表达式
            if(r == NULL) {
                printf("Wrong Expression on line: %d\n",w.pre_line);
                return NULL;
            }
            w = GetToken(token_list);
            s = Sentence();                     //for子句
            if(s == NULL) {
                return NULL;
            }
            root = NewTreeNode();
            root->son[0] = p;
            root->son[1] = q;
            root->son[2] = r;
            root->son[3] = s;
            return root;
        case RETURN:
            w = GetToken(token_list);
            p = Expression(SEMI);
            if(p == NULL) {
                printf("Wrong Expression on line: %d\n",w.pre_line);
                return NULL;
            }
            root = NewTreeNode();
            root->son[0] = p;
            w = GetToken(token_list);
            return root;
        case BREAK:
            w = GetToken(token_list);
            if(w.kind != SEMI) {
                printf("Expected \';\' on line: %d\n",w.pre_line);
                return NULL;
            }
            root = NewTreeNode();
            w = GetToken(token_list);
            return root;
        case CONTINUE:
            w = GetToken(token_list);
            if(w.kind != SEMI) {
                printf("Expected \';\' on line: %d\n",w.pre_line);
                return NULL;
            }
            root = NewTreeNode();
            w = GetToken(token_list);
            return root;
        case DO:                            // do while
            w = GetToken(token_list);
            p = Sentence();
            if(p == NULL) {
                return NULL;
            }
            if(w.kind != WHILE) {
                printf("Expected \"while\" on line: %d\n",w.pre_line);
                return NULL;
            }
            w = GetToken(token_list);
            q = Sentence();
            if(q == NULL) {
                return NULL;
            }
            w = GetToken(token_list);
            if(w.kind != SEMI) {
                printf("Expected \';\' on line: %d\n",w.pre_line);
                return NULL;                
            }
            root = NewTreeNode();
            root->son[0] = p;
            root->son[1] = q;
            w = GetToken(token_list);
            return root;
        case LP:
            w = GetToken(token_list);
            p = Expression(RP);
            if(p == NULL) {
                printf("Wrong Expression on line: %d\n",w.pre_line);
                return NULL;
            }
            root = NewTreeNode();
            root->son[0] = p;
            return root;
        case PLUSPLUS:
        case MINUSMINUS:
        case CHAR_CONST:
        case INT_CONST:
        case FLOAT_CONST:
        case DOUBLE_CONST:
        case LONG_CONST:
        case STRING_CONST:
        case IDENT:
            if(Find(func, w.text) == MATCHED) {
                w = GetToken(token_list);
                if(w.kind != LP) {
                    printf("Expected \'(\' on line: %d\n",w.pre_line);
                    return NULL;
                }
                while(w.kind != RP) {
                    w = GetToken(token_list);
                    if(w.kind != IDENT) {
                        printf("Not a Identifier on line: %d\n",w.pre_line);
                        return NULL;
                    }
                    w = GetToken(token_list);
                    if(w.kind != RP && w.kind != COMMA) {
                        printf("Wrong Syntax on line: %d\n",w.pre_line);
                        return NULL;
                    }
                }
                w = GetToken(token_list);
                if(w.kind != SEMI) {
                    printf("Expected \';\' on line: %d\n",w.pre_line);
                    return NULL;
                }
                root = NewTreeNode();
                w = GetToken(token_list);
                return root;
            }
            p = Expression(SEMI);
            if(p == NULL) {
                printf("Wrong Expression on line: %d]\n",w.pre_line);
                return NULL;
            }
            root = NewTreeNode();
            root->son[0] = p;
            w = GetToken(token_list);
            return root;
        case INT:
        case SHORT:
        case LONG:
        case CHAR:
        case DOUBLE:
        case FLOAT:
        case CONST:
            if(w.kind == CONST) {
                w = GetToken(token_list);
                if(w.kind != INT && w.kind != SHORT && w.kind != LONG
                && w.kind != CHAR && w.kind != DOUBLE && w.kind != FLOAT) {
                    printf("Wrong Syntax on line: %d\n",w.pre_line);
                    return NULL;
                }
            }
            root = LocalVar();
            return root;
        case PRE:
            w = GetToken(token_list);
            if(w.kind != DEFINE && w.kind != INCLUDE) {
                printf("Precompiled Error on line: %d\n", w.pre_line);
                return NULL;
            }
            if(w.kind == DEFINE) {      // define
                w = GetToken(token_list);
                if(w.kind != IDENT) {
                    printf("Not a Identifier on line: %d\n",w.pre_line);
                    return NULL;
                }
                w = GetToken(token_list);
                if(w.kind != INT_CONST && w.kind != DOUBLE_CONST) {
                    printf("Precomplied Error on line: %d\n",w.pre_line);
                    return NULL;
                }
                root = NewTreeNode();
                w = GetToken(token_list);
                return root;
            } else {                // include
                w = GetToken(token_list);
                if(w.kind != LESS){
                    printf("Precomplied Error on line: %d\n",w.pre_line);
                    return NULL;
                }
                str = (char*) calloc(32,sizeof (char));
                w = GetToken(token_list);       //文件名
                strcat(str, w.text);
                w = GetToken(token_list);       // .
                strcat(str, w.text);
                if(w.kind != DOT) {
                    printf("Precomplied Error on line: %d\n",w.pre_line);
                    free(str);
                    return NULL;
                }
                w = GetToken(token_list);       //拓展名
                strcat(str, w.text);
                w = GetToken(token_list);
                if(w.kind != MORE) {
                    printf("Precomplied Error on line: %d\n",w.pre_line);
                    free(str);
                    return NULL;
                }
                root = NewTreeNode();
                w = GetToken(token_list);
                return root;
            }
        case LANNO:
            str = (char*) calloc(100,sizeof (char));
            c = (char*) calloc(2,sizeof (char));
            *c = ' ';
            int l = w.line;
            w = GetToken(token_list);
            while(w.line == l) {
                strcat(str, w.text);
                strcat(str, c);
                w = GetToken(token_list);
            }
            free(c);
            free(str);
            root = NewTreeNode();
            return root;
        case LBA:
            str = (char*) calloc(1000,sizeof (char));
            c = (char*) calloc(2,sizeof (char));
            char *n = (char*) calloc(2,sizeof (char));
            *c = ' ';
            *n = '\n';
            w = GetToken(token_list);
            while(w.kind != RBA) {
                if(w.pre_line < w.line)
                    strcat(str, n);
                strcat(str, w.text);
                strcat(str, c);
                w = GetToken(token_list);
            }
            free(str);
            free(c);
            free(n);
            root = NewTreeNode();
            return root;
        default:
            error_count++;
            printf("Wrong Syntax on line: %d\n",w.pre_line);
            return NULL;
    }
}

// 表达式分析
struct TreeNode* Expression(int end) {

    return NULL;
}
#endif //SOURCE_PROGRAM_FORMATTING_SYNTAXANALYZER_H
