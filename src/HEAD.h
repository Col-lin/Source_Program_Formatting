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
#define unmatched 0
#define TRUE 1
#define FALSE 0
#define BOOL int

struct TreeNode{
    struct TreeNode * son[4] = NULL;
    char *token;
};

struct tree{
    struct tree *lson = NULL;
    struct tree *rson = NULL;
};

#endif //SOURCE_PROGRAM_FORMATTING_HEAD_H
