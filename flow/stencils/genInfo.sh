#!/bin/sh

collectionInfo0='[Desktop Entry]\nName='
collectionInfo1='\nX-KDE-DirType=odg-collection'
stencilInfo='[Desktop Entry]\nName='

for j in `find ./ -type d`
do
    name=${j##*/}
    infoFile=$j'/collection.desktop'
    if [ ! -f $infoFile ]; then
        echo -e 'Generating info for collection '$name
        echo -e $collectionInfo0$name$collectionInfo1 > $infoFile
    fi
done

for i in `find ./ -type f -a -name '*.odg'`
do
    tmp=${i%.odg*}
    infoFile=$tmp.desktop
    name=${tmp##*/}
    name=${name//_/ }
    name=`echo $name | sed 's/\<./\u&/g'`
    if [ ! -f $infoFile ]; then
        echo -e 'Generating info for stencil '$name 
        echo -e $stencilInfo$name > $infoFile
    fi
done

echo -e 'Done.'
