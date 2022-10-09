#include <libpng16/png.h>
#include <math.h>
#include <libgen.h>
#include <jpeglib.h>
#include <setjmp.h>

#include "img.h"
#include "filter.h"

/**
 * Data structure errors when manipulating jpeg.
 */
struct jpegerrmgr {
    /** "public" fields */
    struct jpeg_error_mgr pub;
    /** for return to caller */
    jmp_buf setjmp_buffer;
};

/**
 * @brief Pointer to struct jpegerrmgr.
 */
typedef struct jpegerrmgr * MyErrorPtr;

/**
 * @brief Creates an image from an array of unsigned char.
 * @param w width of the picture in pixels
 * @param h height of the picture in pixels
 * @param buffer a buffer of w times h grey pixels.
 * @return corresponding structure to the data in the buffer
 */
Img * newImgFromArray(int w, int h, unsigned char * buffer) {
    Img * answer = newImgColor(w,h,0);
    memcpy(answer->data,buffer,w*h);
    return answer;
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
    return answer;
}

/**
 * @brief create an image of a vertical black bar on a square white background
 * @param s size of the image
 * @param w width of the back bar
 * @return a newly allocated image.
 */
Img * newImgVerticalBar(int s,int w) {
    Img * answer = newImgColor(s,s,255);
    int m=s/2;
    for (int x=m-w/2;x<m-w/2+w;++x) {
        for (int y=0;y<s;++y) {
            answer->data[x+s*y]=0;
        }
    }
    return answer;
}

/**
 * @brief create an image of a vertical black bar on a rectangular
 *        white background
 * @param sx horizontal size of the image
 * @param sy vertical size of the image
 * @param w width of the back bar
 * @return a newly allocated image.
 */
Img * newImgVerticalBarInRect(int sx, int sy, int w) {
    Img * answer = newImgColor(sx,sy,255);
    int m=sx/2;
    if (m<w) {
        ERROR("Wrong size.","");
    }
    // put some grey arond the bar so that the
    // equivalent filter is 0
    for (int x=m-w;x<=m+w;++x) {
        for (int y=0;y<sy;++y) {
            answer->data[x+sx*y]=128;
        }
    }
    for (int x=m-w/2;x<m-w/2+w;++x) {
        for (int y=0;y<sy;++y) {
            answer->data[x+sx*y]=0;
        }
    }
    return answer;
}
/**
 * @brief create a 3x3 image to perform edge detection.
 * @return a newly allocated image.
 */
Img * newImg33edge() {
    int c=16;
    int b=c>>2;
    unsigned char a[] = {
        128+c-b, 128+c+b, 128+c-b,
        128+c+b, 128-8*c, 128+c+b,
        128+c-b, 128+c+b, 128+c-b
    };
    return newImgFromArray(3,3,a);
}

/**
 * @brief create an image with a black cross
 *        on a w by w white background.
 * @param s size of the cross
 * @param w side of the image
 * @param t thickness of the cross
 * @return a newly allocated image.
 */
Img * newImgCross(int s,int w,int t) {
    Img * answer = newImgColor(w,w,255);
    int m=w/2;
    int count=0;
    while (count<t && count<m) {
        if (s<w-1 && s>0) {
            for (int y=m-s/2;y<m-s/2+s;++y) {
                answer->data[m+count+w*y]=0;
            }
            for (int x=m-s/2;x<m-s/2+s;++x) {
                answer->data[x+w*(m+count)]=0;
            }
            for (int y=m-s/2;y<m-s/2+s;++y) {
                answer->data[m-count+w*y]=0;
            }
            for (int x=m-s/2;x<m-s/2+s;++x) {
                answer->data[x+w*(m-count)]=0;
            }
        }
        count++;
    }
    return answer;
}

/**
 * @brief create an image with a black square
 * on a w by w white background.
 * @param s side of the square in pixel
 * @param w side of the image
 * @param t thickness
 * @return a newly allocated image.
 */
Img * newImgSquare(int s,int w,int t) {
    Img * answer = newImgColor(w,w,255);
    int m=w/2;
    int count=0;
    while (count<t) {
        if (s<w-1 && s>0) {
            for (int y=m-s/2;y<m-s/2+s;++y) {
                answer->data[m-s/2+w*y]=0;
            }
            for (int y=m-s/2;y<m-s/2+s;++y) {
                answer->data[m-s/2+s-1+w*y]=0;
            }
            for (int x=m-s/2;x<m-s/2+s;++x) {
                answer->data[x+w*(m-s/2)]=0;
            }
            for (int x=m-s/2;x<m-s/2+s;++x) {
                answer->data[x+w*(m-s/2+s-1)]=0;
            }
        }
        count++;
        s--;
    }
    return answer;
}

/**
 * @brief create an image with 9x9 black dots
 * on a picture of size w times w.
 * @param w side of the picture
 * @return a newly allocated image.
 */
Img * newImg9By9Dots(int w) {
    Img * answer = newImgColor(w,w,255);
    float step=w/9.;
    float stepOverTwo=step/2.;
    float stepOverThree=step/2;
    float stepOverThreeSq=stepOverThree*stepOverThree;
    for (int x=0;x<w;++x) {
        for (int y=0;y<w;++y) {
            float xr = round((x-stepOverTwo)/step)*step+stepOverTwo;
            float yr = round((y-stepOverTwo)/step)*step+stepOverTwo;
            float d = ((x-xr)*(x-xr)+(y-yr)*(y-yr))/stepOverThreeSq;
            if (d<1) {
                answer->data[x+w*y]=INBYTE((int)(d*255));
            }
        }
    }
    return answer;
}

/**
 * @brief Creates an image with equaly separated N black dots
 *        on a picture of size w by 1 pixel.
 *
 *        First point is located at w/N/2 from the left border of
 *        the picture, then second at w/2/N+w/N,
 *        third at w/2/N+2*w/N, N-th point is located  at 
 *        w/2/N+(N-1)*w/N=w-w/2/N.
 *        So N-th point is at w/N/2 from the right border of
 *        the picture.
 * @param N number of back dots to put
 * @param w width of the picture
 * @return a newly allocated image.
 */
Img * newImgNDotsHori(int N, int w) {
    // create a w by 1 white image
    Img * answer = newImgColor(w,1,255);
    // space between dots is w/N
    float step=((float)w)/((float)N);
    // we begin at step/2 from the border
    float stepOverTwo=step/2.;
    float stepOverFour=step/4;
    float stepOverFourSq=stepOverFour*stepOverFour;
    for (int x=0;x<w;++x) {
        // horizontal position of the closest black dot
        float xr = round((x-stepOverTwo)/step)*step+stepOverTwo;
        // compute distance to the closest black dot
        float d = fabs(xr-x);
        float dmax=3;
        if (d<dmax) {
            // we are close to a black dot so we darken the pixel
            answer->data[x]=INBYTE((int)(d*255/dmax));
        }
    }
    return answer;
}

/**
 * @brief create an image of a given color
 * @param filename name of the file to read
 * @return a newly allocated image.
 */
Img * newImgSudoku(int sz) {
    int w=212;
    int h=212;
    Img * answer = newImgColor(w,h,255);
    int xinit=224/2-sz/2;
    int yinit=xinit;
    int step=sz/9;
    for (int x=0;x<10;++x) {
        int xx=xinit+x*step;
        for (int yy=yinit;yy<yinit+step*9-1;++yy) {
            if (xx>=0 && yy>=0 && xx<w && yy<h)
                answer->data[xx+w*yy]=0;
        }
    }
    answer->data[1+xinit+9*step+w*yinit]=0;
    answer->data[xinit+w*(yinit+1+9*step)]=0;
    for (int y=0;y<10;++y) {
        int yy=yinit+y*step;
        for (int xx=xinit;xx<xinit+step*9-1;++xx) {
            if (xx>=0 && yy>=0 && xx<w && yy<h)
                answer->data[xx+w*yy]=0;
        }
    }
    for (int x=0;x<=3;++x) {
        int xx=xinit+x*step*3;
        for (int yy=yinit;yy<yinit+step*9-1;++yy) {
            if (xx>=0 && yy>=0 && xx<w && yy<h)
                answer->data[1+xx+w*(yy+1)]=0;
        }
    }
    for (int y=0;y<=3;++y) {
        int yy=yinit+y*step*3;
        for (int xx=xinit;xx<xinit+step*9-1;++xx) {
            if (xx>=0 && yy>=0 && xx<w && yy<h)
                answer->data[1+xx+w*(yy+1)]=0;
        }
    }
    return answer;
}

/**
 * @brief used to manage jpeg errors.
 */
void my_error_exit (j_common_ptr cinfo) {
    /* cinfo->err really points to a jpegerrmgr struct, so coerce pointer */
    MyErrorPtr myerr = (MyErrorPtr) cinfo->err;
    
    /* Always display the message. */
    /* We could postpone this until after returning, if we chose. */
    (*cinfo->err->output_message) (cinfo);

    /* Return control to the setjmp point */
    longjmp(myerr->setjmp_buffer, 1);
}

/**
 * @brief create an image from a jpeg file
 * @param filename name of the file to read
 * @return a newly allocated image.
 */
static Img * newImgReadJpeg(char *filename) {
    struct jpeg_decompress_struct cinfo;
    struct jpegerrmgr jerr;
    FILE * infile;		/* source file */
    JSAMPARRAY buffer;		/* Output row buffer */
    int row_stride;		/* physical row width in output buffer */
    
    if ((infile = fopen(filename, "rb")) == NULL) {
        ERROR("can't open ", filename);
    }
    
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;
    
    if (setjmp(jerr.setjmp_buffer)) {
        // JPEG code has signaled an error.
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        ERROR("Error in jpeg file ", filename);
    }
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    
    (void) jpeg_read_header(&cinfo, TRUE);
    
    (void) jpeg_start_decompress(&cinfo);
    
    row_stride = cinfo.output_width * cinfo.output_components;
    buffer = (*cinfo.mem->alloc_sarray)
        ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
    int lineCount=0;
    Img * answer = newImgColor(cinfo.image_width,cinfo.image_height,0);
    // read the image line by line
    while (cinfo.output_scanline < cinfo.output_height) {
        (void) jpeg_read_scanlines(&cinfo, buffer, 1);
        for (int i=0;i<row_stride;++i) {
            int j = (buffer[0][i]+buffer[0][i+1]+buffer[0][i+2])/3;
            answer->data[i/3+lineCount*cinfo.image_width]=j;
        }
        ++lineCount;
    }
    (void) jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return answer;
}

/**
 * @brief create an image from a png file
 * @param filename name of the file to read
 * @return a newly allocated image.
 */
static Img * newImgReadPng(char *filename) {
    Img * answer = (Img *)malloc(sizeof(struct img));
    FILE *fp = fopen(filename, "rb");
    if(!fp) {
        ERROR("file not found - ",filename);
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                             NULL, NULL, NULL);
    
    png_infop info = png_create_info_struct(png);
    if(!info) abort();
    
    if(setjmp(png_jmpbuf(png))) {
        ERROR("Error","");
    }
    
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
    return answer;
}

/**
 * @brief create an image from a png or jpeg file
 * @param filename name of the file to read
 * @return a newly allocated image.
 */
Img * newImgRead(char *filename) {
    int l = strlen(filename);
    if ( (l>4 &&
          filename[l-4]=='.' &&
          (filename[l-3]=='j'|| filename[l-3]=='J')&&
          (filename[l-2]=='p'|| filename[l-2]=='P')&&
          (filename[l-1]=='g'|| filename[l-1]=='G'))
         ||
         (l>5 &&
          filename[l-5]=='.' &&
          (filename[l-4]=='j'|| filename[l-4]=='J')&&
          (filename[l-3]=='p'|| filename[l-3]=='P')&&
          (filename[l-2]=='e'|| filename[l-2]=='E')&&
          (filename[l-1]=='g'|| filename[l-1]=='G'))) {
        return newImgReadJpeg(filename);
    }
    // otherwise we assume it is a png
    return newImgReadPng(filename);
}

/**
 * @brief create an image from an exiting Img instance
 * @param myImg an existing image to copy
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
    return answer;
}

/**
 * @brief Deletes an existing image.
 * @param myImg an existing image to delete
 *        Nothing happens if myImg is NULL.
 */
void deleteImg(Img*myImg) {
    if (myImg==NULL) return;
    free(myImg->data);
    myImg->width=0;
    myImg->height=0;
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
 * @brief Displays an image with numerical values.
 * @param myImg image to display in the terminal.
 */
void imgPrint(Img*myImg) {
    for(int y = 0; y < myImg->height; y++) {
        for(int x = 0; x < myImg->width; x++) {
            printf("%3d ",imgGetVal(myImg,x,y));
        }
        printf("\n");
    }
}

/**
 * @brief draws a rectangle on the image
 * @param myImg image on which to draw the rectangle
 * @param xmin horizontal position of lower left corner 
 * @param ymin vertical position of lower left corner 
 * @param xmax horizontal position of upper right corner 
 * @param ymax vertical position of upper right corner 
 */
void imgDrawRect(Img*myImg,int xmin,int ymin,int xmax,int ymax) {
    if (xmin<0 || ymin<0 || xmax >myImg->width || myImg->height<ymax ||
        xmin>=xmax || ymin>=ymax ) {
        fprintf(stderr,"rectangle : %d %d %d %d\n",xmin,ymin,xmax,ymax);
        fprintf(stderr,"image size is %d %d\n",myImg->width,myImg->height);
        ERROR("Wrong size.","");
    }
    for(int x = xmin; x <xmax; x++) {
        myImg->data[x+myImg->width*ymin]=(x>>2)%2?255:0;
        myImg->data[x+myImg->width*ymax]=(x>>2)%2?255:0;
    }
    for(int y = ymin; y <ymax; y++) {
        myImg->data[xmin+myImg->width*y]=(y>>2)%2?255:0;
        myImg->data[xmax+myImg->width*y]=(y>>2)%2?255:0;
    }
    
}

/**
 * @brief Writes a Img struct to a png file.
 * @param myImg an existing image to save to a file.
 * @param filename name of the png to write.
 */
void imgWrite(Img*myImg,char *filename) {
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
        fprintf(stderr,
                "could not jmp to data while trying to write %s\n",
                filename);
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
 * @brief Return the sum of all pixels in the picture.
 * @param in a picture
 * @return the sum of all pixels.
 */
int imgGetWeight(Img*in) {
    int answer =0;
    for(int i = 0; i < in->height*in->width; i++) {
        answer+=in->data[i];
    }
    return answer;
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
 * @brief Scales an image to a larger image.
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
    return answer;
}

/**
 * @brief perform a convolution between an image and a filter
 *        with unsigned char.
 *
 * The resulting image size is in->width-filter->img->width+1 by
 * in->height-filter->img->height+1.
 * @param in the input picture 
 * @param filter the filter to use
 * @return the newly allocated picture with represents the convolution.
 */
Img* imgConvolution(Img*in,Filter*filter) {
    if (in->width<filter->img->width)
        ERROR("Wrong width","");
    if (in->height<filter->img->height)
        ERROR("Wrong height","");
    int aw=in->width-filter->img->width+1;
    int ah=in->height-filter->img->height+1;
    Img * answer = newImgColor(aw,ah,0);
    for(int y = 0; y < ah; y++) {
        for(int x = 0; x < aw; x++) {
            long int v=0;
            for(int yy = 0; yy < filter->img->height; yy++) {
                for(int xx = 0; xx < filter->img->width; xx++) {
                    v+=imgGetVal(in,x+xx,y+yy)*imgGetVal(filter->img,xx,yy);
                }
            }
            answer->data[x+aw*y]=
                (v<filter->threshold)?0
                :(v>filter->maxVal)?255
                :(255*(v-filter->threshold))/(filter->maxVal-filter->threshold);
        }
    }
    return answer;
}

/**
 * @brief perform a convolution between an image and a filter
 *        with unsigned char.
 *
 * The resulting image size is in->width-filter->img->width+1 by
 * in->height-filter->img->height+1.
 * @param in the input picture 
 * @param filter the filter to use
 * @return the newly allocated picture with represents the convolution.
 */
Img* imgConvolutionDiff(Img*in,Filter*filter) {
    if (in->width<filter->img->width)
        ERROR("Wrong width","");
    if (in->height<filter->img->height)
        ERROR("Wrong height","");
    int aw=in->width-filter->img->width+1;
    int ah=in->height-filter->img->height+1;
    Img * answer = newImgColor(aw,ah,0);
    for(int y = 0; y < ah; y++) {
        for(int x = 0; x < aw; x++) {
            long int v=0;
            for(int yy = 0; yy < filter->img->height; yy++) {
                for(int xx = 0; xx < filter->img->width; xx++) {
                    v+=((int)imgGetVal(in,x+xx,y+yy))*
                        (((int)imgGetVal(filter->img,xx,yy))-128);
                }
            }
            answer->data[x+aw*y]=
                (v<filter->threshold)?0
                :(v>filter->maxVal)?255
                :(255*(v-filter->threshold))/(filter->maxVal-filter->threshold);
        }
    }
    return answer;
}

/**
 * @brief perform a convolution between an image and a filter
 *     applying a threshold given by intFilter and only positive numbers.
 *
 * The resulting image size is in->width by in->height.
 * @param in the input picture 
 * @param filter the filter to use
 * @return the newly allocated picture with represents the convolution.
 */
Img* imgConvolutionSameSize(Img*in,Filter*filter) {
    if (in->width<filter->img->width || in->height<filter->img->height)
        ERROR("Wrong size","");
    int wfOver2=filter->img->width/2;
    int hfOver2=filter->img->height/2;
    Img * answer = newImgColor(in->width,in->height,0);
    for(int y = 0; y < in->height; y++) {
        for(int x = 0; x < in->width; x++) {
            long int v=0;
            for(int yy = 0; yy < filter->img->height; yy++) {
                for(int xx = 0; xx < filter->img->width; xx++) {
                    int xxx=x+xx-wfOver2;
                    int yyy=y+yy-hfOver2;
                    if (xxx>=0 && yyy>=0 && xxx<in->width && yyy<in->height) {
                        v+=imgGetVal(in,xxx,yyy)*imgGetVal(filter->img,xx,yy);
                    }
                }
            }
            //HERE("___v,maxval,threshold,percent__________________");
            //HERED((int)v);
            //HERED((int)filter->maxVal);
            //HERED((int)filter->threshold);
            //HERED((int)filter->percent);
            answer->data[x+in->width*y]=
                (v<filter->threshold)?0
                :(v>filter->maxVal)?255
                :(255*(v-filter->threshold))/(filter->maxVal-filter->threshold);
        }
    }
    return answer;
}
/**
 * @brief perform a convolution between an image and a filter
 *     applying a threshold given by intFilter.
 *
 * The resulting image size is in->width by in->height.
 * @param in the input picture 
 * @param filter the filter to use
 * @return the newly allocated picture with represents the convolution.
 */
Img* imgConvolutionSameSizeDiff(Img*in,Filter*filter) {
    if (in->width<filter->img->width || in->height<filter->img->height)
        ERROR("Wrong size","");
    int wfOver2=filter->img->width/2;
    int hfOver2=filter->img->height/2;
    Img * answer = newImgColor(in->width,in->height,0);
    for(int y = 0; y < in->height; y++) {
        for(int x = 0; x < in->width; x++) {
            long int v=0;
            for(int yy = 0; yy < filter->img->height; yy++) {
                for(int xx = 0; xx < filter->img->width; xx++) {
                    int xxx=x+xx-wfOver2;
                    int yyy=y+yy-hfOver2;
                    if (xxx>=0 && yyy>=0 && xxx<in->width && yyy<in->height) {
                        
                        v+=((int)imgGetVal(in,xxx,yyy))*
                            (((int)imgGetVal(filter->img,xx,yy))-128);
                        /*
                        int d = imgGetVal(in,xxx,yyy)
                            -imgGetVal(filter->img,xx,yy);
                        v+=d*d;
                        */
                    }
                }
            }
            /*
            HERE("___v,maxval,threshold,percent__________________");
            HERED((int)v);
            HERED((int)filter->maxVal);
            HERED((int)filter->threshold);
            HERED((int)filter->percent);
            */

            answer->data[x+in->width*y]=
                (v<filter->threshold)?0
                :(v>filter->maxVal)?255
                :(255*(v-filter->threshold))/(filter->maxVal-filter->threshold);
        }
    }
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
        ERROR("Wrong size, poolsize too large.","");
    if (img->height < poolsize)
        ERROR("Wrong size, poolsize too large.","");
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
    return answer;
}

/**
 * @brief Divide the size of an image by two.
 * @param img an image
 * @return same image as img but with a size scaled down by 2.
 */
Img* imgDivideByTwo(Img* img) {
    Img * answer = newImgColor(img->width/2,img->height/2,255);
    for (int y=0;y<img->height/2;++y) {
        int o = (img->width>>1)*y;
        for (int x=0;x<img->width/2;++x) {
            // compute the average of 4 pixels
            answer->data[x+o] =
                (img->data[(x<<1)+img->width*(y<<1)] +
                 img->data[(x<<1)+1+img->width*(y<<1)] +
                 img->data[(x<<1)+img->width*((y<<1)+1)] +
                 img->data[(x<<1)+1+img->width*((y<<1)+1)] )>>2;
        }
    }
    return answer;
}

/**
 * @brief Rotates an image by 90 degrees.
 * 
 * Resulting image exchange height and width with original image.
 * @param img to rotate
 * @return rotated image
 */
Img* imgRotate90(Img* img) {
    // exchange height and width
    Img * answer = newImgColor(img->height,img->width,255); 
    for (int y=0;y<answer->height;++y) {
        int o = answer->width*y;
        for (int x=0;x<answer->width;++x) {
            answer->data[x+o]=img->data[y+img->width*x];
        }
    }
    return answer;
}

/**
 * @brief Rotates an image
 * @param img to rotate
 * @param deg degrees to rotate
 * @return the down sampled image.
 * @see http://www.leptonica.org/rotation.html
 */
Img* imgRotate(Img* img,int deg) {
    Img * answer = newImgColor(img->width,img->height,255);
    float rad=deg*3.14159/180;
    float c = cos(rad);
    float s = sin(rad);
    int w=img->width;
    int h=img->height;
    float eps=0.0001;    // epsilum, considered as zero   
    for (int x=0;x<w;++x) {
        for (int y=0;y<h;++y) {
            double xd= (x-w/2)*c - (y-h/2)*s + w/2;
            double yd= (x-w/2)*s + (y-h/2)*c + h/2;
            if (xd>0 && yd>0 && xd<w-1 && yd<h-1) {
                double xf=floor(xd);
                double xc=ceil(xd);
                double yf=floor(yd);
                double yc=ceil(yd);
                double ff=sqrt((xd-xf)*(xd-xf)+(yd-yf)*(yd-yf));
                double fc=sqrt((xd-xf)*(xd-xf)+(yd-yc)*(yd-yc));
                double cf=sqrt((xd-xc)*(xd-xc)+(yd-yf)*(yd-yf));
                double cc=sqrt((xd-xc)*(xd-xc)+(yd-yc)*(yd-yc));
                int v=255;
                if (-eps<ff && ff<eps) {
                    v=img->data[(int)(xf+w*yf)];
                } else if (-eps<fc && fc<eps) {
                    v=img->data[(int)(xf+w*yc)];
                } else if (-eps<cf && cf<eps) {
                    v=img->data[(int)(xc+w*yf)];
                } else if (-eps<cc && cc<eps) {
                    v=img->data[(int)(xc+w*yc)];
                } else {
                    // not too close to any point, so compute average
                    double t=1/ff+1/fc+1/cf+1/cc;
                    v=INBYTE((int)((img->data[(int)(xf+w*yf)]/ff+
                                    img->data[(int)(xf+w*yc)]/fc+
                                    img->data[(int)(xc+w*yf)]/cf+
                                    img->data[(int)(xc+w*yc)]/cc)/t));
                }
                answer->data[x+w*y]=v;
            }
        }
    }
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
 * @param f where to write the info (stdout or stderr).
 * @param name the value of argv[0].
 */
void imgUsage(FILE*f,char*name) {
    char * bname=basename(name);
    fprintf(f,"%s usage:\n", bname);
    fprintf(f,"    %s [<input-file>] [ options ] <output-file>\n", bname);
    fprintf(f,"Where option is one of:\n");
    fprintf(f,"    [-b|--blur] <r>:\n");
    fprintf(f,"        Blurs the image with a radius of r.\n");
    fprintf(f,"    [-c|--contrast] :\n");
    fprintf(f,"        Raises contrast level.\n");
    fprintf(f,"    --conv <filename> <percent>:\n");
    fprintf(f,"        load image in <filename> and use it as a convolution\n");
    fprintf(f,"        filter with a threshold set at <percent>.\n");
    fprintf(f,"    --cross <n> <w> <t>:\n");
    fprintf(f,"        Generates a black cross on a white background. \n");
    fprintf(f,"        The cross size is n by n pixels in a w by w\n");
    fprintf(f,"        picture. The cross line is <t> pixels thick.\n");
    fprintf(f,"    [-f|--flatten] :\n");
    fprintf(f,"        Flattens the contrast.\n");
    fprintf(f,"    [-h|--help] :\n");
    fprintf(f,"        Displays this help message and leaves.\n");
    fprintf(f,"    [-i|--inv] :\n");
    fprintf(f,"        Inverses the image. White becomes black, black \n");
    fprintf(f,"        becomes white.\n");
    fprintf(f,"    [-l|--lum] :\n");
    fprintf(f,"        Scales luminosity if the image is too bright or too dark.\n");
    fprintf(f,"    [-p|--print] :\n");
    fprintf(f,"        Prints on stdout numerical value of current image.\n");
    fprintf(f,"    [-s|--scale] <n>:\n");
    fprintf(f,"        Scales image by a factor n.\n");
    fprintf(f,"    --square <n> <w> <t>:\n");
    fprintf(f,"        Generates a black square on a white background. \n");
    fprintf(f,"        The square size is n by n pixels in a w by w\n");
    fprintf(f,"        picture. The square line is <t> pixels thick.\n");
    fprintf(f,"    --sudoku <n>:\n");
    fprintf(f,"        Generates a black empty sudoku grid of n pixels large in a \n");
    fprintf(f,"        224 by 224 picture with white background.\n");
    fprintf(f,"    [-r|--rotate] <n>:\n");
    fprintf(f,"        Rotates an image by n degrees.\n");
    fprintf(f,"    [-v|--vertical] <s> <w>:\n");
    fprintf(f,"        Generates a black vertical bar of width <w>\n");
    fprintf(f,"        in a white square of size <s>.\n");
    fprintf(f,"    --version :\n");
    fprintf(f,"        Displays the current version of %s.\n",bname);
    fprintf(f,"    --1d-dots <n> <w>:\n");
    fprintf(f,"        Generates <n> black dots on a white background, \n");
    fprintf(f,"        in one dimension: the generated image size is <w> \n");
    fprintf(f,"        by 1 pixels.\n");
    fprintf(f,"    --3x3-edge :\n");
    fprintf(f,"        Generates a 3x3 image to perform edge detection if \n");
    fprintf(f,"        used as convolution filter.\n");
    fprintf(f,"    [-3d|--3d] :\n");
    fprintf(f,"        Draws an isometric-like view.\n");
    fprintf(f,"        in a 224x224 image.\n");
    fprintf(f,"    --9x9dots <n>:\n");
    fprintf(f,"        Generates 9x9 black dots on a white background. \n");
    fprintf(f,"        The image size is n by n pixels.\n");
                                                                
    fprintf(f,"Examples:\n");
    fprintf(f,"    %s --sudoku 200 --rotate 5 --blur 2 out.png\n",bname);
    fprintf(f,"        Generates an empty sudoku grid, then rotate it by 5\n");
    fprintf(f,"        degrees, then blurs the image with a radius of 2, then\n");
    fprintf(f,"        save the result in file out.png.\n");
    fprintf(f,"    \n");
    fprintf(f,"    %s --3x3-edge edgefilter.png\n",bname);
    fprintf(f,"        Creates a 3x3 filter to be used for edge detection.\n");
    fprintf(f,"        Save it under edgefilter.png.\n");
    fprintf(f,"    \n");
    fprintf(f,"    %s mypic.png --conv edgefilter.png -10 out.png\n",
            bname);
    fprintf(f,"        Apply the convolution filter edgefilter.png to\n");
    fprintf(f,"        mypic.png and save the result under out.png.\n");
    fprintf(f,"        -10%% of threshold is applied. Put a lower percentage\n");
    fprintf(f,"        for brighter images, a higher one for a darker.\n");
    fprintf(f,"    \n");
    
    fprintf(f,"    %s my_photo.jpg --lum my_new_photo.png\n",bname);
    fprintf(f,"        Reads file my_photo.jpg increase luminosity to stretch from\n");
    fprintf(f,"        dark to very clear and save the result in my_new_photo.png.\n");
}

/**
 * @brief What to do when directly called from the command line.
 * @param argc number of arguments given
 * @param argv value of arguments.
 * @return 0 if no error occurs.
 */
int imgMain(int argc, char *argv[]) {
    if (argc==1) {
        imgUsage(stdout,argv[0]);
        exit(0);
    }
    for (int j=1;j<argc;++j) {
        if (strcmp(argv[j],"-h")==0 || strcmp(argv[j],"--help")==0) {
            imgUsage(stdout,argv[0]);
            exit(0);
        }
    }
    int i=1;
    Img * currentImage=NULL;
    if (argv[i][0]!='-') {
        currentImage=newImgRead(argv[1]);
        i=2;
    }
#define NOT_ENOUGH                                        \
    if (i>=argc) {                                        \
        imgUsage(stderr,argv[0]);                         \
        ERROR("not enough arguments.","");                \
    }

    Img * newImage=NULL;
    while (i<argc) {
        if (argv[i][0]!='-') {
            imgWrite(currentImage,argv[i]);
        } else {
            if (strcmp(argv[i],"--blur")==0 ||
                strcmp(argv[i],"-b")==0 ) {
                ++i;
                NOT_ENOUGH;
                int s=atoi(argv[i]);
                newImage=imgBlur(currentImage,s);
            } else if (strcmp(argv[i],"--cross")==0) {
                ++i;
                NOT_ENOUGH;
                int s=atoi(argv[i]);
                ++i;
                NOT_ENOUGH;
                int w=atoi(argv[i]);
                ++i;
                NOT_ENOUGH;
                int t=atoi(argv[i]);
                newImage=newImgCross(s,w,t);
            } else if (strcmp(argv[i],"--square")==0) {
                ++i;
                NOT_ENOUGH;
                int s=atoi(argv[i]);
                ++i;
                NOT_ENOUGH;
                int w=atoi(argv[i]);
                ++i;
                NOT_ENOUGH;
                int t=atoi(argv[i]);
                newImage=newImgSquare(s,w,t);
            } else if (strcmp(argv[i],"--conv")==0) {
                ++i;
                NOT_ENOUGH;
                char * fileName = argv[i];
                ++i;
                NOT_ENOUGH;
                int percent=atoi(argv[i]);
                Filter * f =newFilter(newImgRead(fileName),percent);
                newImage=imgConvolutionSameSizeDiff(currentImage,f);
                deleteFilter(f);
            } else if (strcmp(argv[i],"--9x9dots")==0) {
                ++i;
                NOT_ENOUGH;
                int w=atoi(argv[i]);
                newImage=newImg9By9Dots(w);
            } else if (strcmp(argv[i],"--3x3-edge")==0) {
                newImage=newImg33edge();
            } else if (strcmp(argv[i],"--1d-dots")==0) {
                ++i;
                NOT_ENOUGH;
                int N=atoi(argv[i]);
                ++i;
                NOT_ENOUGH;
                int w=atoi(argv[i]);
                newImage=newImgNDotsHori(N,w);
            } else if (strcmp(argv[i],"--vertical")==0 ||
                strcmp(argv[i],"-v")==0 ) {
                ++i;
                NOT_ENOUGH;
                int s=atoi(argv[i]);
                ++i;
                NOT_ENOUGH;
                int w=atoi(argv[i]);
                newImage=newImgVerticalBar(s,w);
            } else if (strcmp(argv[i],"--scale")==0 ||
                strcmp(argv[i],"-s")==0 ) {
                ++i;
                NOT_ENOUGH;
                int s=atoi(argv[i]);
                newImage=imgScale(currentImage,s);
            } else if (strcmp(argv[i],"--rotate")==0 ||
                strcmp(argv[i],"-r")==0 ) {
                ++i;
                NOT_ENOUGH;
                int s=atoi(argv[i]);
                newImage=imgRotate(currentImage,s);
            } else if (strcmp(argv[i],"--lum")==0 ||
                strcmp(argv[i],"-l")==0 ) {
                newImage=imgLuminosityScale(currentImage);
            } else if (strcmp(argv[i],"--inv")==0 ||
                strcmp(argv[i],"-i")==0 ) {
                newImage=imgInvert(currentImage);
            } else if (strcmp(argv[i],"--contrast")==0 ||
                strcmp(argv[i],"-c")==0 ) {
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
            } else if (strcmp(argv[i],"--sudoku")==0) {
                ++i;
                int sz=atoi(argv[i]);
                newImage=newImgSudoku(sz);
            } else if (strcmp(argv[i],"--version")==0) {
                printf("%s version %s\n",basename(argv[0]), VERSION);
                printf("compiled with %s on %s\n",CFG_CC,__DATE__);
                printf("git hash  %s\n",CFG_GIT_FHASH);
            } else {
                imgUsage(stdout,argv[0]);
                ERROR("Unknown option: ",argv[i]);
            }
            deleteImg(currentImage);
            currentImage=newImage;
            newImage=NULL;
        }
        ++i;
    }
    deleteImg(currentImage);
    return 0;
}
