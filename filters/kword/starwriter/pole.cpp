/* POLE - Portable library to access OLE Storage
   Copyright (C) 2002-2003 Ariya Hidayat <ariya@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA
*/

#include <fstream>
#include <string>
#include <list>
#include <iostream>
#include <vector>

#include "pole.h"

namespace POLE
{

class Entry
{
  public:
    Entry* parent;
    std::string name;
    unsigned long size;
    unsigned start;
    bool dir;
    std::vector<Entry*> children;
    Entry();
    ~Entry();
  private:
    Entry( const Entry& );
    Entry& operator=( const Entry& );
};

class AllocTable
{
  public:

    AllocTable();

    ~AllocTable();

    bool dirty;

    void clear();

    unsigned long size();

    void resize( unsigned long newsize );

    void set( unsigned long index, unsigned long val );

    std::vector<unsigned long> follow( unsigned long start );

    unsigned long operator[](unsigned long index );

  private:

    std::vector<unsigned long> data;

    AllocTable( const AllocTable& );

    AllocTable& operator=( const AllocTable& );
};

class StorageIO
{
  public:

    /// result of operation
    int result;

    /// owner of this object
    Storage* doc;

    /// filename (possible required)
    std::string filename;

    /// working mode: ReadOnly, WriteOnly, or ReadWrite
    int mode;

    /// working file
    std::fstream file;

    /// size of the file
    unsigned long filesize;

    /// header (first 512 byte)
    unsigned char header[512];

    /// magic id, first 8 bytes of header
    unsigned char magic[8];

    /// switch from small to big file (usually 4K)
    unsigned threshold;

    /// size of big block (should be 512 bytes)
    unsigned bb_size;

    ///  size of small block (should be 64 bytes )
    unsigned sb_size;

    /// allocation table for big blocks
    AllocTable bb;
    
    /// allocation table for small blocks
    AllocTable sb;

    /// starting block index to store small-BAT
    unsigned sbat_start;

    /// blocks where to find data for "small" files
    std::vector<unsigned long> sb_blocks;

    /// starting block to store meta BAT
    unsigned mbat_start;
    
    /// starting block index to store directory info
    unsigned dirent_start;

    /// root directory entry
    Entry* root;
    
    /// current directory entry
    Entry* current_dir;

    /// constructor
    StorageIO( Storage* storage, const char* fileName, int mode );

    /// destructor
    ~StorageIO();

    void flush();

    unsigned long loadBigBlocks( std::vector<unsigned long> blocks, unsigned char* buffer, unsigned long maxlen );

    unsigned long loadBigBlock( unsigned long block, unsigned char* buffer, unsigned long maxlen );

    unsigned long loadSmallBlocks( std::vector<unsigned long> blocks, unsigned char* buffer, unsigned long maxlen );

    unsigned long loadSmallBlock( unsigned long block, unsigned char* buffer, unsigned long maxlen );

    /// construct directory tree
    Entry* buildTree( Entry* parent, int index, const unsigned char* dirent );

    std::string fullName( Entry* e );

    /// given a fullname (e.g "/ObjectPool/_1020961869"), find the entry
    Entry* entry( const std::string& name );


  private:

    void load();

    void create();

    // no copy or assign
    StorageIO( const StorageIO& );
    StorageIO& operator=( const StorageIO& );

};

class StreamIO
{
  public:

    StreamIO( StorageIO* io, Entry* entry );

    ~StreamIO();

    unsigned long size();

    void seek( unsigned long pos );

    unsigned long tell();

    int getch();

    unsigned long read( unsigned char* data, unsigned long maxlen );

    unsigned long read( unsigned long pos, unsigned char* data, unsigned long maxlen );

    StorageIO* io;

    Entry* entry;


  private:

    std::vector<unsigned long> blocks;

    // no copy or assign
    StreamIO( const StreamIO& );
    StreamIO& operator=( const StreamIO& );

    // pointer for read
    unsigned long m_pos;

    // simple cache system to speed-up getch()
    unsigned char* cache_data;
    unsigned long cache_size;
    unsigned long cache_pos;
    void updateCache();

};

}; // namespace POLE

using namespace POLE;

static inline unsigned long readU16( const unsigned char* ptr )
{
  return ptr[0]+(ptr[1]<<8);
}

static inline unsigned long readU32( const unsigned char* ptr )
{
  return ptr[0]+(ptr[1]<<8)+(ptr[2]<<16)+(ptr[3]<<24);
}

static inline void writeU16( unsigned char* ptr, unsigned long data )
{
  ptr[0] = data & 0xff;
  ptr[1] = (data >> 8) & 0xff;
}

static inline void writeU32( unsigned char* ptr, unsigned long data )
{
  ptr[0] = data & 0xff;
  ptr[1] = (data >> 8) & 0xff;
  ptr[2] = (data >> 16) & 0xff;
  ptr[3] = (data >> 24) & 0xff;
}

static const unsigned char pole_magic[] = 
 { 0xd0, 0xcf, 0x11, 0xe0, 0xa1, 0xb1, 0x1a, 0xe1 };
 
Entry::Entry()
{
  name = "Unnamed";
  size = 0;
  dir = false;
}

Entry::~Entry()
{ 
  for( unsigned i=0; i<children.size(); i++ )
  {
    Entry* entry = children[ i ];
    delete entry;
  }
}

AllocTable::AllocTable()
{
  dirty = false;
}

AllocTable::~AllocTable()
{
}

unsigned long AllocTable::size()
{
  return data.size();
}

void AllocTable::resize( unsigned long newsize )
{
  data.resize( newsize );
}

unsigned long AllocTable::operator[]( unsigned long index )
{
  unsigned long result;
  result = data[index];
  return result;
}

void AllocTable::set( unsigned long index, unsigned long value )
{
  if( index >= size() ) resize( index + 1);
  data[ index ] = value;
}

std::vector<unsigned long> AllocTable::follow( unsigned long start )
{
  std::vector<unsigned long> chain;

  if( start >= size() ) return chain; 

  unsigned long p = start;
  while( p < size() )
{
    if( p >= (unsigned long)0xfffe ) break;
    if( p >= size() ) break;
    chain.push_back( p );
    if( data[p] >= size() ) break;
    p = data[ p ];
}

  return chain;
}

// =========== StorageIO ==========

StorageIO::StorageIO( Storage* storage, const char* fname, int m ):
  doc( storage ), filename( fname), mode( m )
{
  // initialization
  result = Storage::Ok; 
  root = (Entry*) 0L;
  current_dir = (Entry*) 0L;
  bb_size = 512;
  sb_size = 64;
  threshold = 4096;

  // prepare input stream
  int om = std::ios::in;
  if( mode == Storage::WriteOnly ) om = std::ios::out;
  if( mode == Storage::ReadWrite) om = std::ios::app;
  om |= std::ios::binary;
  //file.open( filename.c_str(), om );
  file.open( filename.c_str(), std::ios::binary | std::ios::in );

  // check for error
  if( !file.good() )
  {
    std::cerr << "Can't open " << filename << "\n\n";
    result = Storage::OpenFailed;
    return;
  }

  // assume we'll be just fine
  result = Storage::Ok;

  if( mode == Storage::WriteOnly ) create();
  else load();

}

StorageIO::~StorageIO()
{
  filename = std::string();
  flush();
  file.close();
}

void StorageIO::load()
{
  unsigned bb_shift;  // should be 9 (2^9 = 1024)
  unsigned sb_shift;  // should be 6 (2^6 = 64)
  unsigned num_bat;   // blocks for big-BAT
  unsigned num_sbat;  // blocks for small-BAT
  unsigned num_mbat;  // blocks for meta-BAT

  // find size of input file
  file.seekg( 0, std::ios::end );
  filesize = file.tellg();

  // load header
  file.seekg( 0 ); 
  file.read( (char*)header, sizeof( header ) );
  if( file.gcount() != sizeof( header ) )
  {
    result = Storage::NotOLE;
    return;
  }

  // check OLE magic id
  for( unsigned i=0; i<8; i++ )
  {
    magic[i] = header[i];
    if( magic[i] != pole_magic[i] )
    {
      result = Storage::NotOLE;
      return;
    }
  }

  // load important variables
  bb_shift = readU16( header + 0x1e );
  sb_shift = readU16( header + 0x20 );
  num_bat = readU32( header + 0x2c );
  dirent_start = readU32( header + 0x30 );
  threshold = readU32( header + 0x38 );
  sbat_start = readU32( header + 0x3c );
  num_sbat = readU32( header + 0x40 );
  mbat_start = readU32( header + 0x44 );
  num_mbat = readU32( header + 0x48 );

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

  std::vector<unsigned long> chain;
  unsigned char buffer[bb_size];

  // load blocks in meta BAT
  std::vector<unsigned long> metabat( num_mbat * bb_size / 4 );
  unsigned q = 0;
  for( unsigned r=0; r<num_mbat; r++ )
  {
    unsigned char metabuf[ bb_size ];
    loadBigBlock( mbat_start+r, metabuf, sizeof( metabuf ) );
    for( unsigned s=0; s< bb_size/4; s++ )
      metabat[q++] = readU32( metabuf + s*4 ); 
  }

  // load big BAT, keep it in memory
  // each entry in alloc table takes 4 bytes
  bb.resize( num_bat * bb_size / 4 );
  for( unsigned long i=0; i< num_bat; i++ )
  {
    // note that first 109 blocks are defined in header
    // the rest are from meta BAT
    unsigned long block = (i<109) ? readU32( header + 0x4C+i*4 ) : metabat[i-109];

    loadBigBlock( block, buffer, bb_size );

    for( unsigned m=0; m<bb_size/4; m++)
      bb.set( i*bb_size/4 + m, readU32( buffer + m*4 ) );
  }

  // load small BAT, also keep in memory
  // to get blocks for small BAT, follow chain in big BAT
  chain = bb.follow( sbat_start );
  //sb.resize( chain.size() * bb_size / 4 );
  for( unsigned long i=0; i < chain.size(); i++ )
  {
    unsigned long block = chain[i];
    loadBigBlock( block, buffer, bb_size );
    for( unsigned m=0; m<bb_size/4; m++ )
      sb.set( i*bb_size/4 + m, readU32( buffer + m*4 ) ); 
  }

  // construct root directory tree
  unsigned char* dirent;
  chain = bb.follow( dirent_start );
  unsigned bufsize = chain.size() * bb_size;
  dirent = new unsigned char[bufsize];
  loadBigBlocks( chain, dirent, bufsize );
  root = buildTree( (Entry*)0L, 0, dirent );
  
  // fetch block chain as data for small-files
  sb_blocks = bb.follow( readU32( dirent + 0x74 ) );

  delete [] dirent;
  
  // start with root directory
  current_dir = root;

  // done, without error
  result = Storage::Ok;
}

// unsupported yet
void StorageIO::create()
{
  unsigned bb_shift = 9;
  unsigned sb_shift = 6;
  bb_size = 1 << bb_shift;
  sb_size = 1 << sb_shift;
  threshold = 4096;

  // construct blank header
  // only few important parts, the rest will be fixed-up in flush()
  for( int i=0; i<8; i++ ) header[i] = pole_magic[i];
  writeU32( header + 8, 0 );
  writeU32( header + 12, 0 );
  writeU32( header + 16, 0 );
  writeU16( header + 24, 0x3B00 );  // revision ?
  writeU16( header + 26, 3 );       // version ?
  writeU16( header + 28, 0xfffe );  // unknown
  writeU16( header + 30, bb_shift );
  writeU16( header + 32, sb_shift );
  writeU16( header + 34, 0 );
  writeU32( header + 36, 0 );
  writeU32( header + 40, 0 );
  writeU32( header + 52, 0 );
  for( int j=0x4C; j<512; j++ ) header[j] = 0xff;

  // done, without error
  result = Storage::Ok;
}

void StorageIO::flush()
{
  if( mode == Storage::ReadOnly ) return;

  // header fix-up
  writeU32( header + 0x30, dirent_start );
  writeU32( header + 0x38, threshold );
  writeU32( header + 0x3c, sbat_start );
  writeU32( header + 0x44, mbat_start );

  // write the header



  // dirty ?
}

unsigned long StorageIO::loadBigBlocks( std::vector<unsigned long> blocks,
  unsigned char* data, unsigned long maxlen )
{
  // sentinel
  if( !data ) return 0;
  if( !file.good() ) return 0;
  if( blocks.size() < 1 ) return 0;
  if( maxlen == 0 ) return 0;

  // read block one by one, seems fast enough
  unsigned long bytes = 0;
  for( unsigned long i=0; (i < blocks.size() ) & ( bytes<maxlen ); i++ )
  {
    unsigned long block = blocks[i];
    if( block < 0 ) continue;
    unsigned long pos =  bb_size * ( block+1 );
    unsigned long p = (bb_size < maxlen-bytes) ? bb_size : maxlen-bytes;
    if( pos + p > filesize ) p = filesize - pos;
    file.seekg( pos );
    file.read( (char*)data + bytes, p );
    bytes += p;
  }

  return bytes;
}

unsigned long StorageIO::loadBigBlock( unsigned long block,
  unsigned char* data, unsigned long maxlen )
{
  // sentinel
  if( !data ) return 0;
  if( !file.good() ) return 0;
  if( block < 0 ) return 0;

  // wraps call for loadBigBlocks
  std::vector<unsigned long> blocks;
  blocks.resize( 1 );
  blocks[ 0 ] = block;

  return loadBigBlocks( blocks, data, maxlen );
}

// return number of bytes which has been read
unsigned long StorageIO::loadSmallBlocks( std::vector<unsigned long> blocks,
  unsigned char* data, unsigned long maxlen )
{
  // sentinel
  if( !data ) return 0;
  if( !file.good() ) return 0;
  if( blocks.size() < 1 ) return 0;
  if( maxlen == 0 ) return 0;

  // our own local buffer
  unsigned char buf[ bb_size ];

  // read small block one by one
  unsigned long bytes = 0;
  for( unsigned long i=0; ( i<blocks.size() ) & ( bytes<maxlen ); i++ )
  {
    unsigned long block = blocks[i];
    if( block < 0 ) continue;

    // find where the small-block exactly is
    unsigned long pos = block * sb_size;
    unsigned long bbindex = pos / bb_size;
    if( bbindex >= sb_blocks.size() ) break;

    loadBigBlock( sb_blocks[ bbindex ], buf, bb_size );

    // copy the data
    unsigned offset = pos % bb_size;
    unsigned long p = (maxlen-bytes < bb_size-offset ) ? maxlen-bytes :  bb_size-offset;
    p = (sb_size<p ) ? sb_size : p;
    memcpy( data + bytes, buf + offset, p );
    bytes += p;
  }

  return bytes;
}

unsigned long StorageIO::loadSmallBlock( unsigned long block,
  unsigned char* data, unsigned long maxlen )
{
  // sentinel
  if( !data ) return 0;
  if( !file.good() ) return 0;
  if( block < 0 ) return 0;

  // wraps call for loadSmallBlocks
  std::vector<unsigned long> blocks;
  blocks.resize( 1 );
  blocks.assign( 1, block );

  return loadSmallBlocks( blocks, data, maxlen );
}

// recursive function to construct directory tree
Entry* StorageIO::buildTree( Entry* parent, int index, const unsigned char* dirent )
{
  Entry* entry = (Entry*) 0L;

  // find where to start
  unsigned p = index * 128;

  // would be < 32 if first char in the name isn't printable
  unsigned prefix = 32;

  // parse name of this entry, which stored as Unicode 16-bit
  std::string name;
  int name_len = readU16( dirent + 64+p );
  for( int j=0; ( dirent[j+p]) && (j<name_len); j+= 2 )
     name.append( 1, dirent[j+p] );

  // emtpy name ?
  if( !name.length() ) return entry;

  // first char isn't printable ? remove it...
  if( dirent[p] < 32 ){ prefix = dirent[0]; name.erase( 0,1 ); }

  // type of this entry will decide which Entry should be created
  // 2 = file (aka stream), 1 = directory (aka storage), 5 = root
  unsigned type  = dirent[ 0x42 + p];
  if( ( type == 2 ) || ( type == 1 ) || ( type == 5 ) ) entry = new Entry();
  if( entry ) entry->dir = ( ( type = 1 ) || ( type == 5 ) );

  // barf on error
  if( !entry ) return entry;

  // fetch important data
  entry->name = name;
  entry->start = readU32( dirent + 0x74+p );
  entry->size  = readU32( dirent + 0x78+p );

  // append as another child
  entry->parent = parent;
  if( parent ) parent->children.push_back( entry );

  // check previous
  int prev  = readU32( dirent + 0x44+p );
  if( prev >= 0 ) buildTree( parent, prev, dirent );
  
  // traverse to sub
  int dir   = readU32( dirent + 0x4C+p );
  if( entry->dir && (dir > 0 ))
       buildTree( entry, dir, dirent );

  // check next
  int next  = readU32( dirent + 0x48+p );
  if( next >= 0 ) buildTree( parent, next, dirent );
  return entry;
}

// given an entry, find a complete path from root
std::string StorageIO::fullName( Entry* e )
{
  if( !e ) return (const char*) 0L;

  std::string result;

  while( e->parent )
  {
     result.insert( 0, e->name );
     result.insert( 0,  "/" );
     e = e->parent;
  }

  // don't use specified root name (like "Root Entry")
  if( !result.length() ) result = "/";

  return result;
}

// given a fullname (e.g "/ObjectPool/_1020961869"), find the entry
Entry* StorageIO::entry( const std::string& name )
{
   Entry* entry = (Entry*) 0L;

   // sanity check
   if( !root ) return (Entry*) 0L;
   if( !name.length() ) return (Entry*) 0L;
   
   // start from root when name is absolute
   // or current directory when name is relative
   entry = (name[0] == '/' ) ? root : current_dir;

   // split the names, e.g  "/ObjectPool/_1020961869" will become:
   // "ObjectPool" and "_1020961869" 
   std::list<std::string> names;
   std::string::size_type start = 0, end = 0;
   while( start < name.length() )
   {
     end = name.find_first_of( '/', start );
     if( end == std::string::npos ) end = name.length();
     names.push_back( name.substr( start, end-start ) );
     start = end+1;
   }
  
   std::list<std::string>::iterator it; 
   for( it = names.begin(); it != names.end(); ++it )
   {
     std::string entryname = *it;
     Entry *child = (Entry*) 0L;
     if( entry->dir )
       for( unsigned j=0; j < entry->children.size(); j++ )
         if( entry->children[j]->name == entryname )
           child = entry->children[j];
     if( !child ) return (Entry*) 0L;
      entry = child;
   }

   return entry;
}

// =========== StreamIO ==========

StreamIO::StreamIO( StorageIO* _io, Entry* _entry ):
  io( _io ), entry( _entry ), m_pos( 0 ),
  cache_data( 0 ), cache_size( 0 ), cache_pos( 0 )
{
  blocks = ( entry->size >= io->threshold ) ? io->bb.follow( entry->start ) :
     io->sb.follow( entry->start );

  // prepare cache
  cache_size = 4096; // optimal ?
  cache_data = new unsigned char[cache_size];
  updateCache();
}

// FIXME tell parent we're gone
StreamIO::~StreamIO()
{
  delete[] cache_data;  
}

void StreamIO::seek( unsigned long pos )
{
  m_pos = pos;
}

unsigned long StreamIO::tell()
{
  return m_pos;
}

int StreamIO::getch()
{
  // past end-of-file ?
  if( m_pos > entry->size ) return -1;

  // need to update cache ?
  if( !cache_size || ( m_pos < cache_pos ) ||
    ( m_pos >= cache_pos + cache_size ) )
      updateCache();

  // something bad if we don't get good cache
  if( !cache_size ) return -1;

  int data = cache_data[m_pos - cache_pos];
  m_pos++;

  return data;
}

unsigned long StreamIO::read( unsigned long pos, unsigned char* data, unsigned long maxlen )
{
  // sanity checks
  if( !data ) return 0;
  if( maxlen == 0 ) return 0;

  unsigned long totalbytes = 0;
  
  if ( entry->size < io->threshold )
  {
    // small file
    unsigned long index = pos / io->sb_size;

    if( index >= blocks.size() ) return 0;

    unsigned char buf[ io->sb_size ];
    unsigned long offset = pos % io->sb_size;
    while( totalbytes < maxlen )
    {
      if( index >= blocks.size() ) break;
      io->loadSmallBlock( blocks[index], buf, io->bb_size );
      unsigned long count = io->sb_size - offset;
      if( count > maxlen-totalbytes ) count = maxlen-totalbytes;
      memcpy( data+totalbytes, buf + offset, count );
      totalbytes += count;
      offset = 0;
      index++;
    }

  }
  else
  {
    // big file
    unsigned long index = pos / io->bb_size;
    
    if( index >= blocks.size() ) return 0;
    
    unsigned char buf[ io->bb_size ];
    unsigned long offset = pos % io->bb_size;
    while( totalbytes < maxlen )
    {
      if( index >= blocks.size() ) break;
      io->loadBigBlock( blocks[index], buf, io->bb_size );
      unsigned long count = io->bb_size - offset;
      if( count > maxlen-totalbytes ) count = maxlen-totalbytes;
      memcpy( data+totalbytes, buf + offset, count );
      totalbytes += count;
      index++;
      offset = 0;
    }

  }

  return totalbytes;
}

unsigned long StreamIO::read( unsigned char* data, unsigned long maxlen )
{
  unsigned long bytes = read( tell(), data, maxlen );
  m_pos += bytes;
  return bytes;
}

void StreamIO::updateCache()
{
  // sanity check
  if( !cache_data ) return;

  cache_pos = m_pos - ( m_pos % cache_size );
  unsigned long bytes = cache_size;
  if( cache_pos + bytes > entry->size ) bytes = entry->size - cache_pos;
  cache_size = read( cache_pos, cache_data, bytes );
}


// =========== Storage ==========

Storage::Storage()
{
  io = (StorageIO*) 0L;
  result = Storage::Ok;
}

Storage::~Storage()
{
  close();
  delete io;
}

bool Storage::open( const char* fileName, int m )
{
  // only a few modes accepted
  if( ( m != ReadOnly ) && ( m != WriteOnly ) && ( m != ReadWrite ) )
  {
    result = UnknownError;
    return false;
  }

  io = new StorageIO( this, fileName, m );

  result = io->result;
  
  return result == Storage::Ok;
}

void Storage::flush()
{
  if( io ) io->flush();
}

void Storage::close()
{
  flush();
}

// list all files and subdirs in current path
std::list<std::string> Storage::listDirectory()
{
  std::list<std::string> entries;

  // sanity check
  if( !io ) return entries;
  if( !io->current_dir ) return entries;

  // sentinel: do nothing if not a directory
  if( !io->current_dir->dir ) return entries;

  // find all children belongs to this directory
  for( unsigned i = 0; i<io->current_dir->children.size(); i++ )
  {
    Entry* e = io->current_dir->children[i];
    if( e ) entries.push_back( e->name );
  }

  return entries;
}

// enters a sub-directory, returns false if not a directory or not found
bool Storage::enterDirectory( const std::string& directory )
{
  // sanity check
  if( !io ) return false;
  if( !io->current_dir ) return false;

  // look for the specified sub-dir
  for( unsigned i = 0; i<io->current_dir->children.size(); i++ )
  {
    Entry* e = io->current_dir->children[i];
    if( e ) if( e->name == directory ) 
      if ( e->dir )
      {
        io->current_dir = e;
        return true;
      }
  }

  return false;
}

// goes up one level (like cd ..)
void Storage::leaveDirectory()
{
  // sanity check
  if( !io ) return;
  if( !io->current_dir ) return;

  Entry* parent = io->current_dir->parent;
  if( parent ) if( parent->dir ) 
    io->current_dir = parent;
}

// note: without trailing "/"
std::string Storage::path()
{
  // sanity check
  if( !io ) return std::string();
  if( !io->current_dir ) return std::string();

  return io->fullName( io->current_dir );
}

Stream* Storage::stream( const std::string& name )
{
  // sanity check
  if( !name.length() ) return (Stream*) 0L;
  if( !io ) return (Stream*) 0L;

  // make absolute if necessary
  std::string fullName = name;
  if( name[0] != '/' ) fullName.insert( 0, path() + "/" );

  // find to which entry this stream associates
  Entry* entry =  io->entry( name );
  if( !entry ) return (Stream*) 0L;

  StreamIO* sio = new StreamIO( io, entry );
  Stream* s = new Stream( sio );

  return s;
}

// =========== Stream ==========

Stream::Stream( StreamIO* _io ):
  io( _io )
{
}

// FIXME tell parent we're gone
Stream::~Stream()
{
  delete io;
}

unsigned long Stream::tell()
{
  return io ? io->tell() : 0;
}

void Stream::seek( unsigned long newpos )
{
  if( io ) io->seek( newpos );
}

unsigned long Stream::size()
{
  return io ? io->entry->size : 0;
}

int Stream::getch()
{
  return io ? io->getch() : -1;
}

unsigned long Stream::read( unsigned char* data, unsigned long maxlen )
{
  return io ? io->read( data, maxlen ) : 0;
}

