Compare directories
-------------------

1. To prepare for example a list of *.docx test files from your calligratests
working copy, type the following command:

$ find ~/kde4/src/calligratests/interoperability/kword/MSWord2007/ -name "*.docx" > files.txt

2. Prepare initial screen-shots to test against into an existing directory:

$ while read line ; do echo "$line" > /tmp/processing.txt ; cstester --create --verbose --outdir dir1 "$line" ; done < files.txt

3. Prepare verification screen-shots into an existing directory (after your
changes to the calligra source have been applied) and compare them against
initial scree-shots:

$ while read line ; do echo "$line" > /tmp/processing.txt ; ((cstester --verify --verbose --indir dir1 --outdir dir2 "$line") || echo "$line" >> failed.txt) ; done < files.txt

4. To compare initial and verification screen-shots, first a list of test files
with different screen-shots has to be determined.  Then the visualimagecompare
tool helps to inspect changes.  Type the following lines:

$ find dir1 -type f -exec md5sum {} \; | sed "s/ [^\/]*\// /" > dir1.txt
$ find dir2 -type f -exec md5sum {} \; | sed "s/ [^\/]*\// /" > dir2.txt
$ diff -u dir1.txt dir2.txt | grep "^+[0-9a-f]" | sed -e "s/[^ ]* //" -e "s/.check\/thumb_/ /" -e "s/\.png$//" | awk '{if (a==$1) {printf(" %d", $2)} else {printf("\n%s", $0)}; a=$1} END{printf("\n")}' > dirdiff.txt
$ visualimagecompare dir1 dir2 dirdiff.txt

Note: While scree-shots are being prepared, you can type the following command
in another terminal to see which file is processed (useful if you run into an
infinite loop):

$ tail -f /tmp/processing.txt 2>/dev/null

Note: Once the verification in step 3 is finished, the failed.txt file contains
a list of test files with different screen-shots between step 2 and step 3.


cstester scripts
----------------

It is of advantage to create a directory structure as in the next example to
follow the explanation:

tester/
|-- checkdocs.sh
|-- verifydocs.sh
|-- documents/
|   |-- document1.odt
|   |-- ...
|   |-- document10000.xsl
|-- results/
    |-- sha-of-commit1
    |-- sha-of-commit2


To create the basis for tests and to inspect regressions in painting/loading
(crashes), type the following commands:

$ cd tester/documents
$ mkdir ../results/sha-of-commit1
$ ../checkdocs.sh ../results/sha-of-commit1
$ cat tester/error-sha-of-commit1.log

The error-sha-of-commit1.log file contains a list of test files, that either
crashed or got killed by the script as they used too much CPU or RAM while
being processed.

To check for changes between different runs, type the following commands:

$ cd tester/documents
$ mkdir ../results/sha-of-commit2
$ ../verifydocs.sh ../results/sha-of-commit1 ../results/sha-of-commit2
$ cat tester/verify-sha-of-commit1-sha-of-commit2.log

The log file contains a list of test files and pages therein, which changed.
The information can be used with the visualimagecompare tool.  Use the
following format:

$ visualimagecompare file-name page_x page_y


cstrunner
---------

cstrunner is a tool to run cstester in multiple processes.  It creates a report
of files terminated by a signal.  This can happen as a result of a bug in
calligra or when the allowed amount of time or CPU usage has been exceeded.
The md5.txt file providing the ms5sums of generated thumbnails is created in
each output directory.  The following scripts are used by cstrunner:

cstwrapper.sh - Limits the resources cstester is allowed to use and makes sure
the correct exit code is returned to cstrunner.

cstmd5gen.sh - Generates md5sums for a given thumbnail directory and stores
them into the md5.txt file.

Note: Make sure the mentioned scripts are in your $PATH variable when running
cstrunner.


Use the following format:

$ cstrunner docDir resultDir concurrentProcesses

e.g.

$ cstrunner . ../result/sha1 4

To run over all documents in the current directory and put the thumbnails in to
../result/sha1 with 4 concurrent processes.

Do your changes and run it again:

$ cstrunner . ../result/sha2 4


To create the input file for visualimagecomapre use the cstmd5diff.sh script:

$ cstmd5diff.sh <documents dir> <previous result dir> <current result dir>

e.g.

$ cstmd5diff.sh . ../results/sha1 ../results/sha2 > md5-sha1-sha2.log
$ visualimagecompare ../results/sha1 ../results/sha2 md5-sha1-sha2.log --pdf directory_of_pdfs


visualimagecompare
------------------

PageUp, n	Move to the next page of the current document
PageDown, b	Move to the previous page of the current document
Up	        Move to the previous document
Down		Move to the next document
Left		Switch to the left tab
Right		Switch to the right tab
Space		Switch between tabs
p           Show pdf (only works when --pdf is given)
