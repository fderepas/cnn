#ifndef DIGITS_H
#define DIGITS_H

#include "util.h"

// external types
typedef struct layer Layer;

/** 
 * @brief data structure holding the convolution filters to 
 *        detect the digits.
 */
struct digits {
    char * layer1Prefix;
    char * layer2Prefix;
    Layer * layer1;
    Layer * layer2;
};
/**
 * @brief Short cut for struct digits
 */
typedef struct digits Digits;

Digits * newDigits(char*);
void deleteDigits(Digits*);
int digitsGetDefaultLayerPrefixSize();
void digitsGetDefaultLayerPrefix(char* layerPrefix);
void digitsGenerateImageOfFont(char*directory,int numFonts);
void digitsGenerateLayer1 (char * layerPrefix);
void digitsTestLayer1(char * layerPrefix);
int digitsMain(int argc,char**argv);

#endif
