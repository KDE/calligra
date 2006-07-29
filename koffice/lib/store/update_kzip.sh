#!/bin/sh
header="// GENERATED FILE. Do not edit! Generated from kzip.cpp by $0"
echo "$header" > kozip.cpp
cat ../../../kdelibs/kio/kio/kzip.cpp >> kozip.cpp || exit 1
echo "$header" > kozip.h
cat ../../../kdelibs/kio/kio/kzip.h >> kozip.h || exit 1
echo "$header" > kofilterdev.cpp
cat ../../../kdelibs/kio/kio/kfilterdev.cpp >> kofilterdev.cpp || exit 1
echo "$header" > kofilterdev.h
cat ../../../kdelibs/kio/kio/kfilterdev.h >> kofilterdev.h || exit 1
echo "$header" > kolimitediodevice.h
cat ../../../kdelibs/kio/kio/klimitediodevice.h >> kolimitediodevice.h || exit 1

perl -pi -e 's/KZip/KoZip/g' kozip.cpp kozip.h
perl -pi -e 's/kzip\.h/kozip\.h/' kozip.cpp
perl -pi -e 's/KFilterDev/KoFilterDev/g' kofilterdev.cpp kofilterdev.h kozip.cpp
perl -pi -e 's/kfilterdev\.h/kofilterdev\.h/' kofilterdev.cpp kozip.cpp
perl -pi -e 's/KLimitedIODevice/KoLimitedIODevice/g' kolimitediodevice.h kozip.cpp
perl -pi -e 's/klimitediodevice\.h/kolimitediodevice\.h/g' kozip.cpp

perl -pi -e 's/closeArchive/closeArchiveHack/' kozip.cpp kozip.h
perl -pi -e 'if (/.include .karchive\.h./) { print "\#define private public // need protected for m_open access for the HACK, and public for setting KArchiveFile::m_size\n$_\#undef private\n"; }' kozip.h
perl -pi -e 'if (/virtual bool doneWriting/) { print $_; $_ = "    virtual void close(); // HACK for misplaced closeArchive() call in KDE-3.0''s KArchive\n    virtual bool closeArchive() { return true; } // part of the same hack\n"; }' kozip.h
perl -pi -e 'if (/KoZip::doneWriting/) { print "void KoZip::close() { // HACK for misplaced closeArchive() call in KDE-3.0''s KArchive\n    if (!isOpened()) return;\n    closeArchiveHack();\n    device()->close();\n    m_open = false;\n}\n\n"; }' kozip.cpp
perl -pi -e 'if (/d->m_currentFile->setSize\(size\);/) { $_ = "    d->m_currentFile->m_size = size;\n"; }' kozip.cpp

