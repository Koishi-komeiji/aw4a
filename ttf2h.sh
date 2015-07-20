#!/bin/bash

if [ -z "$5" ];then
 echo "Usage: ttf2h <font name> <width> <height> <input ttf file> <output .h file>"
 exit 1
fi
ascii_dec=32
echo "Converting..."
rm -f "$3"
while :
do
if [ "$ascii_dec" -gt "126" ];then
 echo -n "Glyph size: "
 identify tmp.png |awk '{print($3)}'
 break
fi
charr0=`printf '%x\n' $ascii_dec`
charr=`echo -e '\x'$charr0`

echo "ASCII Decimal: $ascii_dec  Character: $charr"
if [ "$ascii_dec" = "64" ];then
 charr='\'$charr
 elif [ "$ascii_dec" = "92" ];then
 charr='\'$charr
fi
convert -background black -fill white -font "$4" -size "$2"'x'"$3" label:"$charr" -type truecolor png24:tmp.png
stream -map rgb -storage-type char tmp.png tmp.raw
cat tmp.raw >> font_"$1"_"$2"'x'"$3"_fontdat

ascii_dec=$((ascii_dec+1))
done

echo 'static unsigned char font_'"$1"_"$2"'x'"$3"'_width = '"$2"';' >> "$5"
echo 'static unsigned char font_'"$1"_"$2"'x'"$3"'_height = '"$3"';' >> "$5"
xxd -i font_"$1"_"$2"'x'"$3"_fontdat >> "$5"
echo 'char *'font_"$1"_"$2"'x'"$3"'[3] = { &font_'"$1"_"$2"'x'"$3"'_width, &font_'"$1"_"$2"'x'"$3"'_height, font_'"$1"_"$2"'x'"$3"'_fontdat };' >> "$5"

rm -f tmp.png tmp.raw font_"$1"_"$2"'x'"$3"_fontdat