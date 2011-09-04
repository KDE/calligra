Compare whole directories
-------------------------

First create a list of files you like to test.

  find ~/kofficetests/interoperability/kword/MSWord2007/ -name "*.docx" > ~/files.txt

Then create the initial screenshots you like later to test against

  while read line ; do echo "$line" > ~/processing.txt ; cstester --create --verbose --outdir ~/dir1 "$line" ; done < ../files.txt

Now create the screenshots that should be used to verify against the screeshots created above

  while read line ; do echo "$line" > ~/processing.txt ; ((cstester --verify --verbose --indir ~/dir1 --outdir ~/dir2 "$line") || echo "$line" >> ~/failed.txt) ; done < ../files.txt

While the both commands above are running you can execute following command in another terminal to see which file is currently processed (useful if you run into an infinite loop)

  tail -f ~/processing.txt 2>/dev/null

Once the verification is done in the ~/failed.txt file you will have a list of documents that changed between the both cstester runs.

If you like to compare the screenshots of 2 directories to and determinate which of the screenshots changed you can use

  find ~/dir1 -type f -exec md5sum {} \; | sed "s/ [^\/]*\// /" >dir1.txt
  find ~/dir2 -type f -exec md5sum {} \; | sed "s/ [^\/]*\// /" >dir2.txt
  diff -u dir1.txt dir2.txt | grep "^+[0-9a-f]" | sed -e "s/[^ ]* //" -e "s/.check\/thumb_/ /" -e "s/\.png$//" | awk '{if (a==$1) {printf(" %d", $2)} else {printf "\n"$0}; a=$1} END{printf("\n")}' > dirdiff.txt

Then you can use the visualimagecompare-tool to control the changes using

  visualimagecompare ~/dir1 ~/dir2 dirdiff.txt



Use the scripts provided with cstester
--------------------------------------

To use the scripts it is of advantage to create a directory structure like the following:
tester/
  checkdocs.sh
  verifydocs.sh
  documents/
    document1.odt
    ...
    document10000.xsl
  results/
    sha-of-commit1
    sha-of-commit2

This explanation of the tools expects the directories to be there.

How to create the basis for the tests and see if regressions in painting/loading (crashes)
> cd tester/documents
> mkdir ../results/sha-of-commit1
> ../checkdocs.sh ../results/sha-of-commit1

Look at the result
> cat tester/error-sha-of-commit1.log

All files in there had a problems. Either they crashed or got killed by the script as they used to much CPU or RAM

To check for changes between different versions do

> cd tester/documents
> mkdir ../results/sha-of-commit2
> ../verifydocs.sh ../results/sha-of-commit1 ../results/sha-of-commit2

Look at the result

> cat tester/verify-sha-of-commit1-sha-of-commit2.log 

The file contains documents and the pages which have changed between the 2 directories
This can be used as a input file to visualimagecompare

The format is 
filename page_number page_number

It contains a line with the filename and the pages that have changed between the 2 tested versions.


cstrunner
---------

cstrunner is a tool to run cstester on multiple processes at the same time. It will create a report of the files
that where terminated by a signal. This can either happen if there is a bug in the calligra or the allowed amount of
time or cpu has been exceeded. It also creates a file md5.txt in each output directory which contains the ms5sums of
the generated thumbnails.

cstrunner uses the scripts cstwrapper and cstmd5gen

- cstwrapper 
limits the resources cstester is allowed to use and makes sure the correct exit code is returned to cstrunner

- cstmd5gen
generates md5sums for a given thumbnail directory and stores them in the file md5.txt in the directory.
