#!/bin/bash
mkdir -p converted
for i in original/*.wav; do
    o=converted/${i#original/}
    sox "$i" -b 16 -r 32000 -c 1 "${o%.wav}.aiff"
done

