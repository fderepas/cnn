#include "util.h"
#include <libpng16/png.h>
#include <math.h>
#include <libgen.h>

/*!
 * @brief A structure to store image data.
 * Field row_pointers point to and array of size height.
 * row_pointers[i] for i between 0 and height-1 points
 * to a array of size width of unsigned char storing i-th line 
 * in the picture.
 */
struct mypng {
    /** number of horizontal pixels */
    int width;
    /** number of vertical pixels */
    int height;
    /** This array points to each line in the picture. */
    unsigned char ** row_pointers;
};

/*!
 * @brief Short name for 'struct mypng'
 */
typedef struct mypng MyPng;

/*!
 * @brief create an image from a png file
 * @param filename name of the file to read
 * @return a newly allocated image.
 */
MyPng * newMyPngRead(char *filename) {
    MyPng * answer = (MyPng *)malloc(sizeof(struct mypng));
    FILE *fp = fopen(filename, "rb");
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                             NULL, NULL, NULL);
    if(!png) {
        ERROR("file not found - ",filename);
    }
    
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
    
    answer->row_pointers =
        (unsigned char**)malloc(sizeof(char**) * answer->height);
    for(int y = 0; y < answer->height; y++) {
        answer->row_pointers[y] =
            (unsigned char*)malloc(png_get_rowbytes(png,info));
    }
    
    png_read_image(png, answer->row_pointers);
    
    fclose(fp);
    png_destroy_read_struct(&png, &info, NULL);
    return answer;
}

/*!
 * @brief create an image from an exiting MyPng instance
 * @param myPng an existing image to copy
 * @return a newly allocated image.
 */
MyPng * newMyPngCopy(MyPng*myPng) {
    MyPng * answer = (MyPng *) malloc(sizeof(struct mypng));
    
    answer->width      = myPng->width      ;
    answer->height     = myPng->height     ;
    answer->row_pointers = (unsigned char**)malloc(sizeof(char**) * answer->height);
    for(int y = 0; y < answer->height; y++) {
        answer->row_pointers[y] = (unsigned char*)malloc(4*answer->width);
        memcpy(answer->row_pointers[y],myPng->row_pointers[y],4*answer->width);
    }
    return answer;
}

/*!
 * @brief Deletes an existing image.
 * @param myPng an existing image to copy
 */
void deleteMyPng(MyPng*myPng) {
    for(int y = 0; y < myPng->height; y++) {
        free(myPng->row_pointers[y]);
    }
    free(myPng->row_pointers);
    myPng->width=0;
    myPng->height=0;
    myPng->row_pointers=NULL;
    free(myPng);
}

/*!
 * @brief Writes a MyPng struct to a file 
 * @param myPng an existing image to save to a file
 * @param filename name of the file to save
 */
void myPngWrite(MyPng*myPng,char *filename) {
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
                 myPng->width, myPng->height,
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
    
    if (!myPng->row_pointers) abort();
    
    png_write_image(png, myPng->row_pointers);
    png_write_end(png, NULL);
    fclose(fp);
    png_destroy_write_struct(&png, &info);
}

/*!
 * @brief Inverts an image.
 * @param in the input image to invert.
 * @return a newly allocated image which is the invese of image in.
 */
MyPng* myPngInvert(MyPng*in) {
    MyPng * answer = newMyPngCopy(in);
    for(int y = 0; y < in->height; y++) {
        unsigned char* row = in->row_pointers[y];
        unsigned char* arow = answer->row_pointers[y];
        for(int x = 0; x < in->width; x++) {
            unsigned char * px = &(row[x * 4]);
            unsigned char * ax = &(arow[x * 4]);
            ax[0]=255-px[0];
            ax[1]=255-px[1];
            ax[2]=255-px[2];
        }
    }
    return answer;
}

/*!
 * @brief Flattens the contrast of an image
 * We perform that by raising to the square the 
 * normalized difference with 128.
 * @param in the input image to flatten.
 * @return a newly allocated image which is flattened.
 */
MyPng* myPngFlattenContrast(MyPng*in) {
    MyPng * answer = newMyPngCopy(in);
    for(int y = 0; y < in->height; y++) {
        unsigned char * row = in->row_pointers[y];
        unsigned char * arow = answer->row_pointers[y];
        for(int x = 0; x < in->width; x++) {
            unsigned char * px = &(row[x * 4]);
            unsigned char * ax = &(arow[x * 4]);
            int m=(px[0]+px[1]+px[2])/3;
            for (int ch=0;ch<3;++ch) {
                float v =(px[ch]-128.0)/128;
                float w = v*v;
                ax[ch]=v>0?INBYTE(w*128+128):INBYTE(128-w*128);
            }
        }
    }
    return answer;
}

/*!
 * @brief Raises the contrast of an image
 * We perform that by computing the square root of
 * normalized difference with 128.
 * @param in the input image to flatten.
 * @return a newly allocated image which is flattened.
 */
MyPng* myPngRaiseContrast(MyPng*in) {
    MyPng * answer = newMyPngCopy(in);
    for(int y = 0; y < in->height; y++) {
        unsigned char * row = in->row_pointers[y];
        unsigned char * arow = answer->row_pointers[y];
        for(int x = 0; x < in->width; x++) {
            unsigned char * px = &(row[x * 4]);
            unsigned char * ax = &(arow[x * 4]);
            int m=(px[0]+px[1]+px[2])/3;
            for (int ch=0;ch<3;++ch) {
                float v =(px[ch]-128.0)/128;
                float w = v<0?sqrtf(-v):sqrt(v);
                ax[ch]=v>0?INBYTE(w*128+128):INBYTE(128-w*128);
            }
        }
    }
    return answer;
}

/*!
 * @brief Get the value a pixel for a given color.
 * Zero is returned if the pixel is out of the picture.
 * @param p the picture
 * @param x the horizontal position
 * @param y the vertical position 
 * @param ch the channel/color : 0 for red, 1 for green, 2 for blue.
 * @return the value of the pixel.
 */
int myPngGetVal(MyPng*p,int x, int y, int ch) {
    if (y<0 || y>=p->height) return 0;
    if (x<0 || x>=p->width) return 0;
    unsigned char * row = p->row_pointers[y];
    unsigned char * px = &(row[x * 4]);
    return px[ch];
}

/*!
 * @brief Blurs a picture.
 * @param in the picture to blur
 * @param radius intensity of the blur
 * @return the newly allocated picture.
 */
MyPng* myPngBlur(MyPng*in,int radius) {
    MyPng * answer = newMyPngCopy(in);
    int sq=radius*radius;
    for(int y = 0; y < in->height; y++) {
        unsigned char * row = in->row_pointers[y];
        unsigned char * arow = answer->row_pointers[y];
        for(int x = 0; x < in->width; x++) {
            unsigned char * px = &(row[x * 4]);
            unsigned char * ax = &(arow[x * 4]);
            for (int ch=0;ch<4;++ch) {
                int v=0;
                for (int xx=-radius;xx<radius;++xx) {
                    for (int yy=-radius;yy<radius;++yy) {
                        if (xx*yy<sq)
                            v+=myPngGetVal(in,x+xx,y+yy,ch);
                    }
                }
                ax[ch]=INBYTE((int)(v/sq/3.14));
            }
        }
    }
    return answer;
}

/*!
 * @brief Spread luminosity in the picture.
 * @param in the picture to spread luminosity
 * @return the newly allocated picture with spreaded luminosity.
 */
MyPng* myPngLuminosityScale(MyPng*in) {
    int lumCount[256];
    memset(lumCount,0,sizeof(int)<<8);
    MyPng * answer = newMyPngCopy(in);
    for(int y = 0; y < in->height; y++) {
        unsigned char * row = in->row_pointers[y];
        unsigned char * arow = answer->row_pointers[y];
        for(int x = 0; x < in->width; x++) {
            unsigned char * px = &(row[x * 4]);
            int v=(px[0]+px[1]+px[2])/3;
            lumCount[v]++;
        }
    }
    // average value is a celle of lumCount should
    // be avg=(in->height*in->width/256
    // we are going to suppress below and above avg/8
    int threshold=(in->height*in->width)>>12; // divide by 256*8
    int topLum=255;
    while (topLum>0 && lumCount[topLum]<threshold) --topLum;
    int botLum=0;
    while (botLum<topLum && lumCount[botLum]<threshold) ++botLum;
    //topLum=100;
    for(int y = 0; y < in->height; y++) {
        unsigned char * row = in->row_pointers[y];
        unsigned char * arow = answer->row_pointers[y];
        for(int x = 0; x < in->width; x++) {
            unsigned char * px = &(row[x * 4]);
            unsigned char * ax = &(arow[x * 4]);
            for (int ch=0;ch<4;++ch) {
                ax[ch]=INBYTE((px[ch]-botLum)*255/(topLum-botLum));
            }
        }
    }
    return answer;
}


/*!
 * @brief Remove colors in a picture
 * @param in the picture from which colors should be removed
 * @return the newly allocated greyscale picture
 */
MyPng* myPngGreyscale(MyPng*in) {
    MyPng * answer = newMyPngCopy(in);
    for(int y = 0; y < in->height; y++) {
        unsigned char * row = in->row_pointers[y];
        unsigned char * arow = answer->row_pointers[y];
        for(int x = 0; x < in->width; x++) {
            unsigned char * px = &(row[x * 4]);
            unsigned char * ax = &(arow[x * 4]);
            int v=(px[0]+px[1]+px[2])/3;
            ax[0]=ax[1]=ax[2]=INBYTE(v);
        }
    }
    return answer;
}

/*!
 * @brief Tells how to use this program.
 * @param f where to write the info.
 * @param name the value of argv[0].
 */
void usage(FILE*f,char*name) {
    char * bname=basename(name);
    fprintf(f,"%s usage:\n", bname);
    fprintf(f,"    %s <input-file> [ options ] <output-file>\n", bname);
    fprintf(f,"Where option is one of:\n");
    fprintf(f,"    [-b|--blur] <r>:\n");
    fprintf(f,"         blurs the image with radius of r.\n");
    fprintf(f,"    [-g|--grey] :\n");
    fprintf(f,"         Transforms the image in a greyscale image.\n");
    fprintf(f,"    [-l|--lum] :\n");
    fprintf(f,"         Scale luminosity if the image is too bright or too dark.\n");
    fprintf(f,"    [-i|--inv] :\n");
    fprintf(f,"         inverse the image.\n");
    fprintf(f,"    [-r|--raise] :\n");
    fprintf(f,"         raises contract level.\n");
    fprintf(f,"    [-f|--flatten] :\n");
    fprintf(f,"         flattens the contrast.\n");
    fprintf(f,"    [-h|--help] :\n");
    fprintf(f,"         display the help and leave.\n");
}

int main(int argc, char *argv[]) {
    int i=2;
    for (int j=1;j<argc;++j) {
        if (strcmp(argv[j],"-h")==0 || strcmp(argv[j],"--help")==0) {
            usage(stdout,argv[0]);
            exit(0);
        }
    }
    if(argc < 3) {
        usage(stderr,argv[0]);
        ERROR("At least Two arguments expected","");
    }
    MyPng * currentImage=newMyPngRead(argv[1]);
    MyPng * newImage=NULL;
    while (i<argc) {
        if (argv[i][0]!='-') {
            myPngWrite(currentImage,argv[i]);
            deleteMyPng(currentImage);
            exit(0);
        } else {
            if (strcmp(argv[i],"--blur")==0 ||
                strcmp(argv[i],"-b")==0 ) {
                ++i;
                int radius=atoi(argv[i]);
                newImage=myPngBlur(currentImage,radius);
            } else if (strcmp(argv[i],"--grey")==0 ||
                strcmp(argv[i],"-g")==0 ) {
                newImage=myPngGreyscale(currentImage);
            } else if (strcmp(argv[i],"--lum")==0 ||
                strcmp(argv[i],"-l")==0 ) {
                newImage=myPngLuminosityScale(currentImage);
            } else if (strcmp(argv[i],"--inv")==0 ||
                strcmp(argv[i],"-i")==0 ) {
                newImage=myPngInvert(currentImage);
            } else if (strcmp(argv[i],"--raise")==0 ||
                strcmp(argv[i],"-r")==0 ) {
                newImage=myPngRaiseContrast(currentImage);
            } else if (strcmp(argv[i],"--flatten")==0 ||
                strcmp(argv[i],"-f")==0 ) {
                newImage=myPngFlattenContrast(currentImage);
            } else {
                ERROR("Unknown option","");
            }
            ++i;
            deleteMyPng(currentImage);
            currentImage=newImage;
            newImage=NULL;
        }
    }

    return 0;
}
