#include <libpng16/png.h>
#include <math.h>
#include <libgen.h>
#include "img.h"


/**
 * @mainpage Project title
 * This is the main page for the project. 
 */

/**
 * update the weight field of the image.
 */
void imgUpdateWeight(Img*img) {
    img->weight=0;
    for(int i = 0; i < img->height*img->width; i++) {
        img->weight+=img->data[i];
    }
}

/**
 * @brief create an image of a given color
 * @param filename name of the file to read
 * @return a newly allocated image.
 */
Img * newImgColor(int w, int h, unsigned char c) {
    Img * answer = (Img *)malloc(sizeof(struct img));
    answer->width=w;
    answer->height=h;
    answer->data=
        (unsigned char*)malloc(answer->height * answer->width);
    memset(answer->data,c,answer->height * answer->width);
    answer->weight=answer->height * answer->width * c;
    return answer;
}
/**
 * @brief create an image from a png file
 * @param filename name of the file to read
 * @return a newly allocated image.
 */
Img * newImgRead(char *filename) {
    Img * answer = (Img *)malloc(sizeof(struct img));
    FILE *fp = fopen(filename, "rb");
    if(!fp) {
        ERROR("file not found - ",filename);
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                             NULL, NULL, NULL);
    
    png_infop info = png_create_info_struct(png);
    if(!info) abort();
    
    if(setjmp(png_jmpbuf(png))) abort();
    
    png_init_io(png, fp);
    
    png_read_info(png, info);
    
    answer->width  = png_get_image_width(png, info);
    answer->height = png_get_image_height(png, info);
    int color_type = png_get_color_type(png, info);
    int bit_depth  = png_get_bit_depth(png, info);
    
    // Read any color_type into 8bit depth, RGBA format.
    // See http://www.libpng.org/pub/png/libpng-manual.txt
    
    if(bit_depth == 16)
        png_set_strip_16(png);
    
    if(color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);
    
    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);
    
    if(png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);
    
    // These color_type don't have an alpha channel then fill it with 0xff.
    if(color_type == PNG_COLOR_TYPE_RGB ||
       color_type == PNG_COLOR_TYPE_GRAY ||
       color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
    
    if(color_type == PNG_COLOR_TYPE_GRAY ||
       color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);
    
    png_read_update_info(png, info);
    
    unsigned char* row_pointers[answer->height];
    answer->data=
        (unsigned char*)malloc(sizeof(char*) * answer->height * answer->width);
    for(int y = 0; y < answer->height; y++) {
        row_pointers[y] =
            (unsigned char*)malloc(png_get_rowbytes(png,info));
    }
    png_read_image(png, row_pointers);
    for(int y = 0; y < answer->height; y++) {
        unsigned char* row = row_pointers[y];
        for(int x = 0; x < answer->width; x++) {
            unsigned char * px = &(row[x * 4]);
            answer->data[x+y*answer->width]=(px[0]+px[1]+px[2])/3;
        }
    }
    for(int y = 0; y < answer->height; y++) {
        free(row_pointers[y]);
    }
    fclose(fp);
    png_destroy_read_struct(&png, &info, NULL);
    imgUpdateWeight(answer);
    return answer;
}

/**
 * @brief create an image from an exiting Img instance
 * @param myPng an existing image to copy
 * @return a newly allocated image.
 */
Img * newImgCopy(Img*myImg) {
    Img * answer = (Img *) malloc(sizeof(struct img));
    
    answer->width      = myImg->width      ;
    answer->height     = myImg->height     ;
    answer->data = (unsigned char*)malloc(sizeof(char) *
                                          answer->height*answer->width);
    memcpy(answer->data,
           myImg->data,
           answer->width*answer->height);
    imgUpdateWeight(answer);
    return answer;
}

/**
 * @brief Deletes an existing image.
 * @param myImg an existing image to copy
 */
void deleteImg(Img*myImg) {
    free(myImg->data);
    myImg->width=0;
    myImg->height=0;
    myImg->weight=0;
    myImg->data=NULL;
    free(myImg);
}

/**
 * @brief Get the value a pixel for a given color.
 * Zero is returned if the pixel is out of the picture.
 * @param p the picture
 * @param x the horizontal position
 * @param y the vertical position 
 * @param ch the channel/color : 0 for red, 1 for green, 2 for blue.
 * @return the value of the pixel.
 */
unsigned char imgGetVal(Img*p,int x, int y) {
    return (y<0 || y>=p->height)?0:(x<0 || x>=p->width)?0:p->data[y*p->width+x];
}

/**
 * Displays an image with numerical values.
 */
void imgPrint(Img*myImg) {
    for(int y = 0; y < myImg->height; y++) {
        for(int x = 0; x < myImg->width; x++) {
            printf("%3d ",imgGetVal(myImg,x,y));
        }
        printf("\n");
    }
    printf("weight: %ld\n",myImg->weight);
}

/**
 * @brief Writes a Img struct to a file 
 * @param myPng an existing image to save to a file
 * @param filename name of the file to save
 */
void imgWrite(Img*myImg,char *filename) {
    int y;
    
    FILE *fp = fopen(filename, "wb");
    if(!fp) {
        ERROR("could not open file ",filename);
    }
    
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        ERROR("could not create png structure","");
    }
    
    png_infop info = png_create_info_struct(png);
    if (!info) {
        ERROR("could not get info","");
    }
    
    if (setjmp(png_jmpbuf(png)))  {
        ERROR("could not jmp to data","");
    }
    
    png_init_io(png, fp);
    
    // Output is 8bit depth, RGBA format.
    png_set_IHDR(png,
                 info,
                 myImg->width, myImg->height,
                 8,
                 PNG_COLOR_TYPE_RGBA,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT
                 );
    
    png_write_info(png, info);
    
    // To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
    // Use png_set_filler().
    //png_set_filler(png, 0, PNG_FILLER_AFTER);
    
    if (!myImg->data) abort();
    
    unsigned char* row_pointers[myImg->height];
    for(int y = 0; y < myImg->height; y++) {
        row_pointers[y] =
            (unsigned char*)malloc(4*myImg->width);
        unsigned char* row = row_pointers[y];
        for(int x = 0; x < myImg->width; x++) {
            unsigned char * px = &(row[x * 4]);
            px[0]=px[1]=px[2]=myImg->data[x+y*myImg->width];
            px[3]=255;
        }
    }
    png_write_image(png, row_pointers);
    
    for(int y = 0; y < myImg->height; y++) {
        free(row_pointers[y]);
    }
    png_write_end(png, NULL);
    fclose(fp);
    png_destroy_write_struct(&png, &info);
}

/**
 * @brief Inverts an image.
 * @param in the input image to invert.
 * @return a newly allocated image which is the invese of image in.
 */
Img* imgInvert(Img*in) {
    Img * answer = newImgCopy(in);
    for(int i = 0; i < in->height*in->width; i++) {
        answer->data[i]=255-in->data[i];
    }
    imgUpdateWeight(answer);
    return answer;
}

/**
 * @brief Flattens the contrast of an image
 * We perform that by raising to the square the 
 * normalized difference with 128.
 * @param in the input image to flatten.
 * @return a newly allocated image which is flattened.
 */
Img* imgFlattenContrast(Img*in) {
    Img * answer = newImgCopy(in);
    for(int i = 0; i < in->height*in->width; i++) {
        int m = in->data[i];
        float v = (m-128.0)/128.;
        float w = v*v;
        answer->data[i]=v>0?INBYTE(w*128+128):INBYTE(128-w*128);
    }
    imgUpdateWeight(answer);
    return answer;
}

/**
 * @brief Raises the contrast of an image
 * We perform that by computing the square root of
 * normalized difference with 128.
 * @param in the input image to flatten.
 * @return a newly allocated image which is flattened.
 */
Img* imgRaiseContrast(Img*in) {
    Img * answer = newImgCopy(in);
    for(int i = 0; i < in->height*in->width; i++) {
        int m = in->data[i];
        float v =(m-128.0)/128.;
        float w = v<0?sqrtf(-v):sqrt(v);
        answer->data[i]=v>0?INBYTE(w*128+128):INBYTE(128-w*128);
    }
    imgUpdateWeight(answer);
    return answer;
}


Img* imgMake3dEffect(Img*in) {
    Img * answer = newImgColor(in->width,in->height+in->width,255);
    
    for(int x = 0; x < in->width; x++) {
        for(int y = 0; y < in->height; y++) {
            answer->data[x + (in->width-x-1+y) * answer->width]=
                in->data[x + y * in->width];
        }
    }
    return answer;
}
/**
 * @brief Scales an image
 * @param in the picture to blur
 * @param s factor to scale
 * @return the newly allocated picture.
 */
Img* imgScale(Img*in,int s) {
    Img * answer = newImgColor(in->width*s,in->height*s,255);
    
    for(int x = 0; x < in->width; x++) {
        for(int y = 0; y < in->height; y++) {
            for(int xx = 0; xx < s; xx++) {
                for(int yy = 0; yy < s; yy++) {
                    answer->data[s*x +xx + (yy+s*y) * answer->width]=
                        in->data[x + y * in->width];
                }
            }
        }
    }
    return answer;
}

/**
 * @brief Blurs a picture.
 * @param in the picture to blur
 * @param radius intensity of the blur
 * @return the newly allocated picture.
 */
Img* imgBlur(Img*in,int radius) {
    Img * answer = newImgCopy(in);
    int sq=radius*radius;
    for(int y = 0; y < in->height; y++) {
        for(int x = 0; x < in->width; x++) {
            int v=0;
            for (int xx=-radius;xx<radius;++xx) {
                for (int yy=-radius;yy<radius;++yy) {
                    if (xx*yy<sq)
                        v+=imgGetVal(in,x+xx,y+yy);
                }
            }
            answer->data[x + y * in->width]=INBYTE((int)(v/sq/3.14));
        }
    }
    imgUpdateWeight(answer);
    return answer;
}

/**
 * @brief Spread luminosity in the picture.
 * @param in the picture to spread luminosity
 * @return the newly allocated picture with spreaded luminosity.
 */
Img* imgLuminosityScale(Img*in) {
    int lumCount[256];
    memset(lumCount,0,sizeof(int)<<8);
    Img * answer = newImgCopy(in);
    for(int i = 0; i < in->height*in->width; i++) {
        lumCount[in->data[i]]++;
    }
    // average value is a celle of lumCount should
    // be avg=(in->height*in->width/256
    // we are going to suppress below and above avg/8
    int threshold=(in->height*in->width)>>12; // divide by 256*8
    if (threshold<2) threshold=2;
    int topLum=255;
    while (topLum>0 && lumCount[topLum]<threshold) --topLum;
    int botLum=0;
    while (botLum<topLum && lumCount[botLum]<threshold) ++botLum;
    //topLum=100;
    for(int i = 0; i < in->height*in->width; i++) {
        answer->data[i]=
            INBYTE((in->data[i]-botLum)*255/(topLum-botLum));
    }
    imgUpdateWeight(answer);
    return answer;
}

long int imgIntFilterMin;
long int imgIntFilterMax;
long int imgIntFilterMaxMinusMin;
long int imgIntFilterMax;


void imgIntFilterMinMaxParam(long int min,long int max) {
    if (max<min)
        ERROR("Max should be larger than min.","");
    imgIntFilterMin=min;
    imgIntFilterMax=max;
    imgIntFilterMaxMinusMin=max-min;
}

unsigned char imgIntFilterMinMax(long int n) {
    if (n<imgIntFilterMin) return 0;
    if (n>imgIntFilterMax) return 255;
    return ((n-imgIntFilterMin)*255)/imgIntFilterMaxMinusMin;
}

/**
 * @brief perform a convolution between an image and a filter
 *     applying a threshold given by intFilter.
 * @param in the input picture 
 * @param filter the filter to use
 * @param intFilter threshold function to apply
 * @return the newly allocated picture with represents the convolution.
 */
Img* imgConvolution(Img*in,Img*filter,IntFilter intFilter) {
    if (in->width<filter->width || in->height<filter->height)
        ERROR("Wrong size","");
    int aw=in->width-filter->width+1;
    int ah=in->height-filter->height+1;
    Img * answer = newImgColor(aw,ah,0);
    for(int y = 0; y < ah; y++) {
        for(int x = 0; x < aw; x++) {
            long int v=0;
            for(int yy = 0; yy < filter->height; yy++) {
                for(int xx = 0; xx < filter->width; xx++) {
                    v+=imgGetVal(in,x+xx,y+yy)*imgGetVal(filter,xx,yy);
                }
            }
            answer->data[x+aw*y]=intFilter(v);
        }
    }
    imgUpdateWeight(answer);
    return answer;
}

/**
 * @brief sets the image to a given weight
 * @param in the image to set
 * @param w the new weight of the image
 * @return the newly created image with a weight close to w
 */
Img* imgSetWeight(Img*in,int w) {
    Img* answer = newImgCopy(in);
    int ww=0;
    for(int i = 0; i < in->height*in->width; i++) {
        ww+=in->data[i];
    }
    float ratio = (w+0.)/(ww+0.);
    
    for(int i = 0; i < in->height*in->width; i++) {
        answer->data[i]=INBYTE((int)round(ratio*in->data[i]));
    }
    imgUpdateWeight(answer);
    return answer;
}

/**
 * @brief Downsample an image using a maxpool strategy.
 * @param img the image to down scale.
 * @param poolsize size of the square on which the maximum is computed.
 * @param stride number of pixel by which the square on which the maximum
 *    is computed is moving at each step.
 * @return the down sampled image.
 */
Img* imgDownSampleMax(Img* img,int poolsize,int stride) {
    if (img->width < poolsize)
        ERROR("Wrong size.","");
    if (img->height < poolsize)
        ERROR("Wrong size.","");
    int w =(img->width - poolsize)/stride+1;
    int h =(img->height - poolsize)/stride+1;
    Img* answer = newImgColor(w,h,0);
    for (int x=0;x<w;++x) {
        for (int y=0;y<h;++y) {
            int max=0;
            for (int xx=0;xx<poolsize;++xx) {
                for (int yy=0;yy<poolsize;++yy) {
                    int v=img->data[x*stride+xx+img->width*(y*stride+yy)];
                    if (v>max)
                        max=v;
                }
            }
            answer->data[x+w*y]=max;
        }
    }
    imgUpdateWeight(answer);
    return answer;
}

/**
 * @brief Downsample an image using an average pool strategy.
 * @param img the image to down scale.
 * @param poolsize size of the square on which the average is computed.
 * @param stride number of pixel by which the square on which the average
 *    is computed is moving at each step.
 * @return the down sampled image.
 */
Img* imgDownSampleAvg(Img* img,int poolsize,int stride) {
    if (img->width < poolsize)
        ERROR("Wrong size.","");
    if (img->height < poolsize)
        ERROR("Wrong size.","");
    int w =(img->width - poolsize)/stride+1;
    int h =(img->height - poolsize)/stride+1;
    Img* answer = newImgColor(w,h,0);
    for (int x=0;x<w;++x) {
        for (int y=0;y<h;++y) {
            int avg=0;
            for (int xx=0;xx<poolsize;++xx) {
                for (int yy=0;yy<poolsize;++yy) {
                    avg+=img->data[x*stride+xx+img->width*(y*stride+yy)];
                }
            }
            answer->data[x+w*y]=avg/poolsize/poolsize;
        }
    }
    imgUpdateWeight(answer);
    return answer;
}

/**
 * @brief Given a large image i1 and a smaller one i2
 *   compute the scaler product of i2 with the subset of i1
 *   at offset (xoffset,yoffset).
 * @param i1 the large image.
 * @param i2 the small image
 * @param xoffset horizontal offset to apply.
 * @param yoffset vertical offset to apply.
 * @return the scalar product of i2 with a sub image of i1.
 */
unsigned char imgScalar(Img*i1,Img*i2,int xoffset,int yoffset) {
    if (i1->width<i2->width+xoffset)
        ERROR("Wrong size","");
    if (i1->height<i2->height+yoffset)
        ERROR("Wrong size","");
    long int s=0;
    for (int y=0;y<i2->height;++y) {
        for (int x=0;x<i2->width;++x) {
            int v=i1->data[x+xoffset+(y+yoffset)*i1->width]
                -i2->data[x+y*i1->width];
            s+=v<0?-v:v;
        }
    }
    int max=i2->height*i2->width*50;
    if (max<s) return 0;
    return INBYTE(255-(s*255)/max);
}

/** 
 * @brief Tells how to use this program.
 * @param f where to write the info.
 * @param name the value of argv[0].
 */
void imgUsage(FILE*f,char*name) {
    char * bname=basename(name);
    fprintf(f,"%s usage:\n", bname);
    fprintf(f,"    %s <input-file> [ options ] <output-file>\n", bname);
    fprintf(f,"Where option is one of:\n");
    fprintf(f,"    [-b|--blur] <r>:\n");
    fprintf(f,"         Blurs the image with radius of r.\n");
    fprintf(f,"    [-g|--grey] :\n");
    fprintf(f,"         Transforms the image in a greyscale image.\n");
    fprintf(f,"    [-l|--lum] :\n");
    fprintf(f,"         Scale luminosity if the image is too bright or too dark.\n");
    fprintf(f,"    [-i|--inv] :\n");
    fprintf(f,"         Inverses the image.\n");
    fprintf(f,"    [-r|--raise] :\n");
    fprintf(f,"         Raises contract level.\n");
    fprintf(f,"    [-f|--flatten] :\n");
    fprintf(f,"         Flattens the contrast.\n");
    fprintf(f,"    [-p|--print] :\n");
    fprintf(f,"         print numerical value of image.\n");
    fprintf(f,"    [-w|--weight] <n>:\n");
    fprintf(f,"         Sets weight of pixels to n.\n");
    fprintf(f,"    [-s|--scale] <n>:\n");
    fprintf(f,"         Scales image by a factor n.\n");
    fprintf(f,"    [-3d|--3d] :\n");
    fprintf(f,"         Draws an isometric-like view.\n");
    fprintf(f,"    [-h|--help] :\n");
    fprintf(f,"         Displays this help message and leaves.\n");
}

/**
 * @brief What to do when directly called from the command line.
 * @param argc number of arguments given
 * @param argv value of arguments.
 * @return 0 if no error occurs.
 */
int imgMain(int argc, char *argv[]) {
    int i=2;
    for (int j=1;j<argc;++j) {
        if (strcmp(argv[j],"-h")==0 || strcmp(argv[j],"--help")==0) {
            imgUsage(stdout,argv[0]);
            exit(0);
        }
    }
    if(argc < 3) {
        imgUsage(stderr,argv[0]);
        ERROR("At least Two arguments expected","");
    }
    Img * currentImage=newImgRead(argv[1]);
    Img * newImage=NULL;
    while (i<argc) {
        if (argv[i][0]!='-') {
            imgWrite(currentImage,argv[i]);
            deleteImg(currentImage);
            exit(0);
        } else {
            if (strcmp(argv[i],"--blur")==0 ||
                strcmp(argv[i],"-b")==0 ) {
                ++i;
                int radius=atoi(argv[i]);
                newImage=imgBlur(currentImage,radius);
            } else if (strcmp(argv[i],"--weight")==0 ||
                strcmp(argv[i],"-w")==0 ) {
                ++i;
                int w=atoi(argv[i]);
                newImage=imgSetWeight(currentImage,w);
            } else if (strcmp(argv[i],"--scale")==0 ||
                strcmp(argv[i],"-s")==0 ) {
                ++i;
                int s=atoi(argv[i]);
                newImage=imgScale(currentImage,s);
            } else if (strcmp(argv[i],"--lum")==0 ||
                strcmp(argv[i],"-l")==0 ) {
                newImage=imgLuminosityScale(currentImage);
            } else if (strcmp(argv[i],"--inv")==0 ||
                strcmp(argv[i],"-i")==0 ) {
                newImage=imgInvert(currentImage);
            } else if (strcmp(argv[i],"--raise")==0 ||
                strcmp(argv[i],"-r")==0 ) {
                newImage=imgRaiseContrast(currentImage);
            } else if (strcmp(argv[i],"--flatten")==0 ||
                strcmp(argv[i],"-f")==0 ) {
                newImage=imgFlattenContrast(currentImage);
            } else if (strcmp(argv[i],"--3d")==0 ||
                strcmp(argv[i],"-3d")==0 ) {
                newImage=imgMake3dEffect(currentImage);
            } else if (strcmp(argv[i],"--print")==0 ||
                strcmp(argv[i],"-p")==0 ) {
                imgPrint(currentImage);
            } else {
                ERROR("Unknown option","");
            }
            ++i;
            deleteImg(currentImage);
            currentImage=newImage;
            newImage=NULL;
        }
    }
    return 0;
}

//int main(int argc, char *argv[]) {
//    return imgMain(argc,argv);
//}
