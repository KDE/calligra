/* POLE - Portable library to access OLE Storage 
   Copyright (C) 2002 Ariya Hidayat <ariya@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, US
*/

#include <fstream>

#include <polestorage.h>
#include <polestorageio.h>

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
  int om = ios::in;
  if( mode == Storage::WriteOnly ) om = std::ios::out;
  if( mode == Storage::ReadWrite) om = std::ios::app;
  om |= std::ios::binary;
  file.open( filename.c_str(), om );

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

  // std::cout << "loading " << filename << "\n" ;

  // find size of input file
  file.seekg( 0, std::ios::end );
  filesize = file.tellg();

  // load header
  file.seekg( 0 ); 
  file.read( header, sizeof( header ) );
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
    file.seekg( bb_size * ( block+1 ) );
    unsigned long p = (bb_size < maxlen-bytes) ? bb_size : maxlen-bytes;
    file.read( data + bytes, p );
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
