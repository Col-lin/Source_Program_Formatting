//
// Created by Col_lin on 2022/8/1.
//

#ifndef SOURCE_PROGRAM_FORMATTING_MATCH_H
#define SOURCE_PROGRAM_FORMATTING_MATCH_H

#include "HEAD.h"

char key_name[36][10] = {"", "auto", "break", "case", "char",
                         "const", "continue", "default", "define",
                         "do", "double", "else", "enum", "extern",
                         "float", "for", "goto", "if", "include",
                         "inline", "int", "long", "register", "return",
                         "short", "signed", "sizeof", "static", "struct",
                         "switch", "typedef", "union", "unsigned",
                         "void", "volatile", "while",};

int KeyWordMatching(char *s) {
    for(int i=1; i<=35; ++i)
        if(strcmp(s, key_name[i]) == 0) return i;
    return UNMATCHED;
}

#endif //SOURCE_PROGRAM_FORMATTING_MATCH_H
