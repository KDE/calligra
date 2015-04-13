#!/bin/sh

path=$(pwd)
for i in `find $path -type f -a -name '*.shape'`
do
    rm $i
done

for i in `find $path -type f -a -name '*.odg'`
do
    zip -d $i Configurations2/accelerator/*
    zip -d $i Configurations2/images/*
    zip -d $i Configurations2/*
    echo "$i cleaned."
done
