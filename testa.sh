#!/bin/bash

GRIDX=$(grep "define GRIDX" libsign.h | cut -d" " -f3)
OFFSET=$(grep "define OFFSET" libsign.h | cut -d" " -f3)
DIMEN=$(( $GRIDX * 2 + $OFFSET ))
K=2
echo $GRIDX $OFFSET $DIMEN

for i in Assinaturas/*.jpg; do
    id=$(echo $i | cut -d"/" -f2 |cut -d"_" -f1 | cut -d"a" -f2)
    ./assina $i $id > "sai/`echo $i | cut -d"/" -f2`.txt"
done
./gerabase $DIMEN sai/* > base.txt
./gerabase $DIMEN sai/a*_{02..09}.jpg.txt > base1.txt
./gerabase $DIMEN sai/a*_01.jpg.txt sai/a*_{03..09}.jpg.txt > base2.txt
./gerabase $DIMEN sai/a*_{01..02}.jpg.txt sai/a*_{04..09}.jpg.txt > base3.txt
./gerabase $DIMEN sai/a*_{01..03}.jpg.txt sai/a*_{05..09}.jpg.txt > base4.txt
./gerabase $DIMEN sai/a*_{01..04}.jpg.txt sai/a*_{06..09}.jpg.txt > base5.txt
./gerabase $DIMEN sai/a*_{01..05}.jpg.txt sai/a*_{07..09}.jpg.txt > base6.txt
./gerabase $DIMEN sai/a*_{01..06}.jpg.txt sai/a*_{08..09}.jpg.txt > base7.txt
./gerabase $DIMEN sai/a*_{01..07}.jpg.txt sai/a*_09.jpg.txt > base8.txt
./gerabase $DIMEN sai/a*_{01..08}.jpg.txt > base9.txt

./normalisera < base.txt > norm_base.txt
for i in base{1..9}.txt; do
    cat $i | ./normalisera > norm_$i
done

for i in {1..9}; do
    for n in {01..29}; do
        s=$(./classifica norm_base$i.txt $K sai/a0${n}_0${i}.jpg.txt)
        C1=$(echo $s | cut -d" " -f 1)
        C2=$(echo $s | cut -d" " -f 2)
        ./perceptrao norm_base$i.txt $C1 $C2 sai/a0${n}_0${i}.jpg.txt
    done
done > previsto.txt

./confusiona < previsto.txt > confusao.txt
