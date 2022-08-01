//
// Created by Col_lin on 2022/7/31.
//

#ifndef SOURCE_PROGRAM_FORMATTING_WORDDETECT_H
#define SOURCE_PROGRAM_FORMATTING_WORDDETECT_H

#include "HEAD.h"
#include "MATCH.h"

enum token_kind {
    ERROR_TOKEN = 100, eof, IDENT,
    CHAR_CONST, INT_CONST, FLOAT_CONST,
    DOUBLE_CONST, LONG_CONST, STRING_CONST,
    PLUS, MINUS, MULTIPLY, DIVIDE, LP, RP,
    ASSIGN, MORE, LESS, EQUAL, NOTEQ, AND,
    OR, PLUSPLUS, MINUSMINUS, MOD, MOREEQ,
    LESSEQ, notEQ, NOT, SEMI, COMMA, LBP,
    RBP, LK, RK, PRE, LANNO, BANNO, DOT};
/*
    KEY: key word 1~35
    ERROR_TOKEN: error token
	eof: end of file
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
	BANNO: /* or * /
	DOT: .
*/

struct WORD {
    int kind;
    char *text;
    int line;
    int pre_line;
};

BOOL isalpha(char c) {
    if(c >= 'a' && c <= 'z')
        return TRUE;
    if(c >= 'A' && c <= 'Z')
        return TRUE;
    return FALSE;
}

BOOL isnumber(char c) {
    if(c >= '0' && c <= '9')
        return TRUE;
    return FALSE;
}

int error_count = 0;

SITUATION WordDetect(FILE *fp) {
    char *c = (char *)malloc(2*sizeof(char));
    *(c+1) = 0;
    *c = getc(fp);
    struct WORD w;
    w.text = (char *)malloc(32*sizeof(char));
    int line_count = 1;
    list_build();
    while(*c != EOF) {
        if(*c == ' '||*c == '\t')
            continue;
//        w.pre_line = line_count;    //line of the last word
        if(*c == '\n') {
            ++line_count;       //count the lines
            continue;
        }
        w.line = line_count;    //line of the word
        if(isalpha(*c) == TRUE) {    //IDENT or KEY
            *c = getc(fp);
            while(*c == '_' || isalpha(*c)) {
                strcat(w.text, c);
                *c = getc(fp);
            }
            ungetc(*c, fp);
            SITUATION type = KeyWordMatching(w.text);
            if(type == unmatched)
                w.kind = IDENT;
            else w.kind = type;
        } else if(isnumber(*c) == TRUE) {   //const number
            w.text = strcat(w.text, c);
            *c = getc(fp);
            if(isalpha(*c) && *c != 'x' && *c !='X') {
                strcat(w.text, c);
                w.kind = ERROR_TOKEN;
                error_count++;
                continue;
            }
            int flag = 0, hexflag = 0;
            while(*c == '.' || isnumber(*c) || *c == 'x' || *c == 'X') {
                if(*c == '.' && flag == 0) { //'.' should be counted less then once
                    strcat(w.text, c);
                    flag = 1;
                } else if(isnumber(*c)) {
                    strcat(w.text, c);
                } else if(*w.text == '0' && (*c == 'x' || *c == 'X') && hexflag == 0) {
                    strcat(w.text, c);
                    hexflag = 1;
                } else {
                    w.kind =ERROR_TOKEN;
                    error_count++;
                    break;
                }
                c = getc(fp);
            }
            ungetc(*c,fp);
        } else {

        }
    }
    return CORRECT;
}

#endif //SOURCE_PROGRAM_FORMATTING_WORDDETECT_H
