//
// Created by Col_lin on 2022/8/19.
//

#ifndef SOURCE_PROGRAM_FORMATTING_STACK_H
#define SOURCE_PROGRAM_FORMATTING_STACK_H

#include "HEAD.h"

struct Stack* CreatStack() {
    struct Stack* s = (struct Stack*) calloc(1,sizeof (struct Stack));
    s->next = NULL;
    s->size = 0;
    s->top = NULL;
}

void StackInsert() {

}

#endif //SOURCE_PROGRAM_FORMATTING_STACK_H
