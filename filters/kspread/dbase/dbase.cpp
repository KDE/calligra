/* This file is part of the KDE project
   Copyright (C) 2002 by Thomas Franke and Andreas Pietzowski <andreas@pietzowski.de>
                         Ariya Hidayat <ariyahidayat@yahoo.de>

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


#include <qdatetime.h>
#include <QDataStream>
#include <QFile>
#include <QString>
#include <qstringlist.h>
#include <q3ptrlist.h>

#include <dbase.h>

DBase::DBase(): m_recordCount( 0 )
{
  fields.setAutoDelete( true );
}

DBase::~DBase()
{
  fields.clear();
  close();
}

  // Headerdefinition in dBASE
  //
  //  Type                            char   Content
  //
  //  unsigned char version           0      dBASE-Version (3)
  //  unsigned char last_update[3]    1-3    Date of last update
  //  unsigned long records           4-7    Number of records
  //  unsigned short header_length    8-9    headerlength
  //  unsigned short record_length    10-11  recordlength
  //  unsigned char reserved[20]      12-31  reserverd info from dBase
  //

bool DBase::load( const QString& filename )
{

  m_file.setFileName( filename );
  if( !m_file.open(QIODevice::ReadOnly) )
    return false;

  m_stream.setDevice( &m_file );
  m_stream.setByteOrder( QDataStream::LittleEndian );

  unsigned filesize = m_file.size();

  // read dBASE version
  quint8 ver;
  m_stream >> ver;
  m_version = ver & 0x7f; // bit 7: has memo ?

  // only dBASE V.3 is supported
  if ( m_version != 3 )
     return false;

  // date of last update
  quint8 y, m, d;
  m_stream >> y >> m >> d;
  // because dBASE saves 102 instead of 2002 (very Y2K-save ;-)
  m_lastUpdate.setYMD( y+1900, m, d );

  // check for valid date
  if( !m_lastUpdate.isValid() ) return false;

  // number of records
  quint32 norec;
  m_stream >> norec;
  m_recordCount = norec;

  // header-length
  quint16 header_length;
  m_stream >> header_length;
  m_headerLength = header_length;

  // record-length
  quint16 record_length;
  m_stream >> record_length;
  m_recordLength = record_length;

  // read the remaining chars
  quint8 dummy;
  for (int foo = 0; foo < 20; ++foo)
    m_stream >> dummy;

  // size of file must match
  if( filesize < m_headerLength + m_recordLength * m_recordCount )
    return false;

  // Now read the headers of the columns and their type

  // Type                              char     Content
  //
  // unsigned char field_name[11]      0-10     Fieldname
  // unsigned char field_type          11       Fieldtype
  // unsigned long field_address       12-15    Fielddataaddress
  // unsigned char field_length        16       Fieldlength
  // unsigned char field_decimals      17       decimals
  // unsigned char reserved[14]        18-31    reserved for internal dBASE-stuff

  fields.clear();
  for( unsigned i = 1; i < m_headerLength/32; ++i )
  {
    DBaseField* field = new DBaseField;

    // columnn-name
    quint8 colname[12];
    for ( int j = 0; j < 11; ++j)
       m_stream >> colname[j];
    colname[11] = '\0';
    field->name = QString( (const char*) &colname[0] );

    // type of column
    quint8 coltype;
    m_stream >> coltype;
    switch( coltype )
    {
      case 'C': field->type = DBaseField::Character; break;
      case 'N': field->type = DBaseField::Numeric; break;
      case 'D': field->type = DBaseField::Date; break;
      case 'M': field->type = DBaseField::Memo; break;
      case 'L': field->type = DBaseField::Logical; break;
      default: field->type = DBaseField::Unknown; break;
    }

    // fileddataaddress
    quint32 addr;
    m_stream >> addr;

    // columnlength
    quint8 colsize;
    m_stream >> colsize;
    field->length = colsize;

    // decimals
    quint8 decimals;
    m_stream >> decimals;
    field->decimals = decimals;

    // read remaining chars
    quint8 dummy;
    for ( int foo = 0; foo < 14; ++foo )
      m_stream >> dummy;

    // now append
    fields.append( field );
  }

  // set the index to the first record
  m_stream.device()->seek( m_headerLength );

  return true;
}

QStringList DBase::readRecord( unsigned recno )
{
  QStringList result;

  // out of range ? return empty strings
  if( recno >= m_recordCount )
  {
    for( unsigned i=0; i<fields.count(); i++)
      result.append( "" );
    return result;
  }

  // seek to where the record is
  qint64 filepos = m_headerLength + recno * m_recordLength;
  m_stream.device()->seek( filepos );

  // first char == '*' means the record is deleted
  // so we just skip it
  quint8 delmarker;
  m_stream >> delmarker;
  if( delmarker == 0x2a )
   return result;

  // load it
  for( unsigned i=0; i<fields.count(); i++ )
    switch( fields.at(i)->type )
    {
      // Numeric or Character
      case DBaseField::Numeric:
      case DBaseField::Character:
      {
        QString str;
        quint8 ch;
        for( unsigned j=0; j<fields.at(i)->length; j++ )
        {  m_stream >> ch; str += QChar(ch); }
        result.append( str );
      }  break;

      // Logical
      case DBaseField::Logical:
      {
        quint8 ch;
        m_stream >> ch;
        switch( ch )
        {
          case 'Y': case 'y': case 'T': case 't': result.append( "True" ); break;
          case 'N': case 'n': case 'F': case 'f': result.append( "False" ); break;
          default: result.append( "" ); break;
        }
      } break;

      // Date, stored as YYYYMMDD
      // Note: convert it to YYYY-MM-DD
      case DBaseField::Date:
      {
        QString str;
        quint8 ch;
        for( unsigned j=0; j<fields.at(i)->length; j++ )
        {  m_stream >> ch; str += QChar(ch); }
        str.insert( 6, '-' );
        str.insert( 4, '-' );
        result.append( str );
      } break;

      // Unknown/Unimplemented
      case DBaseField::Unknown:
      case DBaseField::Memo:
      default:
        result.append( "" ); // unknown
        break;
    }

  return result;
}

void DBase::close()
{
  if( m_file.isOpen() ) m_file.close();
}
