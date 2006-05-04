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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "palmdoc.h"

#include <q3cstring.h>
#include <qdatetime.h>
#include <q3ptrlist.h>
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
  int format = ((int)header[0]<<8) + (int)header[1];
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

  // assemble the records
  QByteArray rec;
  unsigned i = 0;
  for( unsigned r = 1; r < records.count(); r++ )
  {
     QByteArray *p = records.at( r );
     if( !p ) continue;
     rec.resize( rec.size() + p->size() );
     for( unsigned s=0; s<p->size(); s++ )
       rec[i++] = p->at( s );
  }

  // if the text is compressed, then uncompress
  if( format == 2 )
    setText( uncompress( rec ) );

  // if the text is not compressed, simply append as string
  if( format == 1 )
      setText( QString::fromLatin1( rec.data(),rec.count() ) );

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
  for( unsigned i=0; i<data.count(); )
  {
    QByteArray* ptr = new QByteArray;
    unsigned rs = data.count() - i;
    if( rs > recsize ) rs = recsize;
    ptr->resize( rs );
    for( unsigned m=0; m<rs; m++ )
      (*ptr)[m] = data[i++];
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
  header[8] = records.count()>> 8; // no of records
  header[9] = records.count() & 255;
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
QByteArray PalmDoc::compress( const QString& text )
{
  QByteArray result;
  unsigned textlen = text.length();
  const char *ctext =  text.latin1();
  unsigned int i, j;

  // we don't know the compressed size yet
  // therefore allocate buffer big enough
  result.resize( textlen );

  for( i=j=0; i<textlen;  )
  {
    int horizon = 2047;
    int start = (i < horizon) ? 0 : i-horizon;
    bool match = false;
    int match_pos=0, match_len=0;

    // look for match in the buffer
    for( int back = i-1; (!match) && (back > start); back-- )
      if( ctext[i] == ctext[back] )
      if( ctext[i+1] == ctext[back+1] )
      if( ctext[i+2] == ctext[back+2] )
      {
         match = true;
         match_pos = i-back;
         match_len = 3;

         if( i+3 < textlen )
           if( ctext[i+3] == ctext[back+3] )
           {
              match_len = 4;
              if( i+4 < textlen )
                if( ctext[i+4] == ctext[back+4] )
                {
                  match_len = 5;
                }
           }

      }

   if( match )
   {
     unsigned char p = 0x80 | ((match_pos >> 5)&0x3f);
     unsigned char q = ((match_pos & 0x1f) << 3) | (match_len-3);
     result[j++] = p;
     result[j++] = q;
     i+= match_len;
   }
   else
   {
     char ch = ctext[i++] & 0x7f;
     bool space_pack = false;

     if( ch == 0x20 )
       if ( i<textlen )
         if( ctext[i] >= 0x40 )
            space_pack = true;

     if( !space_pack ) result[j++] = ch;
     else result[j++] = ctext[i++] | 0x80;
   }

  }

  result.resize( j );

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
      i++;
      if( i < rec.size() )
         for( unsigned char v = rec[i]; c>0; c-- )
            result.append( v );
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
