/* This file is part of the KDE project
   Copyright (C) 2001 Ariya Hidayat <ariyahidayat@yahoo.de>

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
   Boston, MA 02111-1307, USA.
*/

#include "palmdoc.h"

#include <qcstring.h>
#include <qdatetime.h>
#include <qptrlist.h>
#include <qstring.h>

PalmDoc::PalmDoc(): PalmDB()
{
  m_result = PalmDoc::OK;
  setText( QString::null );
}

PalmDoc::~PalmDoc()
{
}

bool PalmDoc::load( const char* filename )
{
  bool ok;

  ok = PalmDB::load( filename );
  if( !ok )
  {
    m_result = PalmDoc::ReadError;
    return FALSE;
  }

  if( type() != "TEXt" )
  {
    qDebug( "Type is \"%s\", not \"TEXt\", so this is not Palm DOC!", type().latin1() );
    m_result = PalmDoc::InvalidFormat;
    return FALSE;
  }

  if( creator() != "REAd" )
  {
    qDebug( "Creator is \"%s\", not \"REAd\", so this is not Palm DOC!",
      creator().latin1() );
    m_result = PalmDoc::InvalidFormat;
    return FALSE;
  }

  // must have at least two records
  if( records.count() < 2 )
  {
    qDebug( "Palm DOC has at least 2 records!" );
    m_result = PalmDoc::InvalidFormat;
    return FALSE;
  }

  // the very first record is DOC header
  // NOTE: this is not PDB header (which is handled in PalmDB) !
  QByteArray header( *records.at( 0 ) );

  // format of the DOC
  int format = (header[0]<<8) + header[1];
  qDebug( "DOC format: %d (%s)", format,
     (format==1) ? "Plain" : (format==2) ? "Compressed" : "Unknown" );

  // supported is only Plain or Compressed
  if( ( format != 1 ) && ( format != 2 ) )
  {
    qDebug( "Unknown format of document!" );
    m_result = PalmDoc::InvalidFormat;
    return FALSE;
  }

  // initialize
  setText( QString::null );

  // if the text is compressed, then uncompress each record
  if( format == 2 )
    for( unsigned r = 1; r < records.count(); r++ )
    {
      QByteArray rec( *records.at( r ) );
      m_text.append( uncompress( rec ) );
    }

  // if the text is not compressed, assemble the records
  if( format == 1 )
    for( unsigned r = 1; r < records.count(); r++ )
    {
      QByteArray rec( *records.at( r ) );
      m_text.append( QString::fromLatin1( rec.data(),rec.count() ) );
    }

  // done
  m_result = OK;
  return TRUE;
}

bool PalmDoc::save( const char* filename )
{
  // set proper database type and creator
  setType( "TEXt" );
  setCreator( "REAd" );

  // "touch" the database :-)
  setModificationDate( QDateTime::currentDateTime() ); 

  // Palm record size is always 4 KB
  unsigned recsize = 4096;

  // compress the text
  QByteArray data = compress( text() );  

  // prepare the records
  records.clear();
  for( unsigned i=0; i<data.count(); i++ )
  {
    QByteArray* ptr = new QByteArray;
    unsigned rs = data.count() - i;
    if( rs > recsize ) rs = recsize;
    ptr->resize( rs );
    for( unsigned m=0; m<rs; m++ )
      (*ptr)[m] = data[i++];
    --i;
    records.append( ptr );
  } 

  // prepare the header 
  QByteArray header( 16 );
  int docsize = m_text.length();
  header[0] = 0; header[1] = 2;  // 1=plain, 2=compressed 
  header[2] = header[3] = 0; // reserved word, set to 0
  header[4] = (docsize >> 24) & 255; // uncompressed size
  header[5] = (docsize >> 16) & 255;
  header[6] = (docsize >> 8) & 255;
  header[7] = docsize & 255;  
  header[8] = data.count()>> 8; // no of records
  header[9] = data.count() & 255; 
  header[10] = recsize >>8; // record size
  header[11] = recsize & 255;
  header[12] = header[13] = 0;
  header[14] = header[15] = 0;
 
  // header should be the very first record  
  records.prepend( new QByteArray( header ) );

  // write to file 
  bool ok = PalmDB::save( filename );
  if( !ok )
  {
    m_result = WriteError;
    return FALSE;
  }

  // done
  m_result = OK;
  return TRUE;
}

// TODO describe in brief about compression algorithm
// NOTE at the moment, the text is not really compressed anyway !
QByteArray PalmDoc::compress( QString text )
{
  QByteArray result;
  QCString ctext;
 
  ctext = text.latin1();
  result.resize( text.length() );
  for( unsigned i=0; i<text.length(); i++ )
    result[i]=ctext[i];

  return result;
}

#define INRANGE(v,p,q) ((v)>=(p))&&((v)<=(q))

// TODO describe in brief about decompression algorithm
QString PalmDoc::uncompress( QByteArray rec )
{
  QString result;

  for( unsigned i = 0; i < rec.size(); i++ )
  {
    unsigned char c = rec[i];

    if( INRANGE(c,1,8) )
    {
      // FIXME what to do ?
    }

    else if( INRANGE(c,0x09,0x7F) )
      result.append( c );

    else if( INRANGE(c,0xC0,0xFF) )
      result.append( 32 ).append( c^ 0x80 );

    else if( INRANGE(c,0x80,0xBF) )
    {
      unsigned char d = rec[++i];
      int back = (((c<<8)+d) & 0x3fff) >> 3;
      int count = (d & 7) + 3;
      if( result.length()-back >= 0 )
        for(; count>0; count-- )
          result.append( result[result.length()-back] );
    }

  }

  return result;
}
