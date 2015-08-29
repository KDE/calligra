#!/bin/sh
if [ -f CMakeLists.txt ]; then
    rm CMakeLists.txt
fi

for j in `find ./ -type d -name '[A-Z]*'`
do
    name=${j##*/}
    echo -e 'file(GLOB '$name'_Stencils "'$name'/*.odg" "'$name'/*.desktop" "'$name'/*.png")' >> CMakeLists.txt
done

echo -e '\n' >> CMakeLists.txt

for i in `find ./ -type d -name '[A-Z]*'`
do
    name=${i##*/}
    echo -e 'install( FILES ${'$name'_Stencils} DESTINATION ${DATA_INSTALL_DIR}/flow/stencils/'$name')' >> CMakeLists.txt
done

echo -e 'Done.'
