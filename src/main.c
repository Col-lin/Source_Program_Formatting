#include "HEAD.h"
#include "WordDetect.h"

FILE *fp;

int main() {
    fp = fopen("..\\test\\test.c","r");
    if(WordDetect(fp) == WORD_ERROR)
        return 0;
    fclose(fp);
    return 0;
}
