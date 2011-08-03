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
