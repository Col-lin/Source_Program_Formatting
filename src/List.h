//
// Created by Col_lin on 2022/8/6.
//

#ifndef SOURCE_PROGRAM_FORMATTING_LIST_H
#define SOURCE_PROGRAM_FORMATTING_LIST_H

#include "HEAD.h"

struct List* CreatList() {
    struct List* l = (struct List*) malloc(sizeof (struct List));
    l->next = NULL;
    l->last_read = NULL;
    l->size = 0;
    return l;
}

void AddList(struct List* l, struct WORD w) {
    ++(l->size);
    if(l->size == 1) {
        l->next = (struct Node*) malloc(sizeof (struct Node));
        (l->next)->next = NULL;
        (l->next)->word = w;
        return;
    }
    struct Node* p = l->next;
    while(p->next != NULL)
        p = p->next;
    p->next = (struct Node*) malloc(sizeof (struct Node));
    (p->next)->next = NULL;
    (p->next)->word = w;
    return;
}

struct WORD GetToken(struct List* l) {
    struct Node* p;
    if(l->last_read == NULL)
        p = l->next;
    else p = (l->last_read)->next;
    struct WORD w;
    if(p == NULL)
        w.kind = eof;
    else {
        w = p->word;
        l->last_read = p;
    }
    return w;
}

int Find(struct List* l,char *s) {
    struct Node* p = l->next;
    while(p!=NULL) {
        if(strcmp((p->word).text, s) == 0)
            return MATCHED;
        p = p->next;
    }
    return UNMATCHED;
}

#endif //SOURCE_PROGRAM_FORMATTING_LIST_H
