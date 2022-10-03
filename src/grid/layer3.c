#include <stdlib.h>
#include <stdio.h>
#include "l3.c"
#include "fontname.h"
int main () {
    int errorCount=0,caseCount=0;
    init_digits();
    for (int f=1;f<31;++f) {
        // using font f
        int fontError=0;
        for (int d=1;d<10;++d) {
            // trying to guess digit d
            for (int i=1;i<10;++i) {
                // check if i is the highest
                if (d!=i) {
                    if (digits[f][d][i]>=digits[f][d][d]) {
                        // another one is the highest
                        errorCount++;
                        fontError++;
                    }
                }
                caseCount++;
            }
        }
        printf("font %d %s: \t%d errors\n",f,fontname[f],fontError);
    }
    printf("%d/%d\n",errorCount,caseCount);
    return 0;
}
