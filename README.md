
Here how to compile the code

## gcc on ubuntu
```
./autogen.sh
./configure --prefix=$HOME/cnn CFLAGS='-Wall -g'
cd src/cnn
make
make install
./cnn ../../dataset/sudoku_photo/hard3.png 
```



## to clean everything
```
./autogen clean
```

## img

```img``` is generated in directory ```src/cnn```  and copied in directory ```$prefix/bin```. It has been made to manipulate images.

```
img usage:
    img [<input-file>] [ options ] <output-file>
Where option is one of:
    [-b|--blur] <r>:
        Blurs the image with a radius of r.
    [-c|--contrast] :
        Raises contrast level.
    --conv <filename> <percent>:
        load image in <filename> and use it as a convolution
        filter with a threshold set at <percent>.
    --cross <n> <w> <t>:
        Generates a black cross on a white background. 
        The cross size is n by n pixels in a w by w
        picture. The cross line is <t> pixels thick.
    [-f|--flatten] :
        Flattens the contrast.
    [-h|--help] :
        Displays this help message and leaves.
    [-i|--inv] :
        Inverses the image. White becomes black, black 
        becomes white.
    [-l|--lum] :
        Scales luminosity if the image is too bright or too dark.
    [-p|--print] :
        Prints on stdout numerical value of current image.
    [-s|--scale] <n>:
        Scales image by a factor n.
    --square <n> <w> <t>:
        Generates a black square on a white background. 
        The square size is n by n pixels in a w by w
        picture. The square line is <t> pixels thick.
    --sudoku <n>:
        Generates a black empty sudoku grid of n pixels large in a 
        224 by 224 picture with white background.
    [-r|--rotate] <n>:
        Rotates an image by n degrees.
    [-v|--vertical] <s> <w>:
        Generates a black vertical bar of width <w>
        in a white square of size <s>.
    --version :
        Displays the current version of img.
    --1d-dots <n> <w>:
        Generates <n> black dots on a white background, 
        in one dimension: the generated image size is <w> 
        by 1 pixels.
    --3x3-edge :
        Generates a 3x3 image to perform edge detection if 
        used as convolution filter.
    [-3d|--3d] :
        Draws an isometric-like view.
        in a 224x224 image.
    --9x9dots <n>:
        Generates 9x9 black dots on a white background. 
        The image size is n by n pixels.
Examples:
    img --sudoku 200 --rotate 5 --blur 2 out.png
        Generates an empty sudoku grid, then rotate it by 5
        degrees, then blurs the image with a radius of 2, then
        save the result in file out.png.
    
    img --3x3-edge edgefilter.png
        Creates a 3x3 filter to be used for edge detection.
        Save it under edgefilter.png.
    
    img mypic.png --conv edgefilter.png -10 out.png
        Apply the convolution filter edgefilter.png to
        mypic.png and save the result under out.png.
        -10% of threshold is applied. Put a lower percentage
        for brighter images, a higher one for a darker.
    
    img my_photo.jpg --lum my_new_photo.png
        Reads file my_photo.jpg increase luminosity to stretch from
        dark to very clear and save the result in my_new_photo.png.
```

## digits


```digits``` is generated in the ```src/cnn``` directory and copied ```$prefix/bin```. It is made to test digit recognition.

```
digits usage:
    digits [ options ]
Where option is one of:
    [-c|--create] <n>:
         creates the filters to detect digits for layer <n> 
         in directory:
             /Users/fabrice/cnn/share/digits
    [-t|--test] <n>:
         tests the filters for digits.
    [-h|--help] :
         display this help message and exits.
```

## clang on ubuntu
./autogen.sh
./configure --prefix=$HOME/cnn CFLAGS='-Wall -g' CC='clang'
cd src/cnn
make

## homebrew with gcc on mac
./autogen.sh
./configure --prefix=$HOME/cnn CFLAGS='-Wall -g -I/opt/homebrew/include/' LDFLAGS='-L/opt/homebrew/lib' LIBS='-lm -lpng' CC='/opt/homebrew/bin/gcc-12'

## native clang on mac (libpng and libjpeg installed as written below)
./autogen.sh
./configure --prefix=$HOME/cnn CFLAGS='-Wall -g' CC='clang'

## to install libpng on mac using clang (not homebrew related)
```
git clone https://github.com/glennrp/libpng
cd libpng
./configure --prefix=/usr/local CC='clang'
make
sudo make install
```

## to install libjpeg on mac using clang (not homebrew related)
```
git clone https://github.com/libjpeg-turbo/libjpeg-turbo
cd libjpeg-turbo
CC=clang cmake -G"Unix Makefiles" 
make
sudo cmake --install . --prefix /usr/local
```
