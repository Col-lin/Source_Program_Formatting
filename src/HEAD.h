//
// Created by Col_lin on 2022/7/31.
//

#ifndef SOURCE_PROGRAM_FORMATTING_HEAD_H
#define SOURCE_PROGRAM_FORMATTING_HEAD_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CORRECT 0
#define WORD_ERROR 1
#define SYNTAX_ERROR 2
#define SITUATION int
#define MATCHED 1
#define UNMATCHED 0
#define TRUE 1
#define FALSE 0
#define BOOL int

FILE *fil;

/*
    KEY: key word 1~35
    ERROR_TOKEN: error token
	IDENT: identifier
	CHAR_CONST: const & char
	INT_CONST: const & int
	FLOAT_CONST: const & float
	DOUBLE_CONST: const & double
	LONG_CONST: const & long
	STRING_CONST: const & string
	EQUAL: equal ==
	ASSIGN: assign =
	PLUSPLUS: plusplus ++
	PLUS: plus +
	MINUSMINUS: minusminus --
	MINUS: minus -
	MULTIPLY: multiply *
	DIVIDE: divide /
	MOD: mod %
	MOREEQ: more or equal >=
	MORE: more >
	LESSEQ: less or equal <=
	LESS: less <
	NOTEQ: not equal !=
	NOT: not !
	AND: and &&
	OR: or ||
	LP: left parentheses (
	RP: right parentheses )
	SEMI: semicolon ;
	COMMA: comma ,
	LBP: left big bracket {
	RBP: right big bracket }
	LK: [
	RK: ]
	PRE: #
	LANNO: //
	LBA: / *
    RBA: * /
	DOT: .
    eof: end of file
*/
enum token_kind {
    ERROR_TOKEN = 100, IDENT,
    CHAR_CONST, INT_CONST, FLOAT_CONST,
    DOUBLE_CONST, LONG_CONST, STRING_CONST,
    PLUS, MINUS, MULTIPLY, DIVIDE, LP, RP,
    ASSIGN, MORE, LESS, EQUAL, NOTEQ, AND,
    OR, PLUSPLUS, MINUSMINUS, MOD, MOREEQ,
    LESSEQ, NOT, SEMI, COMMA, LBP, RBP, LK,
    RK, PRE, LANNO, LBA, RBA, DOT, eof
};

char kind_name[38][13] ={"ERROR_TOKEN", "IDENT", "CHAR_CONST", "INT_CONST",
                         "FLOAT_CONST", "DOUBLE_CONST", "LONG_CONST",
                         "STRING_CONST", "PLUS", "MINUS", "MULTIPLY",
                         "DIVIDE", "LP", "RP", "ASSIGN", "MORE", "LESS",
                         "EQUAL", "NOTEQ", "AND", "OR", "PLUSPLUS",
                         "MINUSMINUS", "MOD", "MOREEQ", "LESSEQ", "NOT",
                         "SEMI", "COMMA", "LBP", "RBP", "LK", "RK", "PRE",
                         "LANNO", "LBA", "RBA", "DOT"};

enum key_word {
    AUTO = 1, BREAK, CASE, CHAR, CONST, CONTINUE,
    DEFAULT, DEFINE, DO, DOUBLE, ELSE, ENUM,
    EXTERN, FLOAT, FOR, GOTO, IF, INCLUDE,
    INLINE, INT, LONG, REGISTER, RETURN, SHORT,
    SIGNED, SIZEOF, STATIC, STRUCT, SWITCH, TYPEDEF,
    UNION, UNSIGNED, VOID, VOLATILE, WHILE
};

int error_count = 0;

struct TreeNode{
    struct TreeNode * son[4];
    char *token;
};

struct TreeNode* NewTreeNode() {
    struct TreeNode* p = (struct TreeNode*) malloc(sizeof (struct TreeNode));
    for(int i=0; i<4; ++i)
        p->son[i] = NULL;
    p->token = calloc(32,sizeof (char));
    return p;
}

struct tree{
    struct tree *lson;
    struct tree *rson;
};

struct WORD {
    int kind;
    char *text;
    int line;
    int pre_line;
};

struct Node{
    struct Node* next;
    struct WORD word;
};

struct List{
    struct Node* next;
    int size;
    struct Node* last_read;
};

struct Stack{
    struct Node* next;
    int size;
    struct Node* top;
};

struct List *token_list;

struct TreeNode* Program();
//程序分析
struct TreeNode* ExtDefList();
//外部定义序列
struct TreeNode* ExtDef();
//外部定义
struct TreeNode* ExtVarDef();
//外部变量定义
struct TreeNode* FuncDef();
//函数定义
struct TreeNode* FormPara();
//形式参数序列
struct TreeNode* Compound();
//复合语句
struct TreeNode* LocalVar();
//局部变量定义
struct TreeNode* SentenList();
//语句序列
struct TreeNode* Sentence();
//语句分析
struct TreeNode* Expression(int end);
//表达式分析
#endif //SOURCE_PROGRAM_FORMATTING_HEAD_H
