// GENERATED FILE. Do not edit! Generated from kzip.h by ./update_kzip.sh
/* This file is part of the KDE libraries
   Copyright (C) 2002 Holger Schroeder <holger-kde@holgis.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#ifndef __kzip_h
#define __kzip_h

#include <sys/stat.h>
#include <sys/types.h>

#include <qdatetime.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qdict.h>
#include <qvaluelist.h>
#include <karchive.h>
#include <zlib.h>

class KoZipFileEntry;

// Directly "ported" from the tar stuff to support zip file format.

/**
 * @short A class for reading/writing zip archives.
 * @author Holger Schroeder <holger-kde@holgis.net>
 */
class KoZipFilter : public QIODevice
{
/*
    this class gives transparent acces to a compressed zip file,
    dependant on where you start reading in the file,
    so when extracting the directory structure, it gives back the
    raw data, but when you read an included file starting on its
    well-defined startpoint, you will get it extracted.
    (a bit of a hack, but it should work ;) )
*/

public:
    KoZipFilter(const QString& filename);

    bool open(int);
    void close();
    void flush();
    Offset size() const;
    Q_LONG readBlock(char *, long unsigned int);
    Q_LONG writeBlock(const char *, long unsigned int);
    int getch();
    int putch(int);
    int ungetch(int);
    Offset at () const;
//    Q_LONG getpos () const;

    bool at ( Offset pos );
    bool atEnd () const;

    bool setEntry(Q_LONG start, int encoding,Q_LONG csize);
    uLong getcrc() {return crc; }
    void setcrc(uLong _crc) { crc= _crc; }

private:
    typedef QValueList<KoZipFileEntry> KoZipFileList;
    QIODevice * dev;
    KoZipFileList list;
//    Q_LONG m_pos;
    uLong crc;
};

class KoZipFileEntry
{
public:
    KoZipFileEntry() : st(-1)
    {}
    KoZipFileEntry(Q_LONG start, int encoding, Q_LONG csize) :
		st(start) , enc(encoding) , cs(csize)
    {}
    Q_LONG start() const {return st; }
    int encoding() const {return enc; }
    Q_LONG csize() const {return cs; }
    uLong crc32() const {return crc; }

    QString filename() const {return fn; }
    Q_LONG usize() const {return us; }
    Q_LONG headerstart() const {return hst; }

    void setStart(Q_LONG start) { st=start; }
    void setEncoding(int encoding) { enc=encoding; }
    void setCSize(Q_LONG csize) { cs=csize; }
    void setCRC32(uLong crc32) {crc=crc32; }

    void setFilename(QString filename) { fn=filename; }
    void setUSize(Q_LONG usize) { us=usize; }
    void setHeaderStart(Q_LONG headerstart) { hst=headerstart; }

private:
    Q_LONG st;
    int enc;
    Q_LONG cs;
    uLong crc;

    QString fn;
    Q_LONG us;
    Q_LONG hst;

};



class KoZip : public KArchive
{
public:
    /**
     * Creates an instance that operates on the given filename.
     * using the compression filter associated to given mimetype.
     *
     * @param filename is a local path (e.g. "/home/weis/myfile.zip")
     * @param mimetype Only "application/x-zip" is supported by KoZip.
     * If the mimetype is ommitted, it will be determined from the filename.
     * If something else than application/x-zip is found, the resulting KoZip will be invalid.
     * In summary, if you know for sure that the file is a zip file, you will
     * save time by passing application/x-zip as parameter. Otherwise, let KoZip
     * check the file.
     */
    KoZip( const QString& filename, const QString & mimetype = QString::null );

    /**
     * Creates an instance that operates on the given device.
     * The device can be compressed (KFilterDev) or not (QFile, etc.).
     * WARNING: don't assume that giving a QFile here will decompress the file,
     * in case it's compressed!
     */
    KoZip( QIODevice * dev );

    /**
     * If the zip file is still opened, then it will be
     * closed automatically by the destructor.
     */
    virtual ~KoZip();

    /**
     * The name of the zip file, as passed to the constructor
     * Null if you used the QIODevice constructor.
     */
    QString fileName() { return m_filename; }

    /**
     * Special function for setting the "original file name" in the gzip header,
     * when writing a tar.gz file. It appears when using in the "file" command,
     * for instance. Should only be called if the underlying device is a KFilterDev!
     *
     * FIXME: useful for zip ?
     */
    void setOrigFileName( const QCString & fileName );

    /**
     * @internal Not needed for zip
     */
    virtual bool writeDir( const QString&, const QString&, const QString& ) { return true; }

    virtual bool prepareWriting( const QString& name, const QString& user, const QString& group, uint size );
    virtual bool doneWriting( uint size );
    virtual void close(); // HACK for misplaced closeArchive() call in KDE-3.0s KArchive
    virtual bool closeArchive() { return true; } // part of the same hack

protected:
    /**
     * Opens the archive for reading.
     * Parses the directory listing of the archive
     * and creates the KArchiveDirectory/KArchiveFile entries.
     *
     */
    virtual bool openArchive( int mode );
    virtual bool closeArchiveHack();

private:
    /**
     * @internal
     */
    void prepareDevice( const QString & filename, const QString & mimetype, bool forced = false );

    /**
     * @internal
     * Fills @p buffer for writing a file as required by the tar format
     * Has to be called LAST, since it does the checksum
     * (normally, only the name has to be filled in before)
     * @param mode is expected to be 6 chars long, [uname and gname 31].
     */
    void fillBuffer( char * buffer, const char * mode, int size, char typeflag, const char * uname, const char * gname );

    QString m_filename;
protected:
    virtual void virtual_hook( int id, void* data );
private:
    class KoZipPrivate;
    KoZipPrivate * d;
    typedef QValueList<KoZipFileEntry> KoZipFileList;
    KoZipFileList list;
    KoZipFileList::iterator actualFile;
};

#endif
