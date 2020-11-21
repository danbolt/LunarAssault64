#!/bin/bash
mkdir -p table
for i in converted/*.aiff; do
    o=table/${i#converted/}
    ~/Projects/sdk-tools/tabledesign/tabledesign_native "$i" > "${o%.aiff}.table"
done

mkdir -p compressed
for i in table/*.table; do
    o=compressed/${i#table/}
    inp=converted/${i#table/}
    ~/Projects/sdk-tools/adpcm/vadpcm_enc_native -c "$i" "${inp%.table}.aiff"  "${o%.table}.aifc"
done

