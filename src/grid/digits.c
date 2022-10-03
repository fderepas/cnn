#include "imgfam.h"
#include "img.h"
#include "filterfam.h"
#include <libgen.h>

/**
 * @brief Layer 1 is made of 5x5 filters with a rotating white bar
 * on a back backound.
 * 
 * Data is stored in CFG_DATAROOTDIR/digits/layer_1
 * Where CFG_DATAROOTDIR is $prefix/share where $prefix
 * is the value of the --prefix argument supplied to the
 * configure script.
 */
void generateDigitsLayer1 () {
    // create 6 images 15x15 images with a rotating bar
    Img * allBars[6];
    allBars[0]=newImgVerticalBar(15,3);
    for (int i=1;i<6;++i)
        allBars[i]=imgRotate(allBars[0],30*i);
    // make each image 5x5
    for (int i=0;i<6;++i) {
        Img * tmp = allBars[i];
        allBars[i]=imgDownSampleAvg(tmp,3,3);
        deleteImg(tmp);
    }
    // creates corresponding filter familly
    FilterFam * filterFamilly = newFilterFam(6);
    for (int i=0;i<6;++i) {
        filterFamSetFilter(filterFamilly,
                           i,
                           newFilter(imgInvert(allBars[i]),80));
        // make sure they all have the same weight
        filterSetWeight(filterFamilly->filters[i],1300);
        HERE("filter weight");
        HERED((int)filterFamilly->filters[i]->weight);
        deleteImg(allBars[i]);
    }
    // save the familly on the disk
    char s[99];
    sprintf(s,"%s/digits/layer_1",CFG_DATAROOTDIR);
    filterFamWrite(filterFamilly,s);
}

void testDigitsLevel1() {
    // create 6 images 30x30 images with a rotating bar
    Img * testImg[6];
    Img * base = newImgVerticalBar(30,2);
    for (int i=0;i<6;++i) {
        Img * tmp = imgRotate(base,30*i);
        testImg[i]=imgInvert(tmp);
        deleteImg(tmp);
        char s[99];
        sprintf(s,"test_digits_layer_1_%d.png",i);
        imgWrite(testImg[i],s);
    }
    deleteImg(base);

    // read existing filters
    char s[99];
    sprintf(s,"%s/digits/layer_1",CFG_DATAROOTDIR);
    FilterFam * firstLevelFilters=filterFamRead(s);

    
    for (int i=0;i<6;++i) {
        // creates a familly of 6 26x26 images
        ImgFam * firstLayerOutput =
            filterFamApplyConvolution(firstLevelFilters,testImg[i]);
        // verify that the maximum output for a bar with
        // and angle i*30 is of the i-th image.
        int maxIndex=-1;
        int maxVal=-1;
        for (int j=0;j<6;++j) {
            int v=imgGetWeight(firstLayerOutput->imgs[j]);
            if (v>maxVal) {
                maxVal=v;
                maxIndex=j;
            }
        }
        if (maxIndex!=i) {
            ERROR("Wrong index.","");
        }
        char s[99];
        sprintf(s,"test_digits_l1out_%d",i);
        imgFamWrite(firstLayerOutput,s);
    }

    
    // create 6 images 30x30 images with a rotating bar
    // of 30*i+15 degrees
    Img * testImgPlus15[6];
    base = newImgVerticalBar(30,3);
    for (int i=0;i<6;++i) {
        Img * tmp = imgRotate(base,15+30*i);
        testImgPlus15[i]=imgInvert(tmp);
        deleteImg(tmp);
        char s[99];
        sprintf(s,"test_digits_layer_1_15_%d.png",i);
        imgWrite(testImgPlus15[i],s);
    }
    deleteImg(base);
    
    for (int i=0;i<6;++i) {
        // creates a familly of 6 26x26 images
        ImgFam * firstLayerOutput =
            filterFamApplyConvolution(firstLevelFilters,testImgPlus15[i]);
        char s[99];
        sprintf(s,"test_digits_l1out_15_%d",i);
        imgFamWrite(firstLayerOutput,s);
    }
}

/**
 * @brief Generate layer 2 to recognized digit n
 * @param n digit to recognize
 */
void generateLayer2 (int n) {
    char s[99];
    sprintf(s,"%s/digits/layer_1",CFG_DATAROOTDIR);
    FilterFam * firstLevelFilters=filterFamRead(s);
    sprintf(s,"%s/digits/reference_%d.png",CFG_DATAROOTDIR,n);
    //sprintf(s,"%s/digits/reference_O.png",CFG_DATAROOTDIR,n);
    Img * tmpimg = newImgRead(s);
    Img * img = imgInvert(tmpimg);
    deleteImg(tmpimg);
    ImgFam * firstLayerOutput =
        filterFamApplyConvolution(firstLevelFilters,img);
    imgFamWrite(firstLayerOutput,"firstLayerOutput_O");
    ImgFam * firstLayerMaxPoolOutput=imgFamDownSampleMax(firstLayerOutput,2,2);
    //ImgFam * avgPool=imgFamDownSampleMax(firstLayerMaxPoolOutput,2,2);
    
    ImgFam * avgPool = firstLayerMaxPoolOutput;
    char exportBaseName[99];
    sprintf(exportBaseName,"%s/digits/layer_2_Digit%d",CFG_DATAROOTDIR,n);
    //sprintf(exportBaseName,"%s/digits/layer_2_O",CFG_DATAROOTDIR);
    imgFamWrite(avgPool,exportBaseName);
    // free allocated memory
    deleteImg(img);
    deleteFilterFam(firstLevelFilters);
    deleteImgFam(firstLayerOutput);
    deleteImgFam(firstLayerMaxPoolOutput);
    //deleteImgFam(avgPool);
}

/**
 * @brief Creates images of characters 1 to 9.
 *
 * This function creates a bash script which callx the 
 * 'convert' program to: list all fonts on the system and 
 * generate 28x28 pictures of each digits 1 to 9 for the firt
 * numFonts fonts found.
 * @param directory where to create the images
 * @param numFonts number of fonts to create.
 */
void generateImageOfFont(char*directory,int numFonts) {
    char * tmpFile = (char*)malloc(strlen(directory)+40);
    sprintf(tmpFile,"%s/generate_png_from_font.sh",directory);
    FILE * f = fopen(tmpFile,"w");
    HERE(tmpFile);
    fprintf(f,"#/bin/sh\n");
    fprintf(f,"# this file has been automatically generated\n");
    fprintf(f,"set -e\n");
    fprintf(f,"cd `dirname $0`\n");
    fprintf(f,"fonts=(`convert -list font  | grep Font: | grep -vi emoji| sed -e s/Font:// | tr -d ' '`)\n");
    fprintf(f,"fmax=${#fonts[@]}\n");
    fprintf(f,"fmax=$[fmax-1]\n");
    fprintf(f,"if [ $fmax -gt %d ]; then\n",numFonts);
    fprintf(f,"    fmax=%d\n",numFonts);
    fprintf(f,"fi\n");
    fprintf(f,"for f in `seq 0 $fmax`; do\n");
    fprintf(f,"    for i in `seq 1 9`; do\n");
    fprintf(f,"        convert -background white -fill black -font ${fonts[$f]} -size 28x28 -gravity center \"caption:$i\" $[f+1]_$i.png\n");
    fprintf(f,"    done\n");
    fprintf(f,"done\n");
    fclose(f);
    char cmd[99];
    sprintf(cmd,"bash %s",tmpFile);
    HERE(cmd);
    if (system(cmd)) {
        ERROR("Command failed, maybe 'convert' not found: ",cmd);
    }
}

void generateLayer3() {
    char tmpDirName[60];
    //memset(tmpDirName,0,60);
    createTmpDir(tmpDirName,15);
    int numFonts=30;
    generateImageOfFont(tmpDirName,numFonts);
    FILE * layer3File = fopen("l3.c","w");
    for (int f=1;f<=numFonts;++f) {
        char font[80];
        //sprintf(font,"%s/digits/reference",CFG_DATAROOTDIR);
        sprintf(font,"%s/%d",tmpDirName,f);

        
        char s[99];
        sprintf(s,"%s/digits/layer_1",CFG_DATAROOTDIR);
        FilterFam * firstLevelFilters=filterFamRead(s);
        
        FilterFam * secondLevelFilters[10];
        for (int i=1;i<10;++i) {
            char baseName[99];
            sprintf(baseName,"%s/digits/layer_2_Digit%d",CFG_DATAROOTDIR,i);
            secondLevelFilters[i] = filterFamRead(baseName);
        }
        HERE("+++");
        HERE(font);
        unsigned char ** outputFromLevel2 = (unsigned char**)malloc(sizeof(char*)*11);
        
        for (int digit=0;digit<10;++digit) {
            outputFromLevel2[digit]=(unsigned char*)malloc(sizeof(char)*11);
        }
        for (int digit=1;digit<10;++digit) {
            char s[99];
            sprintf(s,"%s_%d.png",font,digit);
            Img * tmpimg = newImgRead(s);
            Img * img = imgInvert(tmpimg);
            deleteImg(tmpimg);
            
            ImgFam * firstLayerOutput =
                filterFamApplyConvolution(firstLevelFilters,img);
            
            ImgFam * firstLayerMaxPoolOutput=imgFamDownSampleMax(firstLayerOutput,2,2);
            //ImgFam * avgPool=imgFamDownSampleMax(firstLayerMaxPoolOutput,2,2);
            ImgFam * avgPool = firstLayerMaxPoolOutput;
            HERE("___");
            int maxIndex=-1;
            int maxVal=-1;
            for (int i=1;i<10;++i) {
                Img*singlePixelImg=imgFamScalar(avgPool,secondLevelFilters[i]);
                
                //HERED(singlePixelImg->data[0]);
                outputFromLevel2[digit][i]=singlePixelImg->data[0];
                printf(" %d\n",singlePixelImg->data[0]);
                if (singlePixelImg->data[0]>maxVal) {
                    maxVal=singlePixelImg->data[0];
                    maxIndex=i;
                }
            }
            if (maxIndex!=digit) {
                fprintf(stderr,
                        "\033[31m%s:%d: ||| wrong index %d should be %d\033[m\n",       
                        __FILE__,                                   
                        __LINE__,                                   
                        maxIndex,
                        digit);
            } else {
                HERE("OK");
            }
        }
        for (int digit=1;digit<10;++digit) {
            fprintf(layer3File,"unsigned char tmp_arr_%d_%d[10]={0,",f,digit);
            for (int i=1;i<10;++i) {
                fprintf(layer3File,"%d%s",outputFromLevel2[digit][i],i==9?"":",");
            }
            fprintf(layer3File,"};\n");
        }
    }
    
    fprintf(layer3File,"unsigned char **digits[%d];\n",numFonts+1);    
    fprintf(layer3File,"unsigned char** init_digits() {\n");
    fprintf(layer3File,"    unsigned char** answer = (unsigned char**) malloc(sizeof(char**)*%d*10);\n",numFonts+1);
    for (int f=1;f<=numFonts;++f) {
        fprintf(layer3File,
                "    digits[%d]=answer+10*%d;\n",f,f);
        fprintf(layer3File,
                "    digits[%d][0]=NULL;\n",f);
        for (int digit=1;digit<10;++digit) {
            fprintf(layer3File,
                    // digits[a][b][c] char
                    // digits[a][b]    char*
                    // digits[a]       char**
                    // digits          char***
                    // answer[0]=
                    "    digits[%d][%d]=tmp_arr_%d_%d;\n",
                    f,digit,
                    f,digit);
        }
    }
    fprintf(layer3File,"    return answer;\n");
    fprintf(layer3File,"}\n");
    fclose(layer3File);
    HERE("l3.c written.");
}

void testFilterForDigit(char * font) {

    char s[99];
    sprintf(s,"%s/digits/layer_1",CFG_DATAROOTDIR);
    FilterFam * firstLevelFilters=filterFamRead(s);

    FilterFam * secondLevelFilters[10];
    for (int i=1;i<10;++i) {
        char baseName[99];
        sprintf(baseName,"%s/digits/layer_2_Digit%d",CFG_DATAROOTDIR,i);
        secondLevelFilters[i] = filterFamRead(baseName);
    }
    HERE("+++");
    HERE(font);
    for (int digit=1;digit<10;++digit) {
        char s[99];
        sprintf(s,"%s_%d.png",font,digit);
        Img * tmpimg = newImgRead(s);
        Img * img = imgInvert(tmpimg);
        deleteImg(tmpimg);
        
        ImgFam * firstLayerOutput =
            filterFamApplyConvolution(firstLevelFilters,img);
    
        ImgFam * firstLayerMaxPoolOutput=imgFamDownSampleMax(firstLayerOutput,2,2);
        //ImgFam * avgPool=imgFamDownSampleMax(firstLayerMaxPoolOutput,2,2);
        ImgFam * avgPool=firstLayerMaxPoolOutput;
        HERE("___");
        int maxIndex=-1;
        int maxVal=-1;
        for (int i=1;i<10;++i) {
            Img*singlePixelImg=imgFamScalar(avgPool,secondLevelFilters[i]);
            //HERED(singlePixelImg->data[0]);
            printf(" %d\n",singlePixelImg->data[0]);
            if (singlePixelImg->data[0]>maxVal) {
                maxVal=singlePixelImg->data[0];
                maxIndex=i;
            }
        }
        if (maxIndex!=digit) {
            fprintf(stderr,
                    "\033[31m%s:%d: ||| wrong index %d should be %d\033[m\n",       
                    __FILE__,                                   
                    __LINE__,                                   
                    maxIndex,
                    digit);
        } else {
            HERE("OK");
        }
    }
}


void generateTestData() {
    char tmpDirName[60];
    //memset(tmpDirName,0,60);
    createTmpDir(tmpDirName,15);
    int numFonts=30;
    generateImageOfFont(tmpDirName,numFonts);
    for (int i=1;i<=numFonts;++i) {
        char fontpath[99];
        sprintf(fontpath,"%s/%d",tmpDirName,i);
        testFilterForDigit(fontpath);
        //testFilterForDigit("/Users/fabrice/cnn/share/digits/reference");
        //sprintf(fontpath,"%s/share/digits/reference/%d",CFG_DEFAULT_PREFIX,i);
        //testFilterForDigit(fontpath);
        //testFilterForDigit("/Users/fabrice/git/cnn/dataset/digits/2");
    }
}

/** 
 * @brief Tells how to use this program.
 * @param f where to write the info (stdout or stderr).
 * @param name the value of argv[0].
 */
void digitsUsage(FILE*f,char*name) {
    char * bname=basename(name);
    fprintf(f,"%s usage:\n", bname);
    fprintf(f,"    %s [ options ]\n", bname);
    fprintf(f,"Where option is one of:\n");
    fprintf(f,"    [-c|--create] <n>:\n");
    fprintf(f,"         creates the filters to detect digits for layer <n> \n");
    fprintf(f,"         in directory:\n");
    fprintf(f,"             %s/digits\n",CFG_DATAROOTDIR);
    fprintf(f,"    [-t|--test] <n>:\n");
    fprintf(f,"         tests the filters for digits.\n");
    fprintf(f,"    [-h|--help] :\n");
    fprintf(f,"         display this help message and exits.\n");
}

/**
 * @brief main program for the digit executable. Parses options from 
 * the command line.
 * @param argc number of arguments given
 * @param argv value of arguments.
 * @return 0 if no error occurs.
 */
int digitsMain(int argc,char**argv) {
    int i=1;
    
    while (i<argc) {
        if (strcmp(argv[i],"--create")==0 ||
            strcmp(argv[i],"-c")==0 ) {
            ++i;
            if (i>=argc) {
                digitsUsage(stderr,argv[0]);
                ERROR("-c|--create expects the number of layer to generate","");
            }
            int l = atoi(argv[i]);
            if (l<1 || l>3) {
                digitsUsage(stderr,argv[0]);
                ERROR("-c|--create 2 or 3 as argument","");
            }
            if (l==1) {
                generateDigitsLayer1();
            } else if (l==2) {
                for (int i=1;i<10;++i)
                    // generate layer 2 for digit i
                    generateLayer2(i);
            } else {
                generateLayer3();
            }
        } else if (strcmp(argv[i],"--test")==0 ||
                   strcmp(argv[i],"-t")==0 ) {
            ++i;
            if (i>=argc) {
                digitsUsage(stderr,argv[0]);
                ERROR("-c|--create expects the number of layer to generate","");
            }
            int t=atoi(argv[i]);
            if (t==1) {
                testDigitsLevel1();
            } else if (t==2) {

            } else if (t==3) {
               generateTestData();
            }
        } else if (strcmp(argv[i],"--help")==0 ||
                   strcmp(argv[i],"-h")==0 ) {
            digitsUsage(stdout,argv[0]);
            exit(0);
        } else {
            digitsUsage(stderr,argv[0]);
            ERROR("unknown option: ",argv[i]);
        }
        ++i;
    }
    return 0;
}

