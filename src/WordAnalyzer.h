//
// Created by Col_lin on 2022/7/31.
//

#ifndef SOURCE_PROGRAM_FORMATTING_WORDANALYZER_H
#define SOURCE_PROGRAM_FORMATTING_WORDANALYZER_H

#include "HEAD.h"
#include "Match.h"
#include "List.h"

int cnt_lp = 0;     //count (
int cnt_rp = 0;     //count )
int cnt_lbp = 0;    //count {
int cnt_rbp = 0;    //count }
int cnt_lba = 0;    //count /*
int cnt_rba = 0;    //count */

BOOL IsAlpha(char c) {
    if(c >= 'a' && c <= 'z')
        return TRUE;
    if(c >= 'A' && c <= 'Z')
        return TRUE;
    return FALSE;
}

BOOL IsNumber(char c) {
    if(c >= '0' && c <= '9')
        return TRUE;
    return FALSE;
}

void AnalysisCompleted(struct WORD w) {
    static int last_line = 0;
    w.pre_line = last_line;
    /*
    if(w.kind < 100)
        printf("KEY%d   %s\n",w.kind,w.text);
    else
        printf("%s    %s\n",kind_name[w.kind - 100],w.text);
        */
    last_line = w.line;
    struct WORD p = w;
    p.text = (char*) calloc(32,sizeof (char));
    strcat(p.text, w.text);
    AddList(token_list, p);
    return;
}

SITUATION WordAnalysis(FILE *fp) {
    token_list = CreatList();
    char* c = (char*) calloc(2,sizeof (char));
    *c = getc(fp);
    struct WORD w;
    w.text = (char*) calloc(32,sizeof (char));
    int line_count = 1;
    while(*c != EOF) {
        *w.text = 0;
        w.kind = ERROR_TOKEN;
        if(*c == ' '||*c == '\t') {
            *c = getc(fp);
            continue;
        }
        if(*c == '\n') {
            ++line_count;       //count the lines
            *c = getc(fp);
            continue;
        }
        w.line = line_count;    //line of the word
        w.text = strcat(w.text, c);
        if(IsAlpha(*c) == TRUE) {    //IDENT or KEY
            *c = getc(fp);
            while(*c == '_' || IsAlpha(*c) || IsNumber(*c)) {
                strcat(w.text, c);
                *c = getc(fp);
            }
            ungetc(*c, fp);
            SITUATION type = KeyWordMatching(w.text);
            if(type == UNMATCHED)
                w.kind = IDENT;
            else w.kind = type;
        } else if(IsNumber(*c) == TRUE) {   //const number
            *c = getc(fp);
            if(IsAlpha(*c) && *c != 'x' && *c !='X') {
                strcat(w.text, c);
                w.kind = ERROR_TOKEN;
                error_count++;
                AnalysisCompleted(w);
                *c = getc(fp);
                continue;
            }
            int flag = 0, hexflag = 0;
            while(*c == '.' || IsNumber(*c) || *c == 'x' || *c == 'X') {
                if(*c == '.' && flag == 0) { //'.' should be counted less then once
                    strcat(w.text, c);
                    flag = 1;
                } else if(IsNumber(*c)) {
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
                        if(*c == EOF) {
                            error_count++;
                            break;
                        }
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
    free(c);
    free(w.text);
    if(error_count > 0) return WORD_ERROR;
    return CORRECT;
}

#endif //SOURCE_PROGRAM_FORMATTING_WORDANALYZER_H
