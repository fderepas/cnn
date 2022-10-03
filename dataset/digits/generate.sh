#/bin/sh
cd `dirname $0`
fonts=(Arial Helvetica)
fmax=${#fonts[@]}
fmax=$[fmax-1]
for f in `seq 0 $fmax`; do
    for i in `seq 1 9` X O; do
        convert -background white -fill black -font ${fonts[$f]} -size 28x28 -gravity center "caption:$i" $[f+1]_$i.png
    done
done
