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

#ifndef POLE_STORAGEIO_H
#define POLE_STORAGEIO_H

#include <fstream>
#include <string>
#include <vector>

#include <polestorage.h>

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

    // result of operation
    int result;

    // owner of this object
    Storage* doc;

    // filename (possible required)
    std::string filename;

    // working mode: ReadOnly, WriteOnly, or ReadWrite
    int mode;

    // working file
    std::fstream file;

    // size of the file
    unsigned long filesize;

    // header (first 512 byte)
    unsigned char header[512];

    // magic id, first 8 bytes of header
    unsigned char magic[8];

    // switch from small to big file (usually 4K)
    unsigned threshold;

    // size of big block (should be 512 bytes)
    unsigned bb_size;

    //  size of small block (should be 64 bytes )
    unsigned sb_size;

    // allocation table for big blocks
    AllocTable bb;
    
    // allocation table for small blocks
    AllocTable sb;

    // starting block index to store small-BAT
    unsigned sbat_start;

    // blocks where to find data for "small" files
    std::vector<unsigned long> sb_blocks;

    // starting block to store meta BAT
    unsigned mbat_start;
    
    // starting block index to store directory info
    unsigned dirent_start;

    // root directory entry
    Entry* root;
    
    // current directory entry
    Entry* current_dir;

    // constructor
    StorageIO( Storage* storage, const char* fileName, int mode );

    // destructor
    ~StorageIO();

    void flush();

    unsigned long loadBigBlocks( std::vector<unsigned long> blocks, unsigned char* buffer, unsigned long maxlen );

    unsigned long loadBigBlock( unsigned long block, unsigned char* buffer, unsigned long maxlen );

    unsigned long loadSmallBlocks( std::vector<unsigned long> blocks, unsigned char* buffer, unsigned long maxlen );

    unsigned long loadSmallBlock( unsigned long block, unsigned char* buffer, unsigned long maxlen );

    // construct directory tree
    Entry* buildTree( Entry* parent, int index, const unsigned char* dirent );

    std::string fullName( Entry* e );

    // given a fullname (e.g "/ObjectPool/_1020961869"), find the entry
    Entry* entry( const std::string& name );


  private:

    void load();

    void create();

    // no copy or assign
    StorageIO( const StorageIO& );
    StorageIO& operator=( const StorageIO& );

};


}

#endif // POLE_STORAGEIO_H
