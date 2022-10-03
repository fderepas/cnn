#include "img.h"
#include "digits.h"
#include "grid.h"

int main (int argc,char**argv) {
    char * progName = basename(argv[0]);
    if (strcmp(progName,"img")==0) {
        return imgMain(argc,argv);
    }
    if (strcmp(progName,"digits")==0) {
        return digitsMain(argc,argv);
    }
    if (strcmp(progName,"grid")==0) {
        return gridMain(argc,argv);
    }
    ERROR("Unexpected program name: ",progName);
    return 0;
}
