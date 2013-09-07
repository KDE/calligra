cstester
--------

Example:

Prepare a list of test files (Using calligratests working copy and
inspecting changes on DOCX files):

$ find ~/kde4/src/calligratests/interoperability/kword/MSWord2007/ -name "*.docx" > files.txt

Prepare reference screen-shots:

$ while read line ; do echo "$line" > /tmp/processing.txt ; cstester --create --verbose --outdir dir1 "$line" ; done < files.txt

Prepare verification screen-shots (after changes to the source code
have been applied) and compare them against reference screen-shots:

$ while read line ; do echo "$line" > /tmp/processing.txt ; ((cstester --verify --verbose --indir dir1 --outdir dir2 "$line") || echo "$line" >> failed.txt) ; done < files.txt

Note: While scree-shots are being prepared, you can type the following
command in another terminal to see which file is processed (useful if
you run into an infinite loop):

$ tail -f /tmp/processing.txt 2>/dev/null

Note: Once verification is finished, the failed.txt file stores a list
of test files with different screen-shots.

Prepare a list of test files with different screen-shots in a format
accepted by visualimagecompare and visually inspect changes:

$ find dir1 -type f -exec md5sum {} \; | sed "s/ [^\/]*\// /" > dir1.txt
$ find dir2 -type f -exec md5sum {} \; | sed "s/ [^\/]*\// /" > dir2.txt
$ diff -u dir1.txt dir2.txt | grep "^+[0-9a-f]" | sed -e "s/[^ ]* //" -e "s/.check\/thumb_/ /" -e "s/\.png$//" | awk '{if (a==$1) {printf(" %d", $2)} else {printf("\n%s", $0)}; a=$1} END{printf("\n")}' > dirdiff.txt

$ visualimagecompare dir1 dir2 dirdiff.txt


cstester scripts
----------------

It is of advantage to create a directory structure as in the next
example to follow the explanation:

tester/
|-- checkdocs.sh
|-- verifydocs.sh
|-- documents/
|   |-- document1.odt
|   |-- ...
|   |-- document10000.xls
|-- results/
    |-- sha-of-commit1
    |-- sha-of-commit2


To create the basis for tests and to inspect regressions in
painting/loading (crashes), type the following commands:

$ cd tester/documents
$ mkdir ../results/sha-of-commit1
$ ../checkdocs.sh ../results/sha-of-commit1
$ cat tester/error-sha-of-commit1.log

The error-sha-of-commit1.log file contains a list of test files, that
either crashed or got killed by the script as they used too much CPU
or RAM while being processed.

Use the following commandsTo check for changes between different runs:

$ cd tester/documents
$ mkdir ../results/sha-of-commit2
$ ../verifydocs.sh ../results/sha-of-commit1 ../results/sha-of-commit2
$ cat tester/verify-sha-of-commit1-sha-of-commit2.log

The log file contains a list of test files and pages therein, which
changed.  The information can be used with the visualimagecompare
tool.  Use the following format:

$ visualimagecompare file-name page_x page_y


cstrunner
---------

Tool to run cstester in multiple processes.  Creates a report of files
terminated by a signal, which happens as a result of a bug or when the
allowed amount of time or CPU usage has been exceeded.  A file storing
MD5 sums of generated thumbnails is created in each output directory.

cstwrapper.sh - Limits the resources cstester is allowed to use and
makes sure the correct exit code is returned to cstrunner.

cstmd5gen.sh - Generates MD5 sums for a given thumbnail directory and
stores them into the md5.txt file.

cstmd5diff.sh - Prepares an input file for visualimagecomapre.

Note: The $PATH variable must contain the location of scripts in order
to run cstester successfully.

Synopsis:

cstrunner <documents dir> <result dir> <concurrentProcesses>

cstmd5diff.sh <documents dir> <reference dir> <verification dir>

Example:

To run over all documents in the current directory and store
thumbnails into ../result/sha1 with 4 concurrent processes:

$ cstrunner . ../result/sha1 4

Apply changes to the source code and run it again:

$ cstrunner . ../result/sha2 4

Create an input file for visualimagecompare and inspect differences:

$ cstmd5diff.sh . ../results/sha1 ../results/sha2 > md5-sha1-sha2.log
$ visualimagecompare ../results/sha1 ../results/sha2 md5-sha1-sha2.log --pdf directory_of_pdfs


visualimagecompare
------------------

Tool to visually inspect differences between reference and
verification screen-shots.

Synopsis:

visualimagecompare <reference dir> <verification dir> <diff file>

Controls:

PageUp, n	Move to the next page of the current document
PageDown, b	Move to the previous page of the current document
Up	        Move to the previous document
Down		Move to the next document
Left		Switch to the left tab
Right		Switch to the right tab
Space		Switch between tabs
p		Show pdf (only works when --pdf is given)
