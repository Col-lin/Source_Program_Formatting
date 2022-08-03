//
// Created by Col_lin on 2022/7/31.
//

#ifndef SOURCE_PROGRAM_FORMATTING_WORDANALYZER_H
#define SOURCE_PROGRAM_FORMATTING_WORDANALYZER_H

#include "HEAD.h"
#include "MATCH.h"

enum token_kind {
    ERROR_TOKEN = 100, IDENT,
    CHAR_CONST, INT_CONST, FLOAT_CONST,
    DOUBLE_CONST, LONG_CONST, STRING_CONST,
    PLUS, MINUS, MULTIPLY, DIVIDE, LP, RP,
    ASSIGN, MORE, LESS, EQUAL, NOTEQ, AND,
    OR, PLUSPLUS, MINUSMINUS, MOD, MOREEQ,
    LESSEQ, NOT, SEMI, COMMA, LBP,
    RBP, LK, RK, PRE, LANNO, LBA, RBA, DOT};
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
	LBA: /*
    RBA: * /
	DOT: .
*/

struct WORD {
    int kind;
    char *text;
    int line;
    int pre_line;
};

int cnt_lp = 0;     //count (
int cnt_rp = 0;     //count )
int cnt_lbp = 0;    //count {
int cnt_rbp = 0;    //count }
int cnt_lba = 0;    //count /*
int cnt_rba = 0;    //count */

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

char kind_name[38][13] ={"ERROR_TOKEN", "IDENT", "CHAR_CONST", "INT_CONST",
                            "FLOAT_CONST", "DOUBLE_CONST", "LONG_CONST",
                            "STRING_CONST", "PLUS", "MINUS", "MULTIPLY",
                            "DIVIDE", "LP", "RP", "ASSIGN", "MORE", "LESS",
                            "EQUAL", "NOTEQ", "AND", "OR", "PLUSPLUS",
                            "MINUSMINUS", "MOD", "MOREEQ", "LESSEQ", "NOT",
                            "SEMI", "COMMA", "LBP", "RBP", "LK", "RK", "PRE",
                            "LANNO", "LBA", "RBA", "DOT"};

void AnalysisCompleted(struct WORD w) {
    printf("%s    %s\n",kind_name[w.kind - 100],w.text);
    return;
}

SITUATION WordAnalysis(FILE *fp) {
    char *c = (char *)malloc(2*sizeof(char));
    *(c+1) = 0;
    *c = getc(fp);
    struct WORD w;
    w.text = (char *)malloc(32*sizeof(char));
    int line_count = 1;
    list_build();
    while(*c != EOF) {
        *w.text = 0;
        w.kind = ERROR_TOKEN;
        if(*c == ' '||*c == '\t') {
            *c = getc(fp);
            continue;
        }
//        w.pre_line = line_count;    //line of the last word
        if(*c == '\n') {
            ++line_count;       //count the lines
            *c = getc(fp);
            continue;
        }
        w.line = line_count;    //line of the word
        w.text = strcat(w.text, c);
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
            *c = getc(fp);
            if(isalpha(*c) && *c != 'x' && *c !='X') {
                strcat(w.text, c);
                w.kind = ERROR_TOKEN;
                error_count++;
                AnalysisCompleted(w);
                *c = getc(fp);
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
                } else
                    break;
                *c = getc(fp);
            }
            if(*(w.text+1) == 'x' || *(w.text+1) == 'X') {
                *c = getc(fp);
                if(strlen(w.text) > 2) {
                    w.kind = INT_CONST;
                    AnalysisCompleted(w);
                    *c = getc(fp);
                    continue;
                } else {
                    w.kind = ERROR_TOKEN;
                    error_count++;
                    AnalysisCompleted(w);
                    *c = getc(fp);
                    continue;
                }
            }
            if((*c == 'l' || *c == 'L') && !flag) {
                strcat(w.text, c);
                w.kind = LONG_CONST;
                AnalysisCompleted(w);
                *c = getc(fp);
                continue;
            }
            if((*c == 'f' || *c == 'F') && flag) {
                strcat(w.text, c);
                w.kind = FLOAT_CONST;
                AnalysisCompleted(w);
                *c = getc(fp);
                continue;
            }
            ungetc(*c, fp);
            if(flag) {
                w.kind = DOUBLE_CONST;
                AnalysisCompleted(w);
                *c = getc(fp);
                continue;
            } else {
                w.kind = INT_CONST;
                AnalysisCompleted(w);
                *c = getc(fp);
                continue;
            }
        } else {                        //other cases
//            printf("%s\n",w.text);
            switch(*c) {
                case '=':
                    *c = getc(fp);
                    if(*c == '=') {         // ==
                        w.kind = EQUAL;
                        strcat(w.text, c);
                    } else {                // =
                        ungetc(*c, fp);
                        w.kind = ASSIGN;
                    }
                    break;
                case '+':
                    *c = getc(fp);
                    if(*c == '+') {         // ++
                        w.kind = PLUSPLUS;
                        strcat(w.text, c);
                    } else {                // +
                        ungetc(*c, fp);
                        w.kind = PLUS;
                    }
                    break;
                case '-':
                    *c = getc(fp);
                    if(*c == '-') {     // --
                        w.kind = MINUSMINUS;
                        strcat(w.text, c);
                    } else {            // -
                        ungetc(*c, fp);
                        w.kind = MINUS;
                    }
                    break;
                case '*':
                    *c = getc(fp);
                    if(*c == '/') {     // */
                        ++cnt_rba;
                        strcat(w.text, c);
                        w.kind = RBA;
                    } else {            // *
                        ungetc(*c, fp);
                        w.kind = MULTIPLY;
                    }
                    break;
                case '/':
                    *c = getc(fp);
                    if(*c == '/') {     // //
                        strcat(w.text, c);
                        w.kind = LANNO;
                    } else if(*c == '*') {  // /*
                        ++cnt_lba;
                        strcat(w.text, c);
                        w.kind = LBA;
                    } else {
                        ungetc(*c, fp);
                        w.kind = DIVIDE;
                    }
                    break;
                case '%':
                    w.kind = MOD;
                    break;
                case '>':
                    *c = getc(fp);
                    if(*c == '=') {     // >=
                        strcat(w.text, c);
                        w.kind = MOREEQ;
                    } else {            // >
                        ungetc(*c, fp);
                        w.kind = MORE;
                    }
                    break;
                case '<':
                    *c = getc(fp);
                    if(*c == '=') {     // <=
                        strcat(w.text, c);
                        w.kind = LESSEQ;
                    } else {            // <
                        ungetc(*c, fp);
                        w.kind = LESS;
                    }
                    break;
                case '!':
                    *c = getc(fp);
                    if(*c == '=') {     // !=
                        strcat(w.text, c);
                        w.kind = NOTEQ;
                    } else {            // !
                        ungetc(*c, fp);
                        w.kind = NOT;
                    }
                    break;
                case '&':
                    *c = getc(fp);
                    if(*c == '&') {     // &&
                        strcat(w.text, c);
                        w.kind = AND;
                    } else {
                        w.kind = ERROR_TOKEN;
                        error_count++;
                    }
                    break;
                case '|':
                    *c = getc(fp);
                    if(*c == '|') {     // ||
                        strcat(w.text, c);
                        w.kind = OR;
                    } else {
                        w.kind = ERROR_TOKEN;
                        error_count++;
                    }
                    break;
                case '(':
                    cnt_lp++;
                    w.kind = LP;
                    break;
                case ')':
                    cnt_rp++;
                    w.kind = RP;
                    break;
                case ';':
                    w.kind = SEMI;
                    break;
                case ',':
                    w.kind = COMMA;
                    break;
                case '\'':
                    *c = getc(fp);
                    strcat(w.text, c);
                    *c = getc(fp);
                    strcat(w.text, c);
                    if(*c == '\'')
                        w.kind = CHAR_CONST;
                    else {
                        w.kind = ERROR_TOKEN;
                        error_count++;
                    }
                    break;
                case '\"':
                    do {
                        *c = getc(fp);
                        strcat(w.text, c);
                    } while(*c != '\"');
                    w.kind = STRING_CONST;
                    break;
                case '{':
                    cnt_lbp++;
                    w.kind = LBP;
                    break;
                case '}':
                    cnt_rbp++;
                    w.kind = RBP;
                    break;
                case '[':
                    w.kind = LK;
                    break;
                case ']':
                    w.kind = RK;
                    break;
                case '#':
                    w.kind = PRE;
                    break;
                case '.':
                    w.kind = DOT;
                    break;
            }
        }
        if(w.kind == ERROR_TOKEN) ++error_count;
        AnalysisCompleted(w);
        *c = getc(fp);
    }
    if(error_count > 0) return WORD_ERROR;
    return CORRECT;
}

#endif //SOURCE_PROGRAM_FORMATTING_WORDANALYZER_H
