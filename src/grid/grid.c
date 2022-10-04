#include "filterfam.h"
#include "filter.h"
#include "imgfam.h"

// upper left
unsigned char gridFilterLayer1UpperleftSmall [] = {
    0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,
    0,   0, 255, 255, 255,
    0,   0, 255,   0,   0,
    0,   0, 255,   0,   0,
};
unsigned char gridFilterLayer1UpperleftMedium [] = {
    0,   0,   0,   0,   0,
    0, 128, 128, 128, 128,
    0, 128, 255, 255, 255,
    0, 128, 255, 128, 128,
    0, 128, 255, 128,   0,
};
unsigned char gridFilterLayer1UpperleftLarge [] = {
    0,   0,   0,   0,   0,
    0, 255, 255, 255, 255,
    0, 255, 255, 255, 255,
    0, 255, 255, 255, 255,
    0, 255, 255, 255,   0,
};
//  upper right
unsigned char gridFilterLayer1UpperrightSmall [] = {
    0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,
  255, 255, 255,   0,   0,
    0,   0, 255,   0,   0,
    0,   0, 255,   0,   0,
};
unsigned char gridFilterLayer1UpperrightMedium [] = {
    0,   0,   0,   0,   0,
  128, 128, 128, 128,   0,
  255, 255, 255, 128,   0,
  128, 128, 255, 128,   0,
    0, 128, 255, 128,   0,
};
unsigned char gridFilterLayer1UpperrightLarge [] = {
    0,   0,   0,   0,   0,
  255, 255, 255, 255,   0,
  255, 255, 255, 255,   0,
  255, 255, 255, 255,   0,
    0, 255, 255, 255,   0,
};
// lower left
unsigned char gridFilterLayer1LowerleftSmall [] = {
    0,   0, 255,   0,   0,
    0,   0, 255,   0,   0,
    0,   0, 255, 255, 255,
    0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,
};
unsigned char gridFilterLayer1LowerleftMedium [] = {
    0, 128, 255, 128,   0,
    0, 128, 128, 128, 128,
    0, 128, 255, 255, 255,
    0, 128, 255, 128, 128,
    0,   0,   0,   0,   0,
};
unsigned char gridFilterLayer1LowerleftLarge [] = {
    0, 255, 255, 255,   0,
    0, 255, 255, 255, 255,
    0, 255, 255, 255, 255,
    0, 255, 255, 255, 255,
    0,   0,   0,   0,   0,
};
//  lower right
unsigned char gridFilterLayer1LowerrightSmall [] = {
    0,   0, 255,   0,   0,
    0,   0, 255,   0,   0,
  255, 255, 255,   0,   0,
    0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,
};
unsigned char gridFilterLayer1LowerrightMedium [] = {
    0, 128, 255, 128,   0,
  128, 128, 128, 128,   0,
  255, 255, 255, 128,   0,
  128, 128, 255, 128,   0,
    0,   0,   0,   0,   0,
};
unsigned char gridFilterLayer1LowerrightLarge [] = {
    0, 255, 255, 255,   0,
  255, 255, 255, 255,   0,
  255, 255, 255, 255,   0,
  255, 255, 255, 255,   0,
    0,   0,   0,   0,   0,
};
//  right
unsigned char gridFilterLayer1RightSmall [] = {
    0,   0, 255,   0,   0,
    0,   0, 255,   0,   0,
  255, 255, 255,   0,   0,
    0,   0, 255,   0,   0,
    0,   0, 255,   0,   0,
};
unsigned char gridFilterLayer1RightMedium [] = {
    0, 128, 255, 128,   0,
  128, 128, 128, 128,   0,
  255, 255, 255, 128,   0,
  128, 128, 255, 128,   0,
    0, 128, 255, 128,   0,
};
unsigned char gridFilterLayer1RightLarge [] = {
    0, 255, 255, 255,   0,
  255, 255, 255, 255,   0,
  255, 255, 255, 255,   0,
  255, 255, 255, 255,   0,
    0, 255, 255, 255,   0,
};

//  left
unsigned char gridFilterLayer1LeftSmall [] = {
    0,   0, 255,   0,   0,
    0,   0, 255,   0,   0,
    0,   0, 255, 255, 255, 
    0,   0, 255,   0,   0,
    0,   0, 255,   0,   0,
};
unsigned char gridFilterLayer1LeftMedium [] = {
    0, 128, 255, 128,   0,
    0, 128, 128, 128, 128,
    0, 255, 255, 128, 255,
    0, 128, 255, 128, 128,
    0, 128, 255, 128,   0,
};
unsigned char gridFilterLayer1LeftLarge [] = {
    0, 255, 255, 255,   0,
    0, 255, 255, 255, 255,
    0, 255, 255, 255, 255,
    0, 255, 255, 255, 255,
    0, 255, 255, 255,   0,
};
//  down
unsigned char gridFilterLayer1DownSmall [] = {
    0,   0, 255,   0,   0,
    0,   0, 255,   0,   0,
  255, 255, 255, 255, 255,
    0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,
};
unsigned char gridFilterLayer1DownMedium [] = {
    0, 128, 255, 128,   0,
  128, 128, 128, 128, 128,
  255, 255, 255, 128, 255,
  128, 128, 255, 128, 128,
    0,   0,   0,   0,   0,
};
unsigned char gridFilterLayer1DownLarge [] = {
    0, 255, 255, 255,   0,
  255, 255, 255, 255, 255,
  255, 255, 255, 255, 255,
  255, 255, 255, 255, 255,
    0,   0,   0,   0,   0,
};
//  up
unsigned char gridFilterLayer1UpSmall [] = {
    0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,
  255, 255, 255, 255, 255,
    0,   0, 255,   0,   0,
    0,   0, 255,   0,   0,
};
unsigned char gridFilterLayer1UpMedium [] = {
    0,   0,   0,   0,   0,
  128, 128, 128, 128, 128,
  255, 255, 255, 128, 255,
  128, 128, 255, 128, 128,
    0, 128, 255, 128,   0,
};
unsigned char gridFilterLayer1UpLarge [] = {
    0,   0,   0,   0,   0,
  255, 255, 255, 255, 255,
  255, 255, 255, 255, 255,
  255, 255, 255, 255, 255,
    0, 255, 255, 255,   0,
};
// horizontal
unsigned char gridFilterLayer1HoriSmall [] = {
    0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,
  255, 255, 255, 255, 255,
    0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,
};
unsigned char gridFilterLayer1HoriMedium [] = {
    0,   0,   0,   0,   0,
  128, 128, 128, 128, 128,
  255, 255, 255, 255, 255,
  128, 128, 128, 128, 128,
    0,   0,   0,   0,   0,
};
unsigned char gridFilterLayer1HoriLarge [] = {
    0,   0,   0,   0,   0,
  255, 255, 255, 255, 255,
  255, 255, 255, 255, 255,
  255, 255, 255, 255, 255,
    0,   0,   0,   0,   0,
};
// vertical
unsigned char gridFilterLayer1VertSmall [] = {
    0,   0, 255,   0,   0,
    0,   0, 255,   0,   0,
    0,   0, 255,   0,   0,
    0,   0, 255,   0,   0,
    0,   0, 255,   0,   0,
};
unsigned char gridFilterLayer1VertMedium [] = {
    0, 128, 255, 128,   0,
    0, 128, 255, 128,   0,
    0, 128, 255, 128,   0,
    0, 128, 255, 128,   0,
    0, 128, 255, 128,   0,
};
unsigned char gridFilterLayer1VertLarge [] = {
    0, 255, 255, 255,   0,
    0, 255, 255, 255,   0,
    0, 255, 255, 255,   0,
    0, 255, 255, 255,   0,
    0, 255, 255, 255,   0,
};


/**
 * @brief Generates filters for the first convolution layer to 
 * detect Sudoku grids.
 */
FilterFam * getGridLayer1Filters() {
    FilterFam * answer=newFilterFam(18);
    int squareCounter=0;
    for (int square_size = 11; square_size<30;square_size+=1) {
        Img * filter = newImgSquare(square_size);
        Filter * invertedFilter = newFilter(imgInvert(filter),80);
        filterSetWeight(invertedFilter,10200);
        deleteImg(filter);
        char filterName[99];
        sprintf(filterName,"%s/grid/grid_layer1_%d.png",CFG_DATAROOTDIR,squareCounter);
        filterWrite(invertedFilter,filterName);
        filterFamSetFilter(answer,squareCounter++,invertedFilter);
    }
    return answer;
}

/**
 * @brief Generates filters for the second convolution layer to 
 * detect Sudoku grids.
 */
FilterFam * getGridLayer2Filters() {
    // make && ./img --9x9dots 45 out.png && open out.png 
    // make && ./img --9x9dots 37 out.png && open out.png 
    // make && ./img --9x9dots 27 out.png && open out.png 
    // make && ./img --9x9dots 19 out.png && open out.png 
    FilterFam * answer=newFilterFam(30);
    for (int  i = 0; i<30;++i) {
        Img * nineByNine = newImg9By9Dots(30+i);
        Img * invertedNineByNine = imgInvert(nineByNine);
        Filter * filter = newFilter(invertedNineByNine,30);
        printf("**************** %d %ld\n",i+30,filter->weight);
        filterSetWeight(filter,171891);
        HERE("after imgSetWeight");
        HERED((int)filter->weight);
        deleteImg(nineByNine);
        filterFamSetFilter(answer,i,filter);
    }
    char filterFamBaseName[99];
    sprintf(filterFamBaseName,"%s/grid/grid_layer2",CFG_DATAROOTDIR);
    filterFamWrite(answer,filterFamBaseName);
    return answer;
}

/** 
 * @brief Tells how to use this program.
 * @param f where to write the info.
 * @param name the value of argv[0].
 */
void gridUsage(FILE*f,char*name) {
    char * bname=basename(name);
    fprintf(f,"%s usage:\n", bname);
    fprintf(f,"    %s <input-file> [ options ] <output-file>\n", bname);
    fprintf(f,"Where option is one of:\n");
    fprintf(f,"    [-b|--blur] <r>:\n");
    fprintf(f,"         Blurs the image with radius of r.\n");
    fprintf(f,"    [-h|--help] :\n");
    fprintf(f,"         Displays this help message and leaves.\n");
}

int gridMain(int argc,char**argv) {
    
    for (int j=1;j<argc;++j) {
        if (strcmp(argv[j],"-h")==0 || strcmp(argv[j],"--help")==0) {
            gridUsage(stdout,argv[0]);
            exit(0);
        }
    }
    if(argc < 3) {
        gridUsage(stderr,argv[0]);
        ERROR("At least Two arguments expected","");
    }
    /* read input image */
    Img * currentImage=newImgRead(argv[1]);
    
    /* first layer with little squares */
    FilterFam * firstLevelFilters=getGridLayer1Filters();
    ImgFam * firstLayerOutput =
        filterFamApplyConvolution(firstLevelFilters,currentImage);
    
    HERE(":::::::::");
    HERED(firstLayerOutput->imgs[0]->width);
    ImgFam * firstLayerMaxPoolOutput=imgFamDownSampleMax(firstLayerOutput,3,3);
    
    imgFamWrite(firstLayerMaxPoolOutput,"gridLayer1Output");
    HERE(":::::::::");
    HERED(firstLayerMaxPoolOutput->imgs[0]->width);
    /* second layer : check we have 9x9 little squares */
    
    FilterFam * secondLevelFilters=getGridLayer2Filters();
    ImgFam * secondLayerOutput =
        filterFamApplyConvolutionOnFam(secondLevelFilters,
                                    firstLayerMaxPoolOutput);
    
    HERE(":::::::::");
    HERED(secondLayerOutput->imgs[0]->width);
    ImgFam * secondLayerMaxPoolOutput=imgFamDownSampleMax(secondLayerOutput,2,2);
    
    imgFamWrite(secondLayerMaxPoolOutput,"gridLayer2Output");
    HERE(":::::::::");
    HERED(secondLayerMaxPoolOutput->imgs[0]->width);
    return 0;
}

/*
./img --sudoku 190 -i -r 5 sudoku.png 
./grid sudoku.png csca
for i in gridLayer1Output_*; do ./img $i -s 4 $i; done
open gridLayer1Output_*

 */
