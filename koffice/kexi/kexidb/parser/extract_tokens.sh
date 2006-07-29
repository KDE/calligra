#!/bin/sh

echo "/* WARNING! All changes made in this file will be lost! Run 'make parser' instead. */"
for t in `grep  "\"[a-zA-Z_]*\"" sqlscanner.l | sed -e "s/\(^[^\"]*\)\"\([^\"]*\)\".*$/\2/g" | sort | uniq` ; do
	if [ "$t" = "ZZZ" ] ; then break ; fi
	echo "INS(\"$t\");";
done
