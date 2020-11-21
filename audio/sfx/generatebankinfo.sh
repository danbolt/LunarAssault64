#!/bin/bash

echo "envelope defaultenv"
echo "{"
echo "    attackTime    = 0;"
echo "    attackVolume  = 127;"
echo "    decayTime   = 500000;"
echo "    decayVolume   = 100;"
echo "    releaseTime   = 200000;"
echo "    "
echo "}"

count=10
sounds=()
for i in compressed/*.aifc; do
	nm=${i#compressed/}
	name=${nm%.aifc}
    echo ""

    sounds+=(${name})

    echo "keymap ${name}Keymap"
    echo "{"
	echo "    velocityMin = 0;"
	echo "    velocityMax = 127;"
	echo "    keyMin      = ${count};"
	echo "    keyMax      = ${count};"
	echo "    keyBase     = ${count};"
	echo "    detune      = 0;"
    echo "}"

    echo ""

    echo "sound ${name}Sound"
    echo "{"
    echo "    use (\"compressed/${name}.aifc\");"
    echo "    "

    echo "    pan    = 64;"
    echo "    volume = 127;"
    echo "    keymap = ${name}Keymap;"
    echo "    envelope = defaultenv;"
    echo "}"

    count=$((count+1))
done

echo ""

echo "instrument sfx"
echo "{"
echo "    volume = 127;"
echo "    pan    = 64;"
echo "    "

for var in "${sounds[@]}"
do
  echo "    sound = ${var}Sound;"
  # do something on $var
done

echo "}"

echo ""



echo "bank sfxbank"
echo "{"
echo "    sampleRate = 32000;"
echo "    "
echo "    instrument [0] = sfx;"
echo "}"

echo ""
