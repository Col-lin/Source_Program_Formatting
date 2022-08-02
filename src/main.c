#include "HEAD.h"
#include "WordAnalyzer.h"

FILE *fp;

int main() {
    fp = fopen("..\\test\\test.c","r");
    if(WordAnalysis(fp) == WORD_ERROR)
        return 0;
    fclose(fp);
    return 0;
}
