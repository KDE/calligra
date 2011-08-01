Compare whole directories
-------------------------

First create a list of files you like to test.

  find ~/kofficetests/interoperability/kword/MSWord2007/ -name "*.docx" > ~/files.txt

Then create the initial screenshots you like later to test against

  for i in `cat ~/files.txt` ; do cstester --create --verbose --outdir ~/dir1 $i ; done

Now create the screenshots that should be used to verify against the screeshots created above

  for i in `cat ~/files.txt` ; do (cstester --verify --verbose --indir ~/dir1 --outdir ~/dir2 $i) || echo "Failed $i" >> ~/failed.txt ; done
