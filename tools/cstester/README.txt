Compare whole directories
-------------------------

First create a list of files you like to test.

  find ~/kofficetests/interoperability/kword/MSWord2007/ -name "*.docx" > ~/files.txt

Then create the initial screenshots you like later to test against

  for i in `cat ~/files.txt` ; do echo "$i" > ~/processing.txt ; cstester --create --verbose --outdir ~/dir1 "$i" ; done

Now create the screenshots that should be used to verify against the screeshots created above

  for i in `cat ~/files.txt` ; do echo "$i" > ~/processing.txt ; ((cstester --verify --verbose --indir ~/dir1 --outdir ~/dir2 "$i") || echo "$i" >> ~/failed.txt) ; done

While the both commands above are running you can execute following command in another terminal to see which file is currently processed (useful if you run into an infinite loop)

  tail -f ~/processing.txt 2>/dev/null

Once the verification is done in the ~/failed.txt file you will have a list of documents that changed between the both cstester runs.







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
> mkdir ../result/sha-of-commit1
> ../checkdocs.sh ../result/sha-of-commit1

Look at the result
> cat tester/error-sha-of-commit1.log

All files in there had a problems. Either they crashed or got killed by the script as they used to much CPU or RAM

To check for changes between different versions do

> cd tester/documents
> mkdir ../result/sha-of-commit2
> ./verifydocs.sh ../result/sha-of-commit1 ../result/sha-of-commit2

Look at the result

> cat tester/verify-sha-of-commit1-sha-of-commit2.log 

The file contains documents and the pages which have changed between the 2 directories
This can be used as a input file to visualimagecompare

The format is 
filename page_number page_number

It contains a line with the filename and the pages that have changed between the 2 tested versions.


