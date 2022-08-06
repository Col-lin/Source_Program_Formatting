#include "HEAD.h"
#include "WordAnalyzer.h"
#include "SyntaxAnalyzer.h"

int main() {
    fil = fopen("..\\test\\test.c","r");
    if(WordAnalysis(fil) == WORD_ERROR) {
        fclose(fil);
        return 0;
    }
    if(SyntaxAnalyzer() == SYNTAX_ERROR) {
        fclose(fil);
        return 0;
    }
    fclose(fil);
    return 0;
}
