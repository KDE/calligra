/* This file is part of the KDE project
   Copyright (C) 2002 Ariya Hidayat <ariyahidayat@yahoo.de>

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
 * Boston, MA 02110-1301, USA.
*/

/*
   The database layout for PalmDB files is described in 
   http://www.palmos.com/dev/support/docs/protein_books/FileFormats/Intro.html
*/

#include "palmdb.h"

#include <q3cstring.h>
#include <QDataStream>
#include <qdatetime.h>
#include <QFile>
#include <q3memarray.h>
#include <q3ptrlist.h>
#include <QString>

PalmDB::PalmDB()
{
  // some default values
  setName( "Unnamed" );
  setAttributes( 0 );
  setVersion( 0 );
  setCreationDate( QDateTime::currentDateTime() );
  setModificationDate( QDateTime::currentDateTime() );
  setLastBackupDate( QDateTime::currentDateTime() );
  setType( QString::null );
  setCreator( QString::null );

  // crash if autodelete ?
  records.setAutoDelete( TRUE );
}

PalmDB::~PalmDB()
{
  records.clear();
}

bool PalmDB::load( const char* filename )
{
  // open input file
  QFile in (filename);
  if (!in.open (QIODevice::ReadOnly))
    return FALSE;

  QDataStream stream;
  stream.setDevice (&in);

  unsigned filesize = stream.device()->size();
  if( filesize < 72 ) return FALSE;

  // always big-endian
  stream.setByteOrder (QDataStream::BigEndian);

  // now start to read PDB header (72 bytes)

  // read and encode database name
  // The name field is 32 bytes long, and is NUL terminated.
  // Use the length parameter of fromLatin1() anyway.
  quint8 name[32];
  for(int k = 0; k < 32; k++)
    stream >> name[k];
  m_name = QString::fromLatin1( (char*) name, 31 );

  // read database attribute
  quint16 attr;
  stream >> attr;
  m_attributes = attr;

  // read database version (app-specific)
  quint16 ver;
  stream >> ver;
  m_version = ver;

  // NOTE: PDB specifies date as number of seconds since 1 Jan 1904
  // QDateTime::setTime_t expects number of seconds since 1 Jan 1970
  // so, we make adjustment with a constant offset of 2082844800
  const int adjust = 2082844800;

  // read creation date
  quint32 creation;
  stream >> creation;
  m_creationDate.setTime_t( creation - adjust );

  // read modification date
  quint32 modification;
  stream >> modification;
  m_modificationDate.setTime_t( modification - adjust );

  // read last backup date
  quint32 lastbackup;
  stream >> lastbackup;
  m_lastBackupDate.setTime_t( lastbackup - adjust );

  // read modification number
  quint32 modnum;
  stream >> modnum;

  // read app info id and sort info id
  quint32 appid, sortid;
  stream >> appid;
  stream >> sortid;

  // read and encode database type
  quint8 dbt[4];
  stream >> dbt[0] >> dbt[1] >> dbt[2] >> dbt[3];
  m_type = QString::fromLatin1( (char*) dbt, 4 );

  // read and encode database creator
  quint8 dbc[4];
  stream >> dbc[0] >> dbc[1] >> dbc[2] >> dbc[3];
  m_creator = QString::fromLatin1( (char*) dbc, 4 );

  // read unique id seed
  quint32 idseed;
  stream >> idseed;
  m_uniqueIDSeed = idseed;

  // now start to read PDB record list (variable-length)

  // next record list
  // FIXME what to do with this ?
  quint32 nextlist;
  stream >> nextlist;

  // number of records
  quint16 numrec;
  stream >> numrec;

  // read entries in record list
  // find out location and size of each record
  Q3MemArray<unsigned> recpos( numrec );
  Q3MemArray<int> recsize( numrec );

  // FIXME any other better way to find record size ?
  for( int r = 0; r < numrec; r++ )
  {
    quint32 pos;
    quint8 flag, dummy;
    stream >> pos >> flag >> dummy >> dummy >> dummy;
    recpos[r] = pos; recsize[r] = filesize - pos;
    if( r> 0 ) recsize[r-1] = pos - recpos[r-1]; // fixup
  }

  // debugging
#ifdef PDB_DEBUG
  qDebug( "name: \"%s\"", m_name.latin1() );
  qDebug( "type: \"%s\"", m_type.latin1() );
  qDebug( "creator: \"%s\"", m_creator.latin1() );
  qDebug( "attributes: 0x%04X", m_attributes );
  qDebug( "version: 0x%04X", m_version );
  qDebug( "creation date: %s", m_creationDate.toString().latin1() );
  qDebug( "modification date: %s", m_modificationDate.toString().latin1() );
  qDebug( "last backup date: %s", m_lastBackupDate.toString().latin1() );
  qDebug( "number of records: %d", numrec );
  for( int r = 0; r < numrec; r++ )
    qDebug( "  rec %d at 0x%X size %d", r, recpos[r], recsize[r] );
#endif

  // load all records
  records.clear();
  for( int r = 0; r < numrec; r++ )
  {
    QByteArray* data = new QByteArray;

    if( recpos[r] < filesize )
      if( recsize[r] >= 0 )
        {
          data->resize( recsize[r] );
          stream.device()->at( recpos[r] );
# warning "kde4 port it"
          //for( int q = 0; q < recsize[r]; q++ )
            //{ quint8 c; stream >> c; data->at(q) = c; }
        }
    records.append( data );
  }

  // close input file
  in.close();

  return TRUE;
}

bool PalmDB::save( const char* filename )
{
  // open output file
  QFile out( filename );
  if( !out.open( QIODevice::WriteOnly ) )
    return FALSE;

  QDataStream stream;
  stream.setDevice( &out );

  // always big-endian
  stream.setByteOrder (QDataStream::BigEndian);

  // now write PDB header (72 bytes)

  // write database name
  setName( name() );
  const char *dbname = m_name.latin1();
  for( unsigned k=0; k<31; k++ )
  {
    quint8 c = (k<m_name.length()) ? dbname[k] : 0;
    stream << c;
  }
  {
    // NUL-terminate the database name
    quint8 c = 0;
    stream << c;
  }



  // write database attribute
  quint16 attr = m_attributes;
  stream << attr;

  // write database version (app-specific)
  quint16 ver = m_version;
  stream << ver;

  // reference date is 1 Jan 1904
  // see also note in function load() above
  QDateTime ref = QDateTime(QDate( 1904, 1, 1) );

  // write creation date
  quint32 creation = -m_creationDate.secsTo( ref );
  stream << creation;

  // write modification date
  quint32 modification = -m_modificationDate.secsTo( ref );
  stream << modification;

  // write last backup date
  quint32 lastbackup = -m_lastBackupDate.secsTo( ref );
  stream << lastbackup;

  // write modification number
  quint32 modnum = 0;
  stream << modnum;

  // write app info id and sort info id
  quint32 appid = 0, sortid = 0;
  stream << appid;
  stream << sortid;

  // write and encode database type
  quint8 dbt[4];
  const char *dbtype = m_type.latin1();
  for( int p=0; p<4; p++ ) dbt[p]=dbtype[p];
  stream << dbt[0] << dbt[1] << dbt[2] << dbt[3];

  // write and encode database creator
  quint8 dbc[4];
  const char *dbcreator = m_creator.latin1();
  for( int p=0; p<4; p++ ) dbc[p]=dbcreator[p];
  stream << dbc[0] << dbc[1] << dbc[2] << dbc[3];

  // write unique id seed
  quint32 idseed = 0;
  stream << idseed;

  // now start to read PDB record list (variable-length)

  // next record list
  quint32 nextlist = 0;
  stream << nextlist;

  // number of records
  quint16 numrec = records.count();
  stream << numrec;

  // where is the first record ?
  // 78 is size of PDB header, 2 is filler before data
  quint32 pos = 78 + 2;
  pos += records.count()*8;

  // write record list
  for( unsigned r = 0; r < records.count(); r++ )
  {
    quint8 flag = 0, dummy = 0;
    stream << pos;
    stream << flag;
    stream  << dummy << dummy << dummy;
    pos += records.at(r)->count();
  }

  // write 2-byte dummy
  quint16 filler = 0;
  stream << filler;

  // write all records
  for( unsigned r = 0; r < records.count(); r++ )
  {
    QByteArray *data = records.at( r );
    if( !data ) continue;
    for( unsigned j=0; j<data->count(); j++ )
      {
        quint8 c = data->at( j ); 
        stream << c;  
      }
  }

  // close output file
  out.close();

  return TRUE;
}

void PalmDB::setType( const QString& t )
{
  m_type = t;
  if( m_type.length() > 4 )
    m_type = m_type.left( 4 );
  while( m_type.length() < 4 )
    m_type.append( 32 );
}


void PalmDB::setCreator( const QString& c )
{
  m_creator = c;
  if( m_creator.length() > 4 )
    m_type = m_creator.left( 4 );
  while( m_creator.length() < 4 )
    m_creator.append( 32 );
}
