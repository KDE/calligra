#!/bin/sh
echo "// GENERATED FILE. Do not edit! Generated from kzip.cpp by $0" > kozip.cpp
cat ../../../kdelibs/kio/kio/kzip.cpp >> kozip.cpp || exit 1
echo "// GENERATED FILE. Do not edit! Generated from kzip.h by $0" > kozip.h
cat ../../../kdelibs/kio/kio/kzip.h >> kozip.h || exit 1

perl -pi -e 's/KZip/KoZip/g' kozip.cpp kozip.h
perl -pi -e 's/kzip\.h/kozip\.h/' kozip.cpp

perl -pi -e 's/closeArchive/closeArchiveHack/' kozip.cpp kozip.h
perl -pi -e 'if (/virtual bool doneWriting/) { print $_; $_ = "    virtual void close(); // HACK for misplaced closeArchive() call in KDE-3.0''s KArchive\n    virtual bool closeArchive() { return true; } // part of the same hack\n"; }' kozip.h
perl -pi -e 'if (/KoZip::doneWriting/) { print "void KoZip::close() { // HACK for misplaced closeArchive() call in KDE-3.0''s KArchive\n    if (!isOpened()) return;\n    closeArchiveHack();\n}\n\n"; }' kozip.cpp
