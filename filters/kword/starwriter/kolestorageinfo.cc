/* This file is part of the KOffice project
   Copyright (C) 2002 Ariya Hidayat <ariya@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License version 2 as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <stdio.h>

#include <qdatastream.h>
#include <qfile.h>
#include <qstring.h>
#include <qstringlist.h>

#include <kolestorage.h>
#include <kolestorageinfo.h>

using namespace KOLE;

// get unsigned 16-bit integer at given offset
static inline Q_UINT32 readU16( QByteArray array, unsigned p )
{
  Q_UINT8 *ptr = (Q_UINT8*) array.data();
  return (Q_UINT16)(ptr[p])+(ptr[p+1]<<8);
}

// get unsigned 32-bit integer at given offset
static inline Q_UINT32 readU32( QByteArray array, unsigned p )
{
  Q_UINT8 *ptr = (Q_UINT8*) array.data();
  return (Q_UINT32)(ptr[p])+(ptr[p+1]<<8)+(ptr[p+2]<<16)+(ptr[p+3]<<24);
}
// write unsigned 16-bit integer at given offset
static inline void writeU32( QByteArray array, unsigned p, Q_UINT16 data )
{
  Q_UINT8 *ptr = (Q_UINT8*) array.data();
  ptr[p] = data & 0xff;
  ptr[p+1] = (data >> 8) & 0xff;
}

// write unsigned 32-bit integer at given offset
static inline void writeU32( QByteArray array, unsigned p, Q_UINT32 data )
{
  Q_UINT8 *ptr = (Q_UINT8*) array.data();
  ptr[p] = data & 0xff;
  ptr[p+1] = (data >> 8) & 0xff;
  ptr[p+2] = (data >> 16) & 0xff;
  ptr[p+3] = (data >> 24) & 0xff;
}

Entry::Entry()
{
  name = "Unnamed";
  size = 0;
  children.setAutoDelete( true );
}

Entry::~Entry()
{
  children.clear();
}

AllocTable::AllocTable()
{
  dirty = false;
}

// auto-enlarge ?
int AllocTable::operator[](int index )
{
  int result;
  result = m_data[index];
  return result;
}

void AllocTable::parse( QByteArray buffer )
{
  Q_UINT8 *data = (Q_UINT8*) buffer.data();
  unsigned count = buffer.size()/4;
  m_data.resize( count );
  for( unsigned i=0, p=0; i<count; i++, p+=4 )
    m_data[i] = (data[p])+(data[p+1]<<8)+(data[p+2]<<16)+(data[p+3]<<24);
}

// need to avoid endless loop ?
QValueVector<int> AllocTable::follow( unsigned start )
{
  QValueVector<int> chain;
  QValueList<int> list;

  // follow the linked-list
  for( int p = (int)start; (p > -1) && (p < (int)m_data.size()); p = m_data[p] )
    list.append( p );

  // convert list to vector for faster access
  chain.resize( list.count() );
  for( unsigned i=0; i<list.count(); i++ )
    chain[i] = list[i];

  return chain;
}

// constructor
StorageInfo::StorageInfo( Storage* _doc, const QString& fileName, int m ):
  doc (_doc ),
  mode ( m )
{
  // initialization
  result = Storage::Ok;
  threshold = 4096;

  // prepare input stream
  file.setName( fileName );
  if( !file.open( mode ) )
  {
    qWarning("cannot access file %s", fileName.latin1() );
    result = Storage::OpenFailed;
    return;
  }
  stream.setDevice( &file );

  // OLE Storage is always little-endian
  stream.setByteOrder( QDataStream::LittleEndian );

  if( ( mode == IO_ReadOnly ) || ( mode == IO_ReadWrite ) )
    load();
  else
    create();
}

// required: file already opened and stream has been set
void StorageInfo::load()
{
  unsigned bb_shift;  // should be 9 (2^9 = 1024)
  unsigned sb_shift;  // should be 6 (2^6 = 64)
  unsigned num_bat;   // blocks for big-BAT
  unsigned num_sbat;  // blocks for small-BAT
  unsigned num_mbat;  // blocks for meta-BAT
  QByteArray dirent;  // data for directory entry

  // find size of the file
  filesize = stream.device()->size();

  // load file header (first 512 bytes)
  header.resize( 512 );
  stream.device()->at( 0 );
  stream.readRawBytes( header.data(), header.size() );

  // check OLE magic id
  if( ( readU32(header,0) != 0xe011cfd0 ) && 
      ( readU32(header,4) != 0xe11ab1a1 ) )
  {
    result = Storage::NotOLE;
    return;
  }

  // load important variables
  bb_shift = readU16( header, 0x1e );
  sb_shift = readU16( header, 0x20 );
  num_bat = readU32( header, 0x2c );
  dirent_start = readU32( header, 0x30 );
  threshold = readU32( header, 0x38 );
  sbat_start = readU32( header, 0x3c );
  num_sbat = readU32( header, 0x40 );
  mbat_start = readU32( header, 0x44 );
  num_mbat = readU32( header, 0x48 );

  // sanity checks !!
  if( ( threshold != 4096 ) || ( num_bat == 0 ) || 
    ( sb_shift > bb_shift ) || ( bb_shift <= 6 ) || ( bb_shift >=31 ) ) 
  {
    result = Storage::BadOLE;
    return;
  }

  // important block size
  bb_size = 1 << bb_shift;
  sb_size = 1 << sb_shift;

  QValueVector<int> chain;
  QByteArray buffer;

  // load blocks in meta BAT
  QValueVector<int> metabat;
  metabat.resize( num_mbat * bb_size / 4 );
  unsigned q = 0;
  for( unsigned r=0; r<num_mbat; r++ )
  {
    QByteArray metabuf( bb_size );
    loadBigBlock( mbat_start+r, metabuf.data(), metabuf.size() );
    for( unsigned s=0; s< bb_size/4; s++ )
      metabat[q++] = readU32( metabuf, s*4 );
  }

  // load big BAT, keep it in memory
  // note that first 109 blocks are defined in header
  // the rest are from meta BAT
  chain.resize( num_bat);
  for( unsigned i=0; i< num_bat; i++ )
    chain[i] = (i<=109) ? readU32( header, 0x4C+i*4 ) : 
      metabat[i-109];
  buffer.resize( chain.size() * bb_size );
  loadBigBlocks( chain, buffer.data(), buffer.size() );
  bb.parse( buffer );

  // load small BAT, also keep in memory
  // to get blocks for small BAT, follow chain in big BAT
  chain = bb.follow( sbat_start );
  buffer.resize( chain.size() * bb_size );
  loadBigBlocks( chain, buffer.data(), buffer.size() );
  sb.parse( buffer );

  // construct root directory tree
  chain = bb.follow( dirent_start );
  buffer.resize( chain.size() * bb_size );
  loadBigBlocks( chain, buffer.data(), buffer.size() );
  dirent.duplicate( buffer );
  if( !dirent.size() )
  {
    result = Storage::BadOLE;
    return;
  }

  root = buildTree( (Entry*)0L, 0, dirent );

  // fetch block chain as data for small-files
  sb_blocks = bb.follow( readU32( dirent, 0x74 ) );

  // start with root directory
  current_dir = root;
}

void StorageInfo::create()
{
  threshold = 4096;
  bb_size = 512;
  sb_size = 64;

  // construct a blank header, fill only a few values
  // all header will be fixed-up later on flush()
  header.resize( 512 );
  for( unsigned h=0; h<512; h++ )
    header[h] = 0;
  writeU32( header, 0x00, magic1 );
  writeU32( header, 0x04, magic2 );
  writeU32( header, 0x18, (Q_UINT32)0x003B ); // revision ?
  writeU32( header, 0x1A, (Q_UINT32)0x0003 ); // version ?
  writeU32( header, 0x1C, (Q_UINT32)0xfffe ); 

  // one BAT block can address 128 blocks, approx 64 KB


}

// destruct everything, relatively easy
StorageInfo::~StorageInfo()
{
  flush();

  for( unsigned i = 0; i < streams.count(); i++ )
  {
    QDataStream* s = streams.at( i );
    if( !s ) continue;
    if( s->device() ) delete s->device();
    delete s;
  }

  if( root ) delete root;

  if( file.isOpen() ) file.close();
}

void StorageInfo::flush()
{
  // DON'T DO ANYTHING if ReadOnly !!
  if( mode == IO_ReadOnly ) return;

  // fix-up header
  QByteArray header( 512 );
  for( unsigned i=0; i<512; i++ ) header[i] = 0;
  writeU32( header, 0, magic1 );
  writeU32( header, 0, magic2 );

}

// return number of bytes which has been read
Q_ULONG StorageInfo::loadBigBlocks( QValueVector<int> blocks, char* data, Q_ULONG maxlen )
{
  // sentinel
  if( !data ) return 0;
  if( !file.isOpen() ) return 0;
  if( blocks.size() < 1 ) return 0;
  if( maxlen == 0 ) return 0;


  // read block one by one, could be optimized but seems fast enough :-)
  unsigned long bytes = 0;
  for( unsigned i=0; ( i<blocks.size() ) & ( bytes<maxlen ); i++ )
  {
     int block = blocks[i];
     if( block < 0 ) continue;
     stream.device()->at( bb_size * ( block+1 ) );
     unsigned p = QMIN(bb_size,maxlen-bytes);
     stream.readRawBytes( data + bytes, p );
     bytes += p;
  }

  return bytes;
}

Q_ULONG StorageInfo::loadBigBlock( int block, char* data, Q_ULONG maxlen )
{
  // sentinel
  if( !data ) return 0;
  if( !file.isOpen() ) return 0;
  if( block < 0 ) return 0;

  // wraps call for loadBigBlocks
  QValueVector<int> blocks;
  blocks.resize( 1 );
  blocks[0] = block;

  return loadBigBlocks( blocks, data, maxlen );
}

// return number of bytes which has been read
Q_ULONG StorageInfo::loadSmallBlocks( QValueVector<int> blocks, char* data, Q_ULONG maxlen )
{
  // sentinel
  if( !data ) return 0;
  if( !file.isOpen() ) return 0;
  if( blocks.size() < 1 ) return 0;
  if( maxlen == 0 ) return 0;

  // our own local buffer
  QByteArray buf( bb_size );

  // read small block one by one
  unsigned long bytes = 0;
  for( unsigned i=0; ( i<blocks.size() ) & ( bytes<maxlen ); i++ )
  {
     int block = blocks[i];
     if( block < 0 ) continue;

     // find where the small-block exactly is
     unsigned pos = block * sb_size;
     int bbindex = pos / bb_size;
     if( bbindex >= (int)sb_blocks.size() ) break;

     loadBigBlock( sb_blocks[ bbindex ], buf.data(), buf.size() );

     // copy the data
     unsigned offset = pos % bb_size;
     unsigned p = QMIN(sb_size,QMIN(maxlen-bytes,buf.size()-offset));
     memcpy( data + bytes, buf.data() + offset, p );
     bytes += p;
  }

  return bytes;
}

Q_ULONG StorageInfo::loadSmallBlock( int block, char* data, Q_ULONG maxlen )
{
  // sentinel
  if( !data ) return 0;
  if( !file.isOpen() ) return 0;
  if( block < 0 ) return 0;

  // wraps call for loadSmallBlocks
  QValueVector<int> blocks;
  blocks.resize( 1 );
  blocks[0] = block;

  return loadSmallBlocks( blocks, data, maxlen );
}

// recursive function to construct directory tree
Entry* StorageInfo::buildTree( Entry* parent, int index, QByteArray dirent )
{
  Entry* entry = (Entry*) 0L;

  // find where to start
  unsigned p = index * 128;

  // would be < 32 if first char in the name isn't printable
  unsigned prefix = 32;

  // parse name of this entry, which stored as Unicode 16-bit 
  QString name;
  int name_len = readU16( dirent, 64+p );
  for( int j=0; ( dirent[j+p]) && (j<name_len); j+= 2 )
    name.append( QChar( dirent[j+p]) );

  // emtpy name ?  
  if( name.isEmpty() ) return entry;

  // first char isn't printable ? remove it...
  if( dirent[p] < 32 ){ prefix = dirent[0]; name.remove( 0,1 ); }

  // type of this entry will decide which Entry should be created
  // 2 = file (aka stream), 1 = directory (aka storage), 5 = root
  unsigned type  = dirent[ 0x42 + p];
  if( ( type == 2 ) || ( type == 1 ) || ( type == 5 ) )
    entry = new Entry();
  if( entry ) entry->dir = ( ( type = 1 ) || ( type == 5 ) );

  // barf on error 
  if( !entry ) return entry;

  // fetch important data
  entry->name = name;
  entry->start = readU32( dirent, 0x74+p );
  entry->size  = readU32( dirent, 0x78+p );

  // append as another child 
  entry->parent = parent;
  if( parent ) parent->children.append( entry );

  // check previous
  int prev  = readU32( dirent, 0x44+p );
  if( prev >= 0 ) buildTree( parent, prev, dirent );

  // traverse to sub
  int dir   = readU32( dirent, 0x4C+p );
  if( entry->dir && (dir > 0 ))
    buildTree( entry, dir, dirent );

  // check next
  int next  = readU32( dirent, 0x48+p );
  if( next >= 0 ) buildTree( parent, next, dirent );

  return entry;
}

// given an entry, find a complete path from root
QString StorageInfo::fullName( Entry* e )
{
  if( !e ) return QString::null;

  QString result;

  while( e->parent )
  {
    result.prepend( e->name );
    result.prepend( "/" );
    e = e->parent;
  }

  // don't use specified root name (like "Root Entry")
  if( result.isEmpty() ) result = "/";

  return result;
}

// given a fullname (e.g "/ObjectPool/_1020961869"), find the entry
Entry* StorageInfo::entry( const QString& name )
{
  Entry* entry = (Entry*) 0L;

  // sanity check
  if( !root ) return (Entry*) 0L;

  // start from root
  entry = root;

  QStringList nl = QStringList::split( '/', name );
  for( unsigned i=0; i< nl.size(); i++ )
  {
     Entry *child = (Entry*) 0L;

     if( entry->dir )
       for( unsigned j=0; j < entry->children.count(); j++ )
         if( entry->children.at(j)->name == nl[i] )
           child = entry->children.at(j);
     if( !child ) return (Entry*) 0L;
     entry = child;
  }

  return entry;
}

