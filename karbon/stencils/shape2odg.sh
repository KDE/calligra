#!/bin/sh

path=$(pwd)
for i in `find $path -type f -a -name '*.shape'`
do
    soffice -invisible macro:///Standard.conv.SaveAsODG\($i\)
done
