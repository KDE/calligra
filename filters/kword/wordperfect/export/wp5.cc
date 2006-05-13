/* This file is part of the KDE project
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <config.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <QTextCodec>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
//Added by qt3to4:
#include <Q3CString>

#include <KWEFBaseWorker.h>
#include <KWEFKWordLeader.h>
#include <KWEFUtil.h>

#include <wp5.h>

bool WPFiveWorker::doOpenFile(const QString& filenameOut, const QString& /*to*/)
{
  filename = filenameOut;
  outfile.setName( filename );
  if( !outfile.open( QIODevice::WriteOnly ) )
    return false;

  output.setDevice( &outfile );
  output.setByteOrder (QDataStream::LittleEndian);
  return true;
}

bool WPFiveWorker::doCloseFile(void)
{
  // asssume we're at the end of the file
  quint32 total_filesize = outfile.at();

  // close the file first
  outfile.close();

  // reopen for read and write
  if( !outfile.open( QIODevice::ReadWrite ) )
    return false;
  output.setDevice( &outfile );
  output.setByteOrder (QDataStream::LittleEndian);

  // now it's time to fix-up some header fields

  // fix for offset 12, int32, filesize
  outfile.at( 20 );
  output << total_filesize;

  // offset 4, int32, pointer to document area
  outfile.at( 4 );
  output << document_area_ptr;

  outfile.close();
  return true;
}

bool WPFiveWorker::doOpenDocument(void)
{
  // write WP document header
  // note that some fields are still "dummy"

  // magic id: -1, "WPC"
  quint8 magic[] = { 0xff, 0x57, 0x50, 0x43 };
  for( int i=0; i<4; i++ ) output << magic[i];

  // pointer to document area (dummy, will be fixed later)
  quint8 docptr[] = { 0x0E, 0x02, 0x00, 0x00 } ;
  for( int i=0; i<4; i++ ) output << docptr[i];

  // write product type ( 1 = WordPerfect )
  quint8 product_type = 1;
  output << product_type;

  // write file type ( 10 = WordPerfect document )
  quint8 file_type = 10;
  output << file_type;

  // write version ( 1 = WordPerfect 5.x )
  quint16 version = 0x0100;
  output << version;

  // write encryption flag ( 0 = not encrypted )
  quint16 encrypt = 0;
  output << encrypt;

  // offset to index header (always 0x200?)
  quint16 index_header_ptr = 0x200;
  output << index_header_ptr;

  // beginning of extended file header (always 5)
  quint32 extheader = 5;
  output << extheader;

  // filesize (dummy, will be fixed later)
  quint32 filesize = 0;
  output << filesize;

  // filler 488 bytes
  quint8 dummy = 0;
  for( int i=0; i<488; i++ ) output << dummy;

  // index header (specified 0 index!)
  quint8 index_header[] = { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0 };
  for( int i=0; i<14; i++ ) output << index_header[i];

  // document area starts, mark it for the header fields
  document_area_ptr = output.device()->at();

  return true;
}

bool WPFiveWorker::doCloseDocument(void)
{
  return true;
}

// quick-and-dirty escape function for WP 5.x chars
// TODO fix it !
static Q3CString WPFiveEscape( const QString& text )
{
  Q3CString result;

  for( unsigned int i=0; i < text.length(); i++ )
  {
    int c = text[i].unicode();
    if( c < 32 ) result += '.';
    else if ( c == 32 ) result += 0x20 ; // hard space
    else if ( c < 128 ) result += text[i].latin1();
    else result += '.';
  }

  return result;
}


bool WPFiveWorker::doFullParagraph(const QString& paraText, 
  const LayoutData& /*layout*/, const ValueListFormatData& paraFormatDataList)
{
  ValueListFormatData::ConstIterator it;
  for( it = paraFormatDataList.begin(); it!=paraFormatDataList.end(); ++it )
  {
    const FormatData& formatData = *it;

    // only if the format is for text (id==1)
    if( formatData.id == 1 )
    {

       quint8 attr = 0; //invalid
       if( formatData.text.weight >= 75 ) attr = 12; // bold
       if( formatData.text.italic ) attr = 8;
       if( formatData.text.underline )
       {
         if( formatData.text.underlineValue == "double" )
           attr = 11; // double underline
         else
           attr = 14; // single underline
       }
       if( formatData.text.verticalAlignment == 1 ) attr = 6; //subscript
       if( formatData.text.verticalAlignment == 2 ) attr = 5; //superscript
       if( formatData.text.strikeout ) attr = 13;

       // due to the file format, before writing the text we must
       // write some prefix-code (such as Bold On) and possibly appropriate suffix-code (Bold Off)

       // attribute on
       if( attr > 0 ) output << (quint8)0xc3 << attr << (quint8)0xc3;

       // set font color
       QColor fgColor = formatData.text.fgColor;
       if( fgColor.isValid() )
       {
         quint8 wp_color[] = { 0xd1, 0, 10, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0xd1 };
         wp_color[7] = (quint8) fgColor.red();
         wp_color[8] = (quint8) fgColor.green();
         wp_color[9] = (quint8) fgColor.blue();
         output.writeRawBytes( (const char*)wp_color, 14 );
       }

       // the text itself, "escape" it first
       Q3CString out = WPFiveEscape( paraText.mid( formatData.pos, formatData.len ) );
       output.writeRawBytes( (const char*)out, out.length() );

       // attribute off
       if( attr > 0 ) output << (quint8)0xc4 << attr << (quint8)0xc4;
    }
  }

  // write hard-return 
  output << (quint8) 0x0a;

  return true;
}

