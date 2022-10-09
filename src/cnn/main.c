#include "img.h"
#include "digits.h"
#include "grid.h"


/** 
 * @brief Tells how to use this program.
 * @param f where to write the info.
 * @param name the value of argv[0].
 */
void usage(FILE*f,char*name) {
    char * bname=basename(name);
    fprintf(f,"This is %s version %s on archictecture %s.\n",
            bname, VERSION,CFG_UNAME);
    fprintf(f,"Usage:\n");
    fprintf(f,"    %s <input-file> | <option> \n", bname);
    fprintf(f,"Solves a Sudoku grid given a png or jpeg image as input.\n");
    fprintf(f,"\n");
    fprintf(f,"Where option is one of:\n");
    fprintf(f,"    [-h|--help] :\n");
    fprintf(f,"         Displays this help message and leaves.\n");
    fprintf(f,"\n");
    fprintf(f,"%s comes with 3 friend tools:\n",bname);
    fprintf(f,"    img:\n");
    fprintf(f,"         Manipulates images. For help type:\n");
    fprintf(f,"         %s/bin/img --help\n",CFG_DEFAULT_PREFIX);
    fprintf(f,"    digits:\n");
    fprintf(f,"         Recognize digits. For help type:\n");
    fprintf(f,"         %s/bin/digits --help\n",CFG_DEFAULT_PREFIX);
    fprintf(f,"    grid:\n");
    fprintf(f,"         Recognize a sudoku grid. For help type:\n");
    fprintf(f,"         %s/bin/grid --help\n",CFG_DEFAULT_PREFIX);
    fprintf(f,"\n");
    fprintf(f,"%s was compiled on %s. Git repo used:\n",bname,__DATE__);
    fprintf(f,"  %s\n",CFG_GIT_REPO);
    fprintf(f,"with hash:\n"); 
    fprintf(f,"  %s\n",CFG_GIT_FHASH); 
}

int main (int argc,char**argv) {
    char * progName = basename(argv[0]);
    if (strcmp(progName,"img")==0) {
        return imgMain(argc,argv);
    } else if (strcmp(progName,"digits")==0) {
        return digitsMain(argc,argv);
    } else if (strcmp(progName,"grid")==0) {
        return gridMain(argc,argv);
    }
    
    for (int j=1;j<argc;++j) {
        if (strcmp(argv[j],"-h")==0 || strcmp(argv[j],"--help")==0) {
            usage(stdout,argv[0]);
            exit(0);
        }
    }
    if(argc < 2) {
        usage(stderr,argv[0]);
        ERROR("At least 1 argument expected, the picture to read","");
    }
    /* read input image */
    Img * rawInputImage=newImgRead(argv[1]);
    Img * goodContastImage=imgLuminosityScale(rawInputImage);
    Img * inverseImage = imgInvert(goodContastImage);
    deleteImg(goodContastImage);
    int scaleFactor=0;
    while (inverseImage->width>400 && inverseImage->height>400) {
        Img * i = imgDivideByTwo(inverseImage);
        deleteImg(inverseImage);
        inverseImage=i;
        scaleFactor++;
    }
    int xmin,ymin,xmax,ymax;
    // locate the grid
    gridLocate(inverseImage,&xmin,&ymin,&xmax,&ymax);
    imgDrawRect(inverseImage,xmin,ymin,xmax,ymax);
    imgWrite(inverseImage,"afterstd.png");
    // draw a square around the grid found in the
    // original picture
    imgDrawRect(rawInputImage,
                xmin<<scaleFactor,
                ymin<<scaleFactor,
                xmax<<scaleFactor,
                ymax<<scaleFactor);
    // write the image
    imgWrite(rawInputImage,"out.png");
    // free allocated memory
    deleteImg(inverseImage);
    deleteImg(rawInputImage);
    return 0;
}
