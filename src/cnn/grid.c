#include "filterfam.h"
#include "filter.h"
#include "imgfam.h"

/**
 * @file grid.c
 * @brief Implements function defined in grid.h.
 *
 * Functions in this file are used to detect the sudoku grid
 * in the picture.
 * Is is acheived via 3 convolution filters. First one 
 * has two parts : 1 detects horizontal lines, the other vertical lines.
 * Second filter makes sure that the lines are long enough. Third filter
 * checks the spacing of the 10 lines (horizontal and vertical) that
 * compose the grid.
 */

/**
 * @brief activates debug infos to be printed.
 */
int gridDumpDebugInfo=1;

/**
 * @brief Generates filters for the convolution layer to 
 *        detect Sudoku grids.
 *  
 * This function generates either vertical or horizontal filters
 * which are rotate between -5 and +5 degrees.
 * @param i : if i==0 this is a vertical filter
 *            if i==1 this is an horizontal filter
 * @param l length of the bar
 * @param tmin minimum thickness of the bar
 * @param tmax maximum thickness of the bar
 * @param p filter threshold percentage
 * @return the newly allocated filter 
 */
FilterFam * gridGetLayerHoriVertFilters(int i,
                                        int l,
                                        int tmin,
                                        int tmax,
                                        int p)
{
    FilterFam * answer=newFilterFam(11);
    int degreeMax=10;
    for (int t=tmin;t<=tmax;++t) {
        for (int degrees=-degreeMax;degrees<=degreeMax;++degrees) {
            Img * filter = newImgVerticalBarInRect(7*t,l,t);
            Img * rotatedFilter = NULL;
            if (i==0)
                rotatedFilter=newImgCopy(filter);
            else
                rotatedFilter=imgRotate90(filter);
            Img * r = imgRotate(rotatedFilter,degrees);
            Filter * invertedFilter = newFilter(imgInvert(r),p);
            if (0 && gridDumpDebugInfo) {
                HERE("layer2 on filter");
                HERED((int)invertedFilter->weight);
                HERE("_________");
                HERED((int)invertedFilter->maxVal);
            }
            ///invertedFilter->maxVal/10;
            deleteImg(filter);
            deleteImg(rotatedFilter);
            deleteImg(r);
            if (gridDumpDebugInfo || l==10) {
                char filterName[99];
                snprintf(filterName,99,"%s/grid/grid_filter_%d_%d_%02d",CFG_DATAROOTDIR,i,l,degrees);
                filterWrite(invertedFilter,filterName);
            }
            filterFamSetFilter(answer,degrees+degreeMax,invertedFilter);
        }
    }
    return answer;
}

/**
 * @brief Perform vertical and horizontal convolution.
 *
 * This function should be called several times until convolution
 * only keeps the sudoku grid lines.
 * @param outputH newly allocated image for horizontal convolution.
 * @param outputV newly allocated image for vertical convolution.
 * @param inputH current image for horizontal convolution.
 * @param inputV current image for vertical convolution.
 * @param width width of convolution
 * @param length length of convolution
 * @param poolsize pool size to reduce image (1 for no reduction)
 * @param stride step to jump to decuce image (1 for no reduction)
 */
void gridVertHoriConvo(Img ** outputH,
                       Img ** outputV,
                       Img * inputH,
                       Img * inputV,
                       int width,
                       int length,
                       int threshold,
                       int poolsize,
                       int stride)
{
    FilterFam * filtersVert=
        gridGetLayerHoriVertFilters(0,length,width,width+0,threshold);
    FilterFam * filtersHori=
        gridGetLayerHoriVertFilters(1,length,width,width+0,threshold);
    
    ImgFam * layerVertOutput =
        filterFamApplyConvolutionSameSizeDiff(filtersVert,inputV);
    ImgFam * layerHoriOutput =
        filterFamApplyConvolutionSameSizeDiff(filtersHori,inputH);
    deleteFilterFam(filtersVert);
    deleteFilterFam(filtersHori);
    
    ImgFam * layerMaxPoolVertOutput=
        imgFamDownSampleMax(layerVertOutput,poolsize,stride);
    ImgFam * layerMaxPoolHoriOutput=
        imgFamDownSampleMax(layerHoriOutput,poolsize,stride);
    deleteImgFam(layerVertOutput);
    deleteImgFam(layerHoriOutput);
    
    ImgFam * betterContrastHori2 =
        imgFamLuminosityScale(layerMaxPoolHoriOutput);
    ImgFam * betterContrastVert2 =
        imgFamLuminosityScale(layerMaxPoolVertOutput);
    deleteImgFam(layerMaxPoolVertOutput);
    deleteImgFam(layerMaxPoolHoriOutput);
    
    if (betterContrastHori2->count!=1 ||
        betterContrastVert2->count!=1) {
        //ERROR("Wrong size.","");
    }
    // write address the filter to return to caller
    //*outputV=betterContrastVert2->imgs[0];
    //*outputH=betterContrastHori2->imgs[0];
    *outputV=imgFamMaxAllFam(betterContrastVert2);
    *outputH=imgFamMaxAllFam(betterContrastHori2);
    // pretend there is no filter in the filterFam
    //betterContrastHori2->count=0;
    //betterContrastVert2->count=0;
    // delete filterFam allocated data
    deleteImgFam(betterContrastHori2);
    deleteImgFam(betterContrastVert2);
}

// for debug
int gridIdentifyNPointsCounter=0;

/**
 * @brief identify N points equaly spaced in a 
 *        n by 1 pixel image.
 *
 * This function, called from gridLocate, is a simple convolution 
 * with some filters comming from newImgNDotsHori.
 * @param N number of points equaly spaced we are looking for.
 * @param img an image of n by 1 pixels
 * @param startRange minimum distance in pixel among which the N
 *        points are spread.
 * @param endRange maximum distance in pixel among which the N
 *        points are spread.
 * @see newImgNDotsHori
 * @see gridLocate
 */
int gridIdentifyNPoints(int N,
                        Img *img,
                        int startRange,
                        int endRange,
                        int * lowerBound,
                        int * upperBound,
                        int * maxCorrelation)
{
    int n = img->width;
    if (img->height!=1) {
        ERROR("Expected a height of 1","");
    }
    gridIdentifyNPointsCounter++;
    Img * imgs[n];
    int convVal[n];
    int weight[n];
    int max=0;
    int maxIdx=0;
    int lowerBoundOffset[n];
    int upperBoundOffset[n];
    for (int i=n/2;i<n-1;++i) {
        Img * f1 = newImgNDotsHori(N,i);
        Filter * f =newFilter(imgInvert(f1),99);
        deleteImg(f1);
        if (gridDumpDebugInfo) {
            char s[99];
            snprintf(s,99,"conv_filter_%d_%d.png",N,i);
            imgWrite(f->img,s);
        }
        Img * c = imgConvolutionDiff(img,f);
        weight[i]=f->weight;
        deleteFilter(f);
        imgs[i]=c;
        if (gridDumpDebugInfo) {
            char s[99];
            snprintf(s,99,"conv_result_%d_%d.png",gridIdentifyNPointsCounter,i);
            imgWrite(c,s);
        }
        int localMax=-1;
        for (int j=0;j<c->width;++j) {
            if (c->data[j]>localMax) {
                localMax=c->data[j];
                lowerBoundOffset[i]=j+i/2/N;
                upperBoundOffset[i]=j+i-i/2/N;
            }
        }
        if (localMax>max) {
            max=localMax;maxIdx=i;
        }
        convVal[i]=localMax;
    }
    for (int i=n/2;i<n-1;++i) {
        if (max-max/20<convVal[i]) {
            if (gridDumpDebugInfo) {
                HERE("------------");
                HERED(convVal[i]);
                HERED(i);
                //imgPrint(imgs[i]);
            }
        }
        deleteImg(imgs[i]);
    }
    *maxCorrelation=convVal[maxIdx];
    *lowerBound=lowerBoundOffset[maxIdx];
    *upperBound=upperBoundOffset[maxIdx];
    return 0;
}

void gridShowAllLayer1Output(Img * img,char *prefix,char * suffix) {
    // turn all debug info to false on n, rather
    gridDumpDebugInfo=0;
    for (int i=200; i>0; i=i-10) {
        printf("generating grid layer 1 output for threshold %d.\n",i);
        Img *layer1HO,*layer1VO;
        gridVertHoriConvo(&layer1HO,&layer1VO,
                          img,
                          img,
                          2,
                          i, /* length */
                          0,  /* threshold */ 
                          1,1);
        char s[99];
        snprintf(s,99,"%sgridLayer1OutHori%03d%s",prefix,i,suffix);
        imgWrite(layer1HO,s);
        snprintf(s,99,"%sgridLayer1OutVert%03d%s",prefix,i,suffix);
        imgWrite(layer1VO,s);

        deleteImg(layer1HO);
        deleteImg(layer1VO);
    }
}

Img* gridPreprocessRawPicture(char * imgName, int * scaleFactor,int saveToDisk)
{
    Img * rawInputImage=newImgRead(imgName);
    *scaleFactor=0;
    while (rawInputImage->width>400 && rawInputImage->height>400) {
        Img * i = imgDivideByTwo(rawInputImage);
        deleteImg(rawInputImage);
        rawInputImage=i;
        (*scaleFactor)++;
    }
    HERE(":-(");
    if (saveToDisk) {
        HERE(":-)");
        imgWrite(rawInputImage,
                 "../../doc/explanation/generated/gridLayer0_0.png");
    }
    Img * goodContastImage=imgLuminosityScale(rawInputImage);
    if (saveToDisk) {
        imgWrite(goodContastImage,
                 "../../doc/explanation/generated/gridLayer0_1.png");
    }
    Img * invertedImage = imgRotate(imgInvert(goodContastImage),-2);
    if (saveToDisk) {
        imgWrite(invertedImage,
                 "../../doc/explanation/generated/gridLayer0_2.png");
    }
    /*
    Img * edgeDetection = imgEdgeDetect(invertedImage);
    if (saveToDisk) {
        imgWrite(edgeDetection,
                 "../../doc/explanation/generated/gridLayer0_3.png");
    }
    Img * answer = imgLuminosityScale(edgeDetection);
    if (saveToDisk) {
        imgWrite(answer,
                 "../../doc/explanation/generated/gridLayer0_4.png");
    }
    */
    deleteImg(goodContastImage);
    //deleteImg(invertedImage);
    //deleteImg(edgeDetection);
    //return answer;
    return invertedImage;
}

/**
 * @brief Locates a sudoku grid in a picture.
 *
 * This function calls gridVertHoriConvo to perform a vertical and
 * horizontal convolution using filters from gridGetLayerHoriVertFilters.
 * The verification that we get 10 lines horizontally or vertically is 
 * performed by calling function gridIdentifyNPoints.
 *
 * This function should not be called on a picture which is too large.
 * Width and height of the picture between 200 and 400 pixels would be 
 * a typical usage.
 *
 * @param img picture in which we are looking.
 * @param xmin pointer to an integer where the horizontal position
 *        of the lower left position of the grid will be written.
 * @param ymin pointer to an integer where the vertical position
 *        of the lower left position of the grid will be written.
 * @param xmax pointer to an integer where the horizontal position
 *        of the upper right position of the grid will be written.
 * @param ymax pointer to an integer where the vertical position
 *        of the upper right position of the grid will be written.
 * @return 0 if a grid had been found, a number between 1 and 255 otherwise.
 * @see gridGetLayerHoriVertFilters
 * @see gridVertHoriConvo
 * @see gridIdentifyNPoints
 */
int gridLocate(Img * img,
               int * xmin,
               int * ymin,
               int * xmax,
               int * ymax)
{
    Img *layer1HO,*layer1VO;
    gridVertHoriConvo(&layer1HO,&layer1VO,
                      img,
                      img,
                      1,
                      50, /* length */
                      -10,  /* threshold */ 
                      1,1);
    
    
    if (gridDumpDebugInfo) {
        imgWrite(layer1HO,"gridLayer1OutputHori.png");
        imgWrite(layer1VO,"gridLayer1OutputVert.png");
    }
    Img *layer5HO,*layer5VO;
    layer5HO=layer1HO;
    layer5VO=layer1VO;
    int sumMinVal=8*img->height;
    int sumMaxVal=128*img->height;
    Img * flattenedVert = newImgColor(layer5VO->width,1,0);
    for (int x=0;x<layer5VO->width;++x) {
        int s=0;
        for (int y=0;y<layer5VO->height;++y) {
            s+=layer5VO->data[x+layer5VO->width*y];
        }
        if (s<sumMinVal) s=0;
        else if (s>sumMaxVal) s=255;
        else s=255*(s-sumMinVal)/(sumMaxVal-sumMinVal);
        flattenedVert->data[x]=s;
        printf("%d ",s);
    }
    
    if (gridDumpDebugInfo) {
        imgWrite(flattenedVert,"gridLayer2OutputVert.png");
    }
    printf("\n");
    sumMinVal=8*img->width;
    sumMaxVal=128*img->width;
    Img * flattenedHori = newImgColor(layer5VO->height,1,0);
    for (int y=0;y<layer5HO->height;++y) {
        int s=0;
        for (int x=0;x<layer5HO->width;++x) {
            s+=layer5HO->data[x+layer5VO->width*y];
        }
        if (s<sumMinVal) s=0;
        else if (s>sumMaxVal) s=255;
        else s=255*(s-sumMinVal)/(sumMaxVal-sumMinVal);
        flattenedHori->data[y]=s;
        printf("%d ",s);
    }
    
    if (gridDumpDebugInfo) {
        imgWrite(flattenedHori,"gridLayer2OutputHori.png");
    }
    printf("\n");

    // the 10 equidistant point should spread on the minimum
    // between img->width and img->height and that value
    // divided by 2.
    int minLength = img->width;
    if (img->height<minLength) minLength=img->height;
    int endRange=minLength;
    int startRange=4*endRange/5;
    int maxCorrelationVer=0;
    gridIdentifyNPoints(10,flattenedVert,startRange, endRange,xmin,xmax,
                        &maxCorrelationVer);

    if (gridDumpDebugInfo) {
        HERE("+++++++++++++++");
    }
    int maxCorrelationHori=0;
    gridIdentifyNPoints(10,flattenedHori,startRange, endRange,ymin,ymax,
                        &maxCorrelationHori);
    HERE("maxCorrelationHori");
    HERED(maxCorrelationHori);
    HERE("maxCorrelationVer");
    HERED(maxCorrelationVer);

    deleteImg(flattenedVert);
    deleteImg(flattenedHori);

    deleteImg(layer1HO);
    deleteImg(layer1VO);
    return 0;
}

/** 
 * @brief Tells how to use this program.
 * @param f where to write the info.
 * @param name the value of argv[0].
 */
void gridUsage(FILE*f,char*name) {
    char * bname=basename(name);
    fprintf(f,"%s usage:\n", bname);
    fprintf(f,"    %s <input-file> [options] \n", bname);
    fprintf(f,"Locates a sudoku grid in an image.:\n");
    fprintf(f,"\n");
    fprintf(f,"Where option is one of:\n");
    fprintf(f,"    [-l|--layer] <n> :\n");
    fprintf(f,"         Output layer <n> to the disk for different values\n");
    fprintf(f,"         of fiters\n");
    fprintf(f,"    [-g|--grid] :\n");
    fprintf(f,"         Try to find a sudoku grid.\n");
    fprintf(f,"    [-h|--help] :\n");
    fprintf(f,"         Displays this help message and leaves.\n");
}

/**
 * @brief main function to execute when the grid
 *        executable is called from the command line.
 * @param argc number of arguments on the command line.
 * @param argv value of arguments on the command line.
 */
int gridMain(int argc,char**argv) {
    int showLevel1=0;
    for (int j=1;j<argc;++j) {
        if (strcmp(argv[j],"-h")==0 || strcmp(argv[j],"--help")==0) {
            gridUsage(stdout,argv[0]);
            exit(0);
        }
    }
    if(argc < 3) {
        gridUsage(stderr,argv[0]);
        ERROR("At least 2 argumens expected, the picture to read, and where tp write the output","");
    }
    
    /* read input image */
    int scaleFactor=0;
    Img * currentImage = gridPreprocessRawPicture(argv[1],
                                                  &scaleFactor,
                                                  0);

#define NOT_ENOUGH                                        \
    if (i>=argc) {                                        \
        gridUsage(stderr,argv[0]);                        \
        ERROR("not enough arguments.","");                \
    }
    int i=2;
    while (i<argc) {
        if (strcmp("-g",argv[i])==0 ||
            strcmp("--grid",argv[i])==0) {
            int xmin,ymin,xmax,ymax;
            // locate the grid
            if (!gridLocate(currentImage,&xmin,&ymin,&xmax,&ymax)) {
                HERE("Found sudoku grid :");
                printf("%d %d %d %d\n",xmin,ymin,xmax,ymax);
                // draw the grid found
                imgDrawRect(currentImage,xmin,ymin,xmax,ymax);
                // write the image
                imgWrite(currentImage,argv[2]);
            } else {
                ERROR("Could not find a grid in picture: ",argv[1]);
            }
        } else if (strcmp("-l",argv[i])==0 ||
                   strcmp("--layer",argv[i])==0) {
            ++i;
            NOT_ENOUGH;
            int layernum = atoi(argv[i]);
            switch (layernum) {
            case 0:
                {
                    int scaleFactor=0;
                    gridDumpDebugInfo=1;
                    gridPreprocessRawPicture(argv[1],&scaleFactor,1);
                }
                break;
            case 1:
                gridShowAllLayer1Output(currentImage,
                                        "../../doc/explanation/generated/",
                                        basename(argv[1]));
                break;
            default :
                ERROR("No such layer","");
                
            }
        } else {
            gridUsage(stderr,argv[0]);
            ERROR("unknown option: ",argv[i]);
        }
        ++i;
    }
    return 0;
}
/*
./img --sudoku 190 -i -r 5 sudoku.png 
./grid sudoku.png csca
for i in gridLayer1Output_*; do ./img $i -s 4 $i; done
open gridLayer1Output_*

*/
