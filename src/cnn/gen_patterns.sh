#!/bin/bash
set -e
cd `dirname $0`
shareDir=`cat ../../config.h | grep CFG_DATAROOTDIR | cut -d ' ' -f 3 | tr -d '"'`
for i in `seq 1 9`; do
    cp ../../dataset/digits/1_$i.png $shareDir/digits/reference_$i.png
done

convert -size 100x100 xc:white -fill black  -draw "rectangle 47,0 53,100" rect_full_size.png 
for i in `seq 0 5`; do
    a=$[i*30]
    convert rect_full_size.png -rotate $a dash-K_$i.png
done
mv dash-K_0.png  dash_0.png
convert -extract 100x100+0+0 dash-K_1.png  dash_1.png
convert -extract 100x100+0+0 dash-K_2.png  dash_2.png
mv dash-K_3.png  dash_3.png
convert -extract 100x100+0+0 dash-K_4.png  dash_4.png
convert -extract 100x100+0+0 dash-K_5.png  dash_5.png
mkdir -p $shareDir/layer1
for i in `seq 0 5`; do
    a=$[i*30]
    convert dash_$i.png -resize 5x5 $shareDir/layer1/layer_1_$i.png
done
rm rect_full_size.png
rm dash-K_*.png
rm dash_*.png


