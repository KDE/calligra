// GENERATED FILE. Do not edit! Generated from kzip.cpp by ./update_kzip.sh

/* This file is part of the KDE libraries
   Copyright (C) 2000 David Faure <faure@kde.org>
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

#include <qfile.h>
#include <kdebug.h>
#include <kmimetype.h>

//#include <kfilterdev.h>
//#include <kfilterbase.h>

#include "kozip.h"

////////////////////////////////////////////////////////////////////////
/////////////////////////// KoZip ///////////////////////////////////////
////////////////////////////////////////////////////////////////////////

class KoZip::KoZipPrivate
{
public:
    KoZipPrivate() {}
    QStringList dirList;
};

KoZip::KoZip( const QString& filename, const QString & _mimetype )
    : KArchive( 0L )
{
    kdDebug(7040) << "KoZip(filename, _mimetype) reached." << endl;
    m_filename = filename;
    d = new KoZipPrivate; //holds a QStringList
    QString mimetype( _mimetype );
    bool forced = true;
    if ( mimetype.isEmpty() )
    {
        mimetype = KMimeType::findByFileContent( filename )->name();
        kdDebug(7040) << "KoZip::KoZip mimetype=" << mimetype << endl;
	if ( mimetype != "application/x-zip")
	{
	    // Something else. Check if it's not really zip though
	    //(e.g. for KOffice docs)
    	    QFile file( filename );
    	    if (!file.exists())
	    {
		mimetype = "application/x-zip";
	    }
	    else
	    {
		if ( file.open( IO_ReadOnly ) )
    	        {
    	    	    unsigned char firstByte = file.getch();
    		    unsigned char secondByte = file.getch();
        	    unsigned char thirdByte = file.getch();
        	    unsigned char fourthByte = file.getch();
        	    if ( firstByte == 'P' && secondByte == 'K' && thirdByte == 3
	    	        && fourthByte == 4)
        	            mimetype = "application/x-zip";
		}
	    }
	    // no need to close file, the QFile destructor does it
            forced = false;
	}
    }
    kdDebug(7040) << "detected mimetype: " << mimetype << endl;
    prepareDevice( filename, mimetype, forced );
}

KoZip::KoZip( QIODevice * dev )
    : KArchive( dev )
{
    kdDebug(7040) << "KoZip::KoZip( QIODevice * dev) reached." << endl;
    d = new KoZipPrivate;
}

void KoZip::prepareDevice( const QString & filename,
                            const QString & mimetype, bool /*forced*/ )
{
    kdDebug(7040) << "preparedevice reached." << endl;
    if( "application/x-zip" == mimetype )
	setDevice( new KoZipFilter( filename ) );
    else
        kdError() << "KoZip: got mimetype: " << mimetype
		<< ", don't know what to do with it. " << endl;
}

KoZip::~KoZip()
{
    // mjarrett: Closes to prevent ~KArchive from aborting w/o device
    kdDebug(7040) << "~KoZip reached." << endl;
    if( isOpened() )
        close();
    if ( !m_filename.isEmpty() )
        delete device(); // we created it ourselves
    delete d;
}
void KoZip::setOrigFileName( const QCString & fileName )
{
    kdDebug(7040) << "setorigfilename reached." << endl;
    if ( !isOpened() || mode() != IO_WriteOnly )
    { // FIXME: what happens, when there is no device() / there is no filter attached?
        qWarning( "KoZip::setOrigFileName: File must be opened for writing first.\n");
        return;
    }
    //// TODO static_cast<KoZipFilter *>(device())->setOrigFileName( fileName );
}

bool KoZip::openArchive( int mode )
{
    kdDebug(7040) << "openarchive reached." << endl;
    if ( mode == IO_WriteOnly )
        return true;

    d->dirList.clear();
    KoZipFilter* dev = static_cast<KoZipFilter *>(device());

    KArchiveEntry* e;
    KArchiveDirectory* tdir;

    uint size=0;
    uint esize=0;
    uint csize=0;
    uint offset=0;
    uint eoffset=0;
    char buffer[0x201];
    bool b=false;
    int  n=0;
    size = dev->size();
    kdDebug(7040) << "KArchive::size()" << size << endl;
    b = (dev->at( size - 6)); //location of offset of start of central directry
			    // FIXME works only if archive contains no comment
    kdDebug(7040) << "dev->at() " << dev->at() << endl;
    n = dev->readBlock( buffer , 4);
//    kdDebug(7040) << "buf1: " << buffer << endl;
//    kdDebug(7040) << "kzip.cpp reached." << endl;
//    kdDebug(7040) << "buf1[0]: " << (uchar)buffer[0] << endl;
//    kdDebug(7040) << "buf1[1]: " << (uchar)buffer[1] << endl;
//    kdDebug(7040) << "buf1[2]: " << (uchar)buffer[2] << endl;
//    kdDebug(7040) << "buf1[3]: " << (uchar)buffer[3] << endl;

    offset = (uchar)buffer[3]*256*256*256 +(uchar)buffer[2]*256*256
	    +(uchar)buffer[1]*256 + (uchar)buffer[0];
    if (offset >= size) kdDebug(7040) << "offset >= size" << endl;
    kdDebug(7040) << "offset: " << offset << endl;

    dev->at(offset);
    bool end = false;
    do {
        n = dev->readBlock( buffer , 0x200);
	if (n < 4)
	{
	    kdDebug(7040) << "shit1" << endl;
	    // do exit (how ?)
	    return false;
	}
	if (buffer[0] == 0x50 && buffer[1] == 0x4b
	    && buffer[2] == 0x01 && buffer[3] == 0x02 )
	{  // valid central entry signature

	    if (n < 46) kdDebug(7040) << "shit2" << endl; // not long enough for valid entry

	    QString str( QString::fromLocal8Bit(buffer) );
	    QString name( buffer + 46);
	    int namelen = buffer[29] * 256 + buffer[28];
	    int extralen = buffer[31] * 256 + buffer[30];
	    int commlen = buffer[33] * 256 + buffer[32];
	    int cmethod = buffer[11] * 256 + buffer[10];

	    kdDebug(7040) << "cmethod: " << cmethod << endl;
//	    kdDebug(7040) << "buf1[1]: " << (uchar)buffer[25] << endl;
//	    kdDebug(7040) << "buf1[2]: " << (uchar)buffer[26] << endl;
//	    kdDebug(7040) << "buf1[3]: " << (uchar)buffer[27] << endl;

	    // uncompressed file size
	    esize = (uchar)buffer[27]*256*256*256 +(uchar)buffer[26]*256*256
		    +(uchar)buffer[25]*256 + (uchar)buffer[24];
	    // compressed file size
	    csize = (uchar)buffer[23]*256*256*256 +(uchar)buffer[22]*256*256
		    +(uchar)buffer[21]*256 + (uchar)buffer[20];
	    // offset of local header
	    eoffset = (uchar)buffer[45]*256*256*256 +(uchar)buffer[44]*256*256
		    +(uchar)buffer[43]*256 + (uchar)buffer[42];

	    eoffset = eoffset + 30 + extralen + namelen; //comment only in central header

	    kdDebug(7040) << "esize: " << esize << endl;
	    kdDebug(7040) << "eoffset: " << eoffset << endl;
	    kdDebug(7040) << "csize: " << csize << endl;
	    kdDebug(7040) << "buffer[29]: " << buffer[29] << endl;

	    QString nam = name.left(namelen);
	    QString nm;

            int pos = nam.findRev( '/' );
            if ( pos == -1 )
            {
	        nm = nam;
//	        kdDebug(7040) << "name: " << name << endl;
//		kdDebug(7040) << "nam: " << nam << endl;
	        kdDebug(7040) << "nm: " << nm << endl;
		// fill time field
	        e = new KArchiveFile( this, nm, 0777, 1234, rootDir()->user(), rootDir()->group(), "",
                          eoffset, esize );
	        kdDebug(7040) << "KArchiveFile created" << endl;
	        rootDir()->addEntry(e);
		// set pos in KoZipFilter
		dev->setEntry(eoffset, cmethod, csize);
	    }
            else
	    {
		tdir= findOrCreate(nam.left(pos));
        	nm = nam.mid( pos + 1 );
	        kdDebug(7040) << "name: " << name << endl;
		kdDebug(7040) << "nam: " << nam << endl;
	        kdDebug(7040) << "nm: " << nm << endl;
		// fill time field
	        e = new KArchiveFile( this, nm, 0777, 1234, rootDir()->user(), rootDir()->group(), "",
                          eoffset, esize );
		kdDebug(7040) << "KArchiveFile created" << endl;
	        tdir->addEntry(e);
		// set pos in KoZipFilter
		dev->setEntry(eoffset, cmethod, csize);

	    }
	    //calculate offset to next entry
	    kdDebug(7040) << "offset before: " << offset << endl;
	    offset = offset + 46 + commlen + extralen + namelen;
	    kdDebug(7040) << "offset after: " << offset << endl;
	    dev->at(offset);
	}
	else
	{
    	    if (buffer[0] == 0x50 && buffer[1] == 0x4b
		&& buffer[2] == 0x05 && buffer[3] == 0x06 )
		end = true;	//start of end of central dir reached.
	} // do exit
    } while ( !end);
    kdDebug(7040) << "*** done *** " << endl;
    return true;
}

bool KoZip::closeArchiveHack()
{
    if ( mode() != IO_WriteOnly )
    {
        kdDebug(7040) << "closearchive readonly reached." << endl;
        d->dirList.clear();
        return true;
    }
    //writeonly
    //write all central dir file entries

    // to be written at the end of the file...
    char buffer[ 0x201 ];
    uLong crc = crc32(0L, Z_NULL, 0);

    Q_LONG centraldiroffset = static_cast<KoZipFilter *>(device())->at();
    kdDebug(7040) << "closearchive: centraldiroffset: " << centraldiroffset << endl;
    Q_LONG atbackup = device()->at();
    KoZipFileList::iterator it;

    for (it= list.begin(); it !=list.end(); ++it )
    {	//set crc and compressed size in each local file header
        device()->at( (*it).headerstart() + 14);
//	kdDebug(7040) << "closearchive setcrcandcsize: filename: "
//	    << (*it).filename()
//	    << " encoding: "<< (*it).encoding() << endl;
        memset( buffer, 0, 0x200 );

        uLong crc = (*it).crc32();
	buffer[ 0 ] = (uchar)(crc % 256); //crc checksum
        buffer[ 1 ] = (uchar)((crc / 256) % 256);
	buffer[ 2 ] = (uchar)((crc / (256*256)) % 256);
        buffer[ 3 ] = (uchar)((crc / (256*256*256))% 256);

        int mysize1 = (*it).csize();
	buffer[ 4 ] = (uchar)(mysize1 % 256); //compressed file size
        buffer[ 5 ] = (uchar)((mysize1 / 256) % 256);
	buffer[ 6 ] = (uchar)((mysize1 / (256*256)) % 256);
        buffer[ 7 ] = (uchar)((mysize1 / (256*256*256))% 256);

	device()->writeBlock( buffer, 8);
    }
    device()->at( atbackup);
    for (it= list.begin(); it !=list.end(); ++it )
    {
	kdDebug(7040) << "closearchive: filename: " << (*it).filename()
	    << " encoding: "<< (*it).encoding() << endl;


//        char buffer[ 0x201 ];
        memset( buffer, 0, 0x200 );

        buffer[ 0 ] = 'P'; //central file header signature
        buffer[ 1 ] = 'K';
        buffer[ 2 ] = 1;
        buffer[ 3 ] = 2;

        buffer[ 4 ] = 0x14; // version made by
        buffer[ 5 ] = 0;

        buffer[ 6 ] = 0x14; // version needed to extract
	buffer[ 7 ] = 0;

	if ( (*it).encoding() == 8 )
	    buffer[ 8 ] = 8; // general purpose bit flag,deflated
	else
    	    buffer[ 8 ] = 0; // general purpose bit flag,stored

	buffer[ 9 ] = 0;

	if ( (*it).encoding() == 8 )
            buffer[ 10 ] = 8; // compression method, deflated
	else
            buffer[ 10 ] = 0; // compression method, stored
        buffer[ 11 ] = 0;

        buffer[ 12 ] = 0; //dummy last mod file time
	buffer[ 13 ] = 0;
        buffer[ 14 ] = 0; //dummy last mod file date
	buffer[ 15 ] = 0;

        uLong crc = (*it).crc32();
	buffer[ 16 ] = (uchar)(crc % 256); //crc checksum
        buffer[ 17 ] = (uchar)((crc / 256) % 256);
	buffer[ 18 ] = (uchar)((crc / (256*256)) % 256);
        buffer[ 19 ] = (uchar)((crc / (256*256*256))% 256);

        int mysize1 = (*it).csize();
	buffer[ 20 ] = (uchar)(mysize1 % 256); //compressed file size
        buffer[ 21 ] = (uchar)((mysize1 / 256) % 256);
	buffer[ 22 ] = (uchar)((mysize1 / (256*256)) % 256);
        buffer[ 23 ] = (uchar)((mysize1 / (256*256*256))% 256);

        int mysize = (*it).usize();
	buffer[ 24 ] = (uchar)(mysize % 256); //uncompressed file size
        buffer[ 25 ] = (uchar)((mysize / 256) % 256);
	buffer[ 26 ] = (uchar)((mysize / (256*256)) % 256);
        buffer[ 27 ] = (uchar)((mysize / (256*256*256))% 256);

	buffer[ 28 ] = (uchar)(*it).filename().length(); //filename length
        buffer[ 29 ] = (uchar)(((*it).filename().length() / 256) % 256);

	buffer[ 30 ] = 0; // extra field length
        buffer[ 31 ] = 0;

	buffer[ 32 ] = 0; // file comment length
        buffer[ 33 ] = 0;

	buffer[ 34 ] = 0; // disk number start
        buffer[ 35 ] = 0;

	buffer[ 36 ] = 0; // internal file attributes
        buffer[ 37 ] = 0;

        buffer[ 38 ] = 0; // external file attributes
	buffer[ 39 ] = 0;
        buffer[ 40 ] = 0;
	buffer[ 41 ] = 0;

        int myhst = (*it).headerstart();
	buffer[ 42 ] = (uchar)(myhst % 256); //relative offset of local header
        buffer[ 43 ] = (uchar)((myhst / 256) % 256);
	buffer[ 44 ] = (uchar)((myhst / (256*256)) % 256 );
        buffer[ 45 ] = (uchar)((myhst / (256*256*256)) % 256);

        // file name
	strncpy( buffer + 46, QFile::encodeName((*it).filename()),
		    (*it).filename().length() );
	int i = (*it).filename().length() + 46;
	    kdDebug(7040) << "closearchive length to write: " << i << endl;
	crc = crc32(crc, (Bytef *)buffer, i);
	device()->writeBlock( buffer, i);
    }
    Q_LONG centraldirendoffset = static_cast<KoZipFilter *>
			(device())->at();
    kdDebug(7040) << "closearchive: centraldirendoffset: "
		<< centraldirendoffset << endl;
    kdDebug(7040) << "closearchive: device()->at(): "
		<< device()->at() << endl;


    //write end of central dir record.
    memset( buffer, 0, 0x200 );
    buffer[ 0 ] = 'P'; //end of central dir signature
    buffer[ 1 ] = 'K';
    buffer[ 2 ] = 5;
    buffer[ 3 ] = 6;

    buffer[ 4 ] = 0; // number of this disk
    buffer[ 5 ] = 0;

    buffer[ 6 ] = 0; // number of disk with start of central dir
    buffer[ 7 ] = 0;

    int count = list.count();
    kdDebug(7040) << "number of files (count): " << count << endl;


    buffer[ 8 ] = (uchar)(count % 256); // total number of entries in central dir of
    buffer[ 9 ] = (uchar)((count / 256) % 256); // this disk

    buffer[ 10 ] = buffer[ 8 ]; // total number of entries in the central dir
    buffer[ 11 ] = buffer[ 9 ];

    int cdsize = centraldirendoffset - centraldiroffset;
    buffer[ 12 ] = (uchar)(cdsize % 256); //size of the central dir
    buffer[ 13 ] = (uchar)((cdsize / 256) % 256);
    buffer[ 14 ] = (uchar)((cdsize / (256*256)) % 256);
    buffer[ 15 ] = (uchar)((cdsize / (256*256*256))% 256);

    kdDebug(7040) << "end : centraldiroffset: " << centraldiroffset << endl;
    kdDebug(7040) << "end : centraldirsize: " << cdsize << endl;

    buffer[ 16 ] = (uchar)(centraldiroffset % 256) ; //central dir offset
    buffer[ 17 ] = (uchar)((centraldiroffset / 256) % 256);
    buffer[ 18 ] = (uchar)((centraldiroffset / (256*256)) % 256);
    buffer[ 19 ] = (uchar)((centraldiroffset / (256*256*256)) % 256);

    buffer[ 20 ] = 0; //zipfile comment length
    buffer[ 21 ] = 0;

    device()->writeBlock( buffer, 22);

    kdDebug(7040) << "kzip.cpp reached." << endl;
    d->dirList.clear();
    return true;
}

bool KoZip::prepareWriting( const QString& name, const QString& user,
						const QString& group, uint size )
{
    kdDebug(7040) << "prepareWriting reached." << endl;
    if ( !isOpened() )
    {
        qWarning( "KoZip::writeFile: You must open the zip file before writing to it\n");
        return false;
    }

    if ( mode() != IO_WriteOnly )
    {
        qWarning( "KoZip::writeFile: You must open the zip file for writing\n");
        return false;
    }
    // construct a KoZipFileEntry and add it to list
    KoZipFileEntry * e = new KoZipFileEntry();
    e->setFilename(name);
    e->setUSize(size);
    if ( name == "meta.xml") // openoffice meta.xml is not compressed
			    // to allow indexing of the stored file,
			    // so we will do it too.
	e->setEncoding( 0 ); //stored
    else
	e->setEncoding( 8 ); //deflated
    e->setHeaderStart( static_cast<KoZipFilter *>(device())->at() );
    e->setStart( static_cast<KoZipFilter *>(device())->at()
		    + 30 + name.length() );
        kdDebug(7040) << "wrote file start: " << e->start()
		<< " name: " << name << endl;
    // write out zip header
    list.append ( *e );

     static_cast<KoZipFilter *>(device())->setEntry( e->start(),
    					e->encoding(), 123);

    actualFile = list.end();
    --actualFile;
    if (actualFile == list.end() )
        kdDebug(7040) << "actualFile: something wrong..." << endl;


    char buffer[ 0x201 ];
    memset( buffer, 0, 0x200 );

    buffer[ 0 ] = 'P'; //local file header signature
    buffer[ 1 ] = 'K';
    buffer[ 2 ] = 3;
    buffer[ 3 ] = 4;

    buffer[ 4 ] = 0x14; // version needed to extract
    buffer[ 5 ] = 0;

    buffer[ 6 ] = 0; // general purpose bit flag
    buffer[ 7 ] = 0;

    buffer[ 8 ] = (uchar)(e->encoding() % 256); // compression method
    buffer[ 9 ] = (uchar)(e->encoding() / 256);

    buffer[ 10 ] = 0; //dummy last mod file time
    buffer[ 11 ] = 0;
    buffer[ 12 ] = 0; //dummy last mod file date
    buffer[ 13 ] = 0;

    buffer[ 14 ] = 'C'; //dummy crc
    buffer[ 15 ] = 'R';
    buffer[ 16 ] = 'C';
    buffer[ 17 ] = 'q';

    buffer[ 18 ] = 'C'; //compressed file size
    buffer[ 19 ] = 'S';
    buffer[ 20 ] = 'I';
    buffer[ 21 ] = 'Z';

    int mysize = size;
    buffer[ 22 ] = (uchar)(mysize % 256) ; //uncompressed file size
    buffer[ 23 ] = (uchar)((mysize / 256) % 256);
    buffer[ 24 ] = (uchar)((mysize / (256*256)) % 256);
    buffer[ 25 ] = (uchar)((mysize / (256*256*256)) % 256);

    buffer[ 26 ] = (uchar)name.length(); //filename length
    buffer[ 27 ] = (uchar)((name.length() / 256) % 256);

    buffer[ 28 ] = 0; // extra field length
    buffer[ 29 ] = 0;

    // file name
    strncpy( buffer + 30, QFile::encodeName(name), name.length() );

    // Write header
    bool b= (device()->writeBlock( buffer, name.length() + 30 )
	    == name.length() + 30);
    static_cast<KoZipFilter *>(device())->setcrc( 0L );
    return b;
}

void KoZip::close() { // HACK for misplaced closeArchive() call in KDE-3.0s KArchive
    if (!isOpened()) return;
    closeArchiveHack();
}

bool KoZip::doneWriting( uint size )
{
    kdDebug(7040) << "donewriting reached." << endl;
    kdDebug(7040) << "filename: " << (*actualFile).filename() << endl;
    kdDebug(7040) << "getpos (at): "
	<< device()->at() << endl;
    kdDebug(7040) << "headerstart: " << (*actualFile).headerstart() << endl;
//    kdDebug(7040) << ": " << (*actualFile).csize() << endl;
//    kdDebug(7040) << "headerstart: " << (*actualFile).headerstart() << endl;
    int csize = static_cast<KoZipFilter *>(device())->at() -
        (*actualFile).headerstart() - 30 -
	(*actualFile).filename().length();
	(*actualFile).setCSize(csize);
    kdDebug(7040) << "filename: " << (*actualFile).filename() << endl;
    kdDebug(7040) << "usize: " << (*actualFile).usize() << endl;
    kdDebug(7040) << "csize: " << (*actualFile).csize() << endl;
    kdDebug(7040) << "headerstart: " << (*actualFile).headerstart() << endl;

    kdDebug(7040) << "crc: " <<
	    static_cast<KoZipFilter *>(device())->getcrc() << endl;
    (*actualFile).setCRC32(static_cast<KoZipFilter *>(device())->getcrc());

    return true;
}

void KoZip::virtual_hook( int id, void* data )
{ KArchive::virtual_hook( id, data ); }



KoZipFilter::KoZipFilter(const QString& filename)
{
    dev = new QFile ( filename );
}

bool KoZipFilter::open(int _mode)
{
    return dev->open(_mode);
}
void KoZipFilter::close()
{
    dev->close();
}
void KoZipFilter::flush()
{
    dev->flush();
}
Q_ULONG KoZipFilter::size() const
{
    return dev->size();
}

Q_LONG KoZipFilter::readBlock(char * c, long unsigned int i)
{
    bool doinflate=false;
    int cmethod=0;
    Q_LONG csize=0;
    int pos=dev->at();
	kdDebug(7040) << "readblock. pos: " << pos <<" size: " << i << endl;
    KoZipFileList::iterator it;
    for (it= list.begin(); it !=list.end(); ++it )
    {
	kdDebug(7040) << "kzipfilter123: offset: " << (*it).start()
	    << " encoding: "<< (*it).encoding() << endl;
	if (pos == (*it).start())
	{
	    cmethod=(*it).encoding();
	    csize=(*it).csize();
	    kdDebug(7040) << "cmethod: " << cmethod << endl;
	    kdDebug(7040) << "csize: " << csize << endl;

	}
    }
    if (cmethod == 8) //zip deflated
    {
        // Inflate contents!
        QByteArray * dataBuffer = new QByteArray( i );
	dev->readBlock( reinterpret_cast<char *>(dataBuffer->data() ), i);
        z_stream d_stream;      /* decompression stream */

        d_stream.zalloc = ( alloc_func ) 0;
        d_stream.zfree = ( free_func ) 0;
        d_stream.opaque = ( voidpf ) 0;

        d_stream.next_in = ( unsigned char * ) dataBuffer->data();
        d_stream.avail_in = csize;

        inflateInit2( &d_stream, -MAX_WBITS );

        int err;
        for ( ;; ) {
            d_stream.next_out =
                reinterpret_cast <
                unsigned char *>(c);
            d_stream.avail_out = i ;
            err = inflate( &d_stream, Z_NO_FLUSH );
            if ( err == Z_STREAM_END )
                break;
            if ( err < 0 ) { // some error
                kdWarning(7040) << "readBlock: zlib inflate returned error " << err << endl;
                break;
            }
        }

        delete dataBuffer;
	return i;
    }
    else if (cmethod == 0)
        return dev->readBlock(c, i);
    else
    {
	kdError() << "This zip file contains files compressed with method "
	    << cmethod <<", this method is currently not supported by kio_zip,"
	    <<" please use a command-line tool to handle this file." << endl;
	return 0;
    }
}
Q_LONG KoZipFilter::writeBlock(const char * c, long unsigned int i)
{
//    kdDebug(7040) << "filter:writeblock: m_pos before: " << m_pos << endl;

    bool dodeflate=false;
    int cmethod=0;
    Q_LONG csize=0;
    int pos=dev->at();
    kdDebug(7040) << "writeblock. dev->at() : " << pos <<" size: " << i << endl;
    KoZipFileList::iterator it;
    KoZipFileList::iterator it2;
    // crc to be calculated over uncompressed stuff...
    // and they didn't mention it in their docs...
    crc=crc32(crc, (const Bytef *) c , i);
    for (it= list.begin(); it !=list.end(); ++it )
    {
//	kdDebug(7040) << "kzipfilter writeblock : offset: " << (*it).start()
//	    << " encoding: "<< (*it).encoding() << endl;
	if (pos == (*it).start())
	{
	    cmethod=(*it).encoding();
	    kdDebug(7040) << "cmethod found: " << cmethod << endl;
	    it2=it;
	}
    }
    if (cmethod == 8) //zip deflate
    {
	    kdDebug(7040) << "compression part reached... " << endl;
	    kdDebug(7040) << "crc : " << QString::number( crc , 16) << endl;
        // Deflate contents!
        QByteArray * dataBuffer = new QByteArray( i );
        z_stream d_stream;      /* decompression stream */
//	    kdDebug(7040) << "compression part 1 " << endl;

        d_stream.zalloc = ( alloc_func ) 0;
        d_stream.zfree = ( free_func ) 0;
        d_stream.opaque = ( voidpf ) 0;
//	    kdDebug(7040) << "compression part 2 " << endl;

        d_stream.next_in = (unsigned char *)c;
        d_stream.avail_in = i;
//	    kdDebug(7040) << "compression part 3 " << endl;
        int result = deflateInit2(&d_stream, Z_DEFAULT_COMPRESSION,
		    Z_DEFLATED, -MAX_WBITS, 8, Z_DEFAULT_STRATEGY); // same here
//	    kdDebug(7040) << "compression part 4 " << endl;

        int err;
        d_stream.next_out = (unsigned char *)dataBuffer->data();
//	    kdDebug(7040) << "compression part 5 " << endl;

        d_stream.avail_out = i ;
        err = deflate( &d_stream, Z_FINISH );
        if ( err == Z_STREAM_END )
            kdDebug(7040) << "Z_STREAM_END " << endl;
        else
            kdWarning(7040) << "writeBlock: zlib deflate returned error " << err << endl;

        kdDebug(7040) << "compression part 6: total_out: " <<
            d_stream.total_out << endl;
	(*it2).setCSize( d_stream.total_out );
        Q_LONG l;
        kdDebug(7040) << "crc after : " << QString::number( crc , 16) << endl;
        l=dev->writeBlock((const char *)dataBuffer->data(),
				d_stream.total_out);
        kdDebug(7040) << "compressed written: " << l << endl;
	delete dataBuffer;
	return i; // faked written bytes
    }
    else if (cmethod == 0)
    {
        Q_LONG l;
//	    kdDebug(7040) << "crc uncompressed : " << QString::number( crc , 16) << endl;
        l=dev->writeBlock(c, i);
//        kdDebug(7040) << "uncompressed written: " << l << endl;
//	    kdDebug(7040) << "crc uncompressed after: " << QString::number( crc , 16) << endl;
        return l;

    }
}

int KoZipFilter::getch()
{
    return dev->getch();
}

int KoZipFilter::putch(int i)
{
    return dev->putch(i);
}

int KoZipFilter::ungetch(int i)
{
    return dev->ungetch(i);
}

Q_ULONG KoZipFilter::at() const
{
    return dev->at();
}

bool KoZipFilter::at ( Offset pos )
{
    return dev->at( pos);
}

bool KoZipFilter::atEnd () const
{
    return dev->atEnd();
}

bool KoZipFilter::setEntry(Q_LONG start, int encoding, Q_LONG csize)
{
    kdDebug(7040) << "setentry called." << endl;
    kdDebug(7040) << "setentry: start: " << start << " encoding: " << encoding <<
	" csize: " << csize << endl;
    list.append( KoZipFileEntry(start, encoding, csize));
    return true;
}

