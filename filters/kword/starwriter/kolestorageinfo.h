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

#ifndef KOLE_STORAGEINFO_H
#define KOLE_STORAGEINFO_H

#include <qdatastream.h>
#include <qfile.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qvaluevector.h>

#include <kolestorage.h>

namespace KOLE
{

class Entry;

class EntryList: public QPtrList<Entry>
{
  public:
    EntryList(){ setAutoDelete(true); }
};

class Entry
{
  public:
    Entry* parent;
    QString name;
    unsigned size;
    unsigned start;
    bool dir;
    EntryList children;
    Entry();
    virtual ~Entry();
};

/**
 * Implements allocation table.
 */

class AllocTable
{
  public:

    bool dirty;

    AllocTable();

    QValueVector<int> follow( unsigned start );

    void parse( QByteArray data );

    unsigned size(){ return m_data.size(); }

    int operator[]( int index );

  private:

    // no copy or assign
    AllocTable( const AllocTable& );
    AllocTable& operator=( const AllocTable& );

    QValueVector<int> m_data;

};

/**
 * Holds various low-level information about OLE storage,
 */
class StorageInfo
{
  public:

    // magic id to identify OLE document
    static const Q_UINT32 magic1 =  0xe011cfd0;
    static const Q_UINT32 magic2 =  0xe11ab1a1;

    // owner (must be KOLE::Storage)
    Storage* doc;

    // keep track of slave streams (cause we must delete it)
    QPtrList<QDataStream> streams;

    // working mode: IO_ReadOnly, IO_WriteOnly, or IO_ReadWrite
    int mode;

    // working file
    QFile file;

    // working stream
    QDataStream stream;

    // size of the file
    unsigned filesize;

    // header (first 512 byte)
    QByteArray header;

    // switch from small to big file (usually 4K)
    unsigned threshold;

    //  size of big block (usually 512)
    unsigned bb_size;

    //  size of small block (should be 64)
    unsigned sb_size;

    // allocation table for big blocks
    AllocTable bb;

    // allocation table for small blocks
    AllocTable sb;

    // starting block index to store small-BAT
    unsigned sbat_start;

    // blocks where to find data for "small" files
    QValueVector<int> sb_blocks;

    // meta block allocation table
    AllocTable mb;

    // starting block to store meta BAT
    unsigned mbat_start;

    // starting block index to store directory info
    unsigned dirent_start;

    // root directory entry
    Entry* root;

    // current directory entry
    Entry* current_dir;

    // constructor 
    StorageInfo( Storage* _doc, const QString& fileName, int m );

    // destructor 
    ~StorageInfo();

    // result of operation
    int result;

    // flush changes
    void flush();

    // note: buffer must be large enough 
    Q_ULONG loadBigBlocks( QValueVector<int> blocks, char* buffer, Q_ULONG maxlen );

    // same as before, but only for one block
    Q_ULONG loadBigBlock( int block, char* buffer, Q_ULONG maxlen );

    // note: buffer must be large enough 
    Q_ULONG loadSmallBlocks( QValueVector<int> blocks, char* buffer, Q_ULONG maxlen );

    // same as before, but only for one block
    Q_ULONG loadSmallBlock( int block, char* buffer, Q_ULONG maxlen );

    // construct directory tree
    Entry* buildTree( Entry* parent, int index, QByteArray dirent );

    // find a full path of given directory entry
    QString fullName( Entry* entry );

    // find an entry for given name, null if not found
    Entry* entry( const QString& name );

  private:

    // no copy or assign
    StorageInfo( const StorageInfo& );
    StorageInfo& operator=( const StorageInfo& );

    void load();

    void create();

};

}

#endif // KOLE_STORAGEINFO_H
