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

#include "parser.h"

#include <q3memarray.h>
#include <QDataStream>
#include <QFile>
#include <q3ptrlist.h>
#include <QString>

using namespace WP;

static Token::Attr
mapToAttr (int data)
{
  switch (data)
    {
    case 0:
      return Token::ExtraLarge;
    case 1:
      return Token::VeryLarge;
    case 2:
      return Token::Large;
    case 3:
      return Token::Small;
    case 4:
      return Token::Fine;
    case 5:
      return Token::Superscript;
    case 6:
      return Token::Subscript;
    case 7:
      return Token::Outline;
    case 8:
      return Token::Italic;
    case 9:
      return Token::Shadow;
    case 10:
      return Token::Redline;
    case 11:
      return Token::DoubleUnderline;
    case 12:
      return Token::Bold;
    case 13:
      return Token::StrikedOut;
    case 14:
      return Token::Underline;
    case 15:
      return Token::SmallCaps;
    case 16:
      return Token::Blink;
    };
  return Token::None;
}

static Token::Align
mapToAlign (int data)
{
  switch (data)
    {
    case 0:
      return Token::Left;
    case 1:
      return Token::Full;
    case 2:
      return Token::Center;
    case 3:
      return Token::Right;
    case 4:
      return Token::All;
    };
  return Token::Left;
}

Parser::Parser ()
{
  tokens.setAutoDelete( TRUE );
  packets.setAutoDelete( TRUE );
}

bool
Parser::parse (const QString & filename)
{
  // initialize
  tokens.clear();
  packets.clear();
  docTitle = docAuthor = docAbstract = "";

  // open input file and assign the stream
  QDataStream stream;
  QFile in (filename);
  if (!in.open (QIODevice::ReadOnly))
    return FALSE;
  stream.setDevice (&in);

  // must be little-endian
  stream.setByteOrder (QDataStream::LittleEndian);

  // read 16-bytes document header
  quint8 header[16];
  for (int k = 0; k < 16; k++)
    stream >> header[k];

  // check first 4 bytes, must be (in hex): 0xFF, 0x57, 0x50, 0x43
  if ((header[0] != 0xFF) || (header[1] != 0x57) ||
      (header[2] != 0x50) || (header[3] != 0x43))
    return FALSE;

  // get document start
  unsigned m_docstart = header[4] + (header[5] << 8) +
    (header[6] << 16) + (header[7] << 24);
  if (m_docstart > stream.device ()->size ())
    return FALSE;

  // check document type
  unsigned product_type = header[8];
  unsigned file_type = header[9];
  if ((product_type != 1) || (file_type != 10))
    return FALSE;

  // check document format version: for WP 5.x or WP 6/7/8
  // major 0 means WP 5.x, otherwise WP 6/7/8
  unsigned major_version = header[10];
  unsigned minor_version = header[11];
  version = (major_version<<8) + minor_version;

  // do not accept other than 0 (WP 5.x) and 2 (WP 6/7/8)
  if ( (major_version != 0) && (major_version != 2) )
    return FALSE;

  // check if this is encrypted
  unsigned encrypt_hash = header[12] + (header[13] << 8);
  if ( encrypt_hash ) return FALSE;

  in.close ();

  // parse packets in prefix area
  if( major_version==0 ) parsePacketWP5( filename );
  else parsePacketWP6( filename );

  // parse document area
  if ( major_version == 0 ) parseDocWP5 ( filename, m_docstart );
  else parseDocWP6 ( filename, m_docstart );

  return TRUE;
}

void
Parser::parsePacketWP5( const QString & filename )
{
  // open input file and assign the stream
  QDataStream stream;
  QFile in (filename);
  if (!in.open (QIODevice::ReadOnly))
    return;
  stream.setDevice (&in);

  unsigned filesize = stream.device()->size();

  // must be little-endian
  stream.setByteOrder (QDataStream::LittleEndian);

  for( unsigned next_block=16; next_block>0; )
  {
    Q3MemArray<quint8> buf( 10 );
    stream.device()->at( next_block );
    for( int c=0; c<10; c++ )
      stream >> buf.at( c );

    unsigned type = buf[0] + (buf[1]<<8);
    unsigned count = buf[2]+ (buf[3]<<8);
    unsigned size = buf[4] + (buf[5]<<8);
    next_block = buf[6] + (buf[7] << 8) + (buf[8] << 16) + (buf[9] << 24);

    if( type != 0xfffb ) break;
    if( size != 10*count ) break;

    for( unsigned v=0; v<count; v++ )
    {
      quint16 packet_type;
      quint32 packet_size, packet_pos;
      stream >> packet_type;
      stream >> packet_size;
      stream >> packet_pos;

      if( packet_type == 0 ) break;
      if( packet_pos <= 0 ) continue;
      if( packet_size < 0 ) continue;
      if( packet_pos + packet_size > filesize ) continue;

      Packet* p = new Packet;
      p->type = packet_type;
      p->pos = packet_pos;
      p->size = packet_size;
      packets.append( p );
    }

  }

  // load all packets
  for( Q3PtrListIterator<Packet> it(packets); it; ++it )
  {
      Packet* p = it.current();
      stream.device()->at( p->pos );
      p->data.resize( p->size );
      for( unsigned q = 0; q < p->size; q++ )
        stream >> p->data.at(q );
  }

  in.close();

  // process all known packets
  for( Q3PtrListIterator<Packet> i(packets); i; ++i )
  {
    Packet* p = i.current();
    if( p->data.size()==0 ) continue;

    // document sumary
    if( p->type== 1 )
    {
      unsigned c;
      QString desc, desc_type, subject, author, typist, abstract;

      // handle difference between WP 5.0 and WP 5.1
      unsigned limit = (p->data[p->data.size()-1]==0xff) ? 94 : 57;

      for( c=26; c<limit; c++)
        desc.append( p->data[c] );
      if( limit==94 )
        for( c=94; (c<p->data.size())&&(p->data[c]); c++ )
          desc_type.append( p->data[c] );

      for( c++; (c<p->data.size())&&(p->data[c]); c++)
        subject.append( p->data[c] );
      for( c++; (c<p->data.size())&&(p->data[c]); c++)
        author.append( p->data[c] );
      for( c++; (c<p->data.size())&&(p->data[c]); c++)
        typist.append( p->data[c] );
      for( c++; (c<p->data.size())&&(p->data[c]); c++)
        abstract.append( p->data[c] );

      docTitle = desc.trimmed();
      docAuthor = author.trimmed();
      docAbstract = abstract.trimmed();
    }

  }
}

void
Parser::parsePacketWP6( const QString & filename )
{
  // open input file and assign the stream
  QDataStream stream;
  QFile in (filename);
  if (!in.open (QIODevice::ReadOnly))
    return;
  stream.setDevice (&in);

  unsigned filesize = stream.device()->size();

  // must be little-endian
  stream.setByteOrder (QDataStream::LittleEndian);

  quint16 flag, count;
  stream.device()->at( 0x200 );

  stream >> flag;  // FIXME should be checked == 2 ?
  stream >> count;

  stream.device()->at( 0x20e );
  for( unsigned c=0; c<count; c++ )
  {
    quint8 packet_type, packet_flag;
    quint16 count, hidcount;
    quint32 packet_size, packet_pos;

    stream >> packet_flag;
    stream >> packet_type;
    stream >> count;
    stream >> hidcount;
    stream >> packet_size;
    stream >> packet_pos;

    if( packet_pos + packet_size > filesize ) continue;

    Packet* p = new Packet;
    p->type = packet_type;
    p->pos = packet_pos;
    p->size = packet_size;
    packets.append( p );
  }

  // load all packets
  for( Q3PtrListIterator<Packet> it(packets); it; ++it )
  {
    Packet* p = it.current();
    stream.device()->at( p->pos );
    p->data.resize( p->size );
    for( unsigned q = 0; q < p->size; q++ )
      stream >> p->data.at(q );
  }

  in.close();

  // process all known packets
  for( Q3PtrListIterator<Packet> i(packets); i; ++i )
  {
    Packet* p = i.current();
    if( p->data.size()==0 ) continue;

    // extended document summary
    if( p->type == 18 )
    {
      for( unsigned j=0; j<p->data.size();)
      {
        unsigned size = p->data[j] + (p->data[j+1]<<8);
        unsigned tag = p->data[j+2] + (p->data[j+3]<<8);
        QString str;
        for( unsigned k=0; k<size-8; k++)
          if(!((j+8+k)&1))
            if( p->data[j+8+k]==0 ) break;
            else str.append( p->data[j+8+k] );

        str = str.trimmed();

        if( tag==1 ) docAbstract = str;
        if( tag==5 ) docAuthor = str;
        if( tag==17 ) docTitle = str;

        j+= size;
      }
    }

  }

}

void
Parser::parseDocWP5( const QString & filename, int start )
{
  // open input file and assign the stream
  QDataStream stream;
  QFile in (filename);
  if (!in.open (QIODevice::ReadOnly))
    return;
  stream.setDevice (&in);

  // sentinel
  if ( start < 0 ) return;
  if ( start >= stream.device ()->size () ) return;

  // must be little-endian
  stream.setByteOrder (QDataStream::LittleEndian);

  // seek to start of document area
  stream.device ()->at (start);

  // main loop
  QString text;
  while (!stream.atEnd ())
    {

      // read one byte
      quint8 code;
      stream >> code;

      // ASCII printable characters ?
      if ((code >= 33) && (code <= 127))
        text.append ((char) code);
      else
        {
          // either fixed-length or variable-length function

          Q3MemArray < quint8 > data;
          Q3MemArray < quint16 > pid;
          quint8 subfunction = 0;

          if ((code >= 0xC0) && (code <= 0xCF))
            {
              quint8 dummy;
              unsigned lentab[] =
                { 2, 7, 9, 1, 1, 3, 4, 5, 2, 3, 4, 4, 6, 8, 8, 10 };
              unsigned length = lentab[code & 0x0F];
              data.resize (length);
              for (unsigned c = 0; c < length; c++)
                stream >> data.at (c);
              stream >> dummy;  // FIXME should be checked == code ?
            }
          else if ((code >= 0xD0) && (code <= 0xFF))
            {
              quint16 length;
              stream >> subfunction;
              stream >> length;

              data.resize (length);
              for (unsigned c = 0; (c < length) && !stream.atEnd (); c++)
                stream >> data.at (c);
            }

          // NOTE: code < 32 is single-byte function

          // this is to simplify
          unsigned function = (code << 8) + subfunction;

          // flush previous text first
          if (!text.isEmpty ())
            {
              tokens.append (new Token (text));
              text = "";
            }

          switch (function)
            {

            case 0x2000:
            case 0x0d00:
            case 0x0b00:
              tokens.append (new Token (Token::SoftSpace));
              break;

            case 0xa900:
              tokens.append (new Token (Token::HardHyphen));
              break;

            case 0x0a00:
            case 0x9900:
            case 0x8c00:
              tokens.append (new Token (Token::HardReturn));
              break;

            case 0xc000:
              tokens.append (new Token (Token::ExtChar, data[1], data[0]));
              break;

            case 0xc300:
              tokens.append (new Token (Token::AttrOn, mapToAttr (data[0])));
              break;

            case 0xc400:
              tokens.append (new Token (Token::AttrOff, mapToAttr (data[0])));
              break;

            case 0xd001:
              tokens.append (new Token (Token::LeftMargin,
                                        data[4] + (data[5] << 8)));
              tokens.append (new Token (Token::RightMargin,
                                        data[6] + (data[7] << 8)));
              break;

            case 0xd005:
              tokens.append (new Token (Token::TopMargin,
                                        data[4] + (data[5] << 8)));
              tokens.append (new Token (Token::BottomMargin,
                                        data[6] + (data[7] << 8)));
              break;

              // NOTE we store linespace as 1/65536th, i.e 655536 means single space
              // on WP 5.x, (data[3]<<8)+data[2] is in 1/256th, so make 8-bit adjustment
            case 0xd002:
              tokens.append (new Token (Token::Linespace,
                                        (data[2] << 8) + (data[3] << 16)));
              break;

            case 0xd006:
              tokens.append (new Token (Token::Justification,
                                        mapToAlign (data[1])));
              break;

            case 0xd100:
              tokens.append (new Token (Token::FontColor,
                                        data[3], data[4], data[5]));
              break;

            case 0xd101:
              tokens.append (new Token (Token::FontSize,
                                        (data[29] << 8) + data[28]));
              break;

            case 0x8300:
              tokens.append (new Token (Token::SoftReturn));
              // FIXME this is actually Soft End of Center/Align
              break;

            case 0xc500:
              // TODO block protect on/off
              break;

            case 0xd311:
              // TODO set language [Lang]
              break;

            case 0xd301:
              // TODO set underline mode
              break;

            case 0xd700:
              tokens.append (new Token (Token::MarkTocStart));
              break;

            case 0xd701:
              tokens.append (new Token (Token::MarkTocEnd));
              break;

            case 0xdb00:
              // TODO begin style on
              break;

            case 0xdb01:
              // TODO end style on
              break;

            case 0xdb02:
              // TODO global on
              break;

            case 0xdb03:
              // TODO style off
              break;

            case 0xdc00:
              tokens.append (new Token (Token::TableCell));
              break;

            case 0xdc01:
              tokens.append (new Token (Token::TableRow));
              break;

            case 0xdc02:
              tokens.append (new Token (Token::TableOff));
              break;

            default:
              tokens.append (new Token (Token::Function, function));
            }

        }
    }

  // flush left-over text if any
  if (!text.isEmpty ())
    {
      tokens.append (new Token (text));
      text = "";
    }
}

void
Parser::parseDocWP6 (const QString & filename, int start)
{
  // open input file and assign the stream
  QDataStream stream;
  QFile in (filename);
  if (!in.open (QIODevice::ReadOnly))
    return;
  stream.setDevice (&in);

  // sentinel
  if ( start < 0 ) return;
  if ( start >= stream.device ()->size () ) return;

  // must be little-endian
  stream.setByteOrder (QDataStream::LittleEndian);

  // seek to start of document area
  stream.device ()->at (start);

  // main loop
  QString text;
  while (!stream.atEnd ())
    {

      // read one byte
      quint8 code;
      stream >> code;

      // ASCII printable characters ?
      if ((code >= 33) && (code <= 127))
        text.append ((char) code);

      // WP default extended international characters ?
      else if ((code >= 1) && (code <= 32))
        {
          // flush previous text first
          if (!text.isEmpty ())
            {
              tokens.append (new Token (text));
              text = "";
            }

          int xlate[] = {
            35, 34, 7, 36, 31, 30, 27, 33, 29, 77, 76, 39, 38, 45, 41, 40,
            47, 43, 49, 57, 56, 81, 80, 83, 82, 63, 62, 71, 70, 67, 73, 23
          };
          tokens.append (new Token (Token::ExtChar, 1, xlate[code - 1]));
        }


      else
        {
          // either fixed-length or variable-length function

          Q3MemArray < quint8 > data;
          Q3MemArray < quint16 > pid;
          quint8 subfunction = 0;

          if ((code >= 0xF0) && (code <= 0xFF))
            {
              quint8 dummy;
              unsigned lentab[] =
                { 2, 3, 1, 1, 1, 1, 2, 2, 2, 3, 3, 4, 4, 6, 6, 0 };
              unsigned length = lentab[code & 0x0F];
              data.resize (length);
              for (unsigned c = 0; c < length; c++)
                stream >> data.at (c);
              stream >> dummy;  // FIXME should be checked == code ?
            }
          else if ((code >= 0xD0) && (code <= 0xFF))
            {
              quint16 length;
              quint8 flag = 0, numpid = 0;
              stream >> subfunction;
              stream >> length;
              stream >> flag;
              length -= 10;

              if (flag & 0x80)
                {
                  stream >> numpid;
                  pid.resize (numpid);
                  for (unsigned n = 0; n < numpid; n++)
                    stream >> pid.at (n);
                  length = length - 1 - numpid * 2;
                }

              quint16 nondel;
              stream >> nondel;

              data.resize (length);
              for (int c = 0; length && !stream.atEnd (); length--, c++)
                stream >> data[c];

              quint16 dummy_length;
              quint8 dummy_code;
              stream >> dummy_length;   // FIXME should be checked == length ?
              stream >> dummy_code;     // FIXME should be checked == code ?

            }

          // this is to simplify
          unsigned function = (code << 8) + subfunction;

          // flush previous text first
          if (!text.isEmpty ())
            {
              tokens.append (new Token (text));
              text = "";
            }

          int val;

          switch (function)
            {

            case 0x8000:
              tokens.append (new Token (Token::SoftSpace));
              break;

            case 0x8100:
              tokens.append (new Token (Token::HardSpace));
              break;

            case 0x8400:
              tokens.append (new Token (Token::HardHyphen));
              break;

            case 0x8700:
              tokens.append (new Token (Token::DormantHardReturn));
              break;

            case 0x8800:
              tokens.append (new Token (Token::SoftReturn));
              // FIXME this is actually Soft End of Center/Align
              break;

            case 0xcc00:
              tokens.append (new Token (Token::HardReturn));
              break;

            case 0xd001:
            case 0xd014:
              tokens.append (new Token (Token::SoftReturn));
              break;

            case 0xd004:
              tokens.append (new Token (Token::HardReturn));
              break;

            case 0xd00a:
              tokens.append (new Token (Token::TableCell));
              break;

            case 0xd00b:
            case 0xd00c:
            case 0xd00d:
            case 0xd00e:
            case 0xd00f:
            case 0xd010:
              tokens.append (new Token (Token::TableRow));
              break;

            case 0xd011:
            case 0xd012:
            case 0xd013:
              tokens.append (new Token (Token::TableOff));
              break;

            case 0xd100:
              tokens.append (new Token (Token::TopMargin,
                                        data[0] + (data[1] << 8)));
              break;

            case 0xd101:
              tokens.append (new Token (Token::BottomMargin,
                                        data[0] + (data[1] << 8)));
              break;

            case 0xd200:
              tokens.append (new Token (Token::LeftMargin,
                                        data[0] + (data[1] << 8)));
              break;

            case 0xd201:
              tokens.append (new Token (Token::RightMargin,
                                        data[0] + (data[1] << 8)));
              break;

            case 0xd301:
              tokens.append (new Token (Token::Linespace,
                                        (data[2] << 16) + (data[1] << 8) +
                                        data[0]));
              break;

            case 0xd304:
              handleTab (data);
              break;

            case 0xd305:
              tokens.append (new Token (Token::Justification,
                                        mapToAlign (data[0])));
              break;

            case 0xd30a:
              // TODO spacing after paragraph
              break;

            case 0xd30b:
              tokens.append (new Token (Token::ParagraphIndent,
                                        data[0] + (data[1] << 8)));
              break;

            case 0xd30c:
              val = data[0] + (data[1] << 8);
              if (val > 32767)
                val = val - 65536;
              tokens.append (new Token (Token::LeftMarginAdjust, val));
              break;

            case 0xd30d:
              val = data[0] + (data[1] << 8);
              if (val > 32767)
                val = val - 65536;
              tokens.append (new Token (Token::RightMarginAdjust, val));
              break;

            case 0xd402:
            case 0xd403:
              // TODO underline spaces ? tabs ?
              break;

            case 0xd40a:
              tokens.append (new Token (Token::MarkTocStart));
              break;

            case 0xd40b:
              tokens.append (new Token (Token::MarkTocEnd));
              break;

            case 0xd418:
              tokens.append (new Token (Token::FontColor,
                                        data[0], data[1], data[2]));
              break;

            case 0xd41b:
              tokens.append (new Token (Token::FontSize,
                                        (data[1] << 8) + data[0]));
              break;

            case 0xd41c:
              // TODO set language [Lang]
              break;

            case 0xd41a:
              {
                unsigned fontpid = pid[0];
                if( fontpid == 0 || fontpid > packets.count() ) continue;
                Packet* p = packets.at( fontpid-1 );
                if( p->type==85 )
                {
                  // read the typeface (stored as WP word-string)
                  QString typeface;
                  unsigned strlen = p->data[22];
                  for( unsigned i=24; strlen && (i<p->data.size()); i+=2, strlen-=2 )
                    if( p->data[i]) typeface.append( p->data[i] );
                    else break;
                  typeface = typeface.trimmed();

                  // hack: get rid of "Regular" as font name suffix
                  QString suffix = "Regular";
                  if( typeface.right( suffix.length() ) == suffix )
                     typeface = typeface.left( typeface.length() -  suffix.length() ).trimmed();

                  tokens.append( new Token( Token::FontFace, typeface ) );
                }
              }
              break;

            case 0xd426:
            case 0xd427:
              // TODO block protect on/off
              break;

            case 0xd42a:
              tokens.append (new Token (Token::TableOn));
              break;

            case 0xd42b:
              tokens.append (new Token (Token::TableEnd));
              break;

            case 0xd42c:
              tokens.append (new Token (Token::TableColumn));
              break;

            case 0xd45f:
              // FIXME check what it does
              // this always appears within table cell
              break;

            case 0xdd0a:
            case 0xdd0b:
              // TODO global on/off
              break;

            case 0xf000:
              tokens.append (new Token (Token::ExtChar, data[1], data[0]));
              break;

            case 0xf200:
              tokens.append (new Token (Token::AttrOn, mapToAttr (data[0])));
              break;

            case 0xf300:
              tokens.append (new Token (Token::AttrOff, mapToAttr (data[0])));
              break;

            case 0xdd00:
            case 0xdd01:
              // TODO style begin on/off
              break;

            case 0xdd02:
            case 0xdd03:
              // TODO style end on/off
              break;

            case 0xe011:
              // TODO soft left-tab
              break;

            case 0xe040:
              // TODO center on margin
              break;

            case 0xe080:
              tokens.append (new Token (Token::TabHardFlushRight));
              break;

            case 0xd111:
              // FIXME unknown functions
              break;

            case 0xfb00:
              // text highlight (or background color)
              tokens.append (new Token (Token::HighlightOn,
                                        data[0], data[1], data[2]));
              break;

            case 0xfc00:
              // turn off highlight, data is last highlight color
              tokens.append (new Token (Token::HighlightOff,
                                        data[0], data[1], data[2]));
              break;

            default:
              tokens.append (new Token (Token::Function, function));
              break;

            };


        }

    }

  // flush left-over text if any
  if (!text.isEmpty ())
    {
      tokens.append (new Token (text));
      text = "";
    }
}


static Token::TabType
mapToTabType (int t)
{
  switch (t)
    {
    case 0:
      return Token::LeftTab;
    case 1:
      return Token::CenterTab;
    case 2:
      return Token::RightTab;
    case 3:
      return Token::DecimalTab;
    case 4:
      return Token::VerticalTab;
    }
  return Token::LeftTab;
}

void
Parser::handleTab (Q3MemArray < quint8 > data)
{
  Q3PtrList < Token::Tab > tabs;
  bool relative = data[0];
  int adjust = data[1] + (data[2] << 8);
  int num = data[3];
  int p = 4;
  int tabtype = 0;
  int tabpos = 0;

  for (int i = 0; i < num; i++)
    {
      int tt = data[p];

      if (tt & 0x80)
        {
          int rep = tt & 0x7F;
          p++;
          for (int c = 0; c < rep; c++)
            {
              int diff = data[p] + (data[p + 1] << 8);
              tabs.
                append (new Token::
                        Tab (mapToTabType (tabtype), tabpos + diff));
              p += 2;
            }
        }
      else
        {
          tabtype = tt & 0x7f;
          tabpos = data[p + 1] + (data[p + 2] << 8);
          tabs.append (new Token::Tab (mapToTabType (tabtype), tabpos));
          p += 3;
        }

    }


  if (tabs.count ())
    tokens.append (new Token (tabs));

}

// the following tables maps WP charset/charcode to unicode character

// WP multinational characters (charset 1)
static unsigned multinational_map[] = {
  0x0300, 0x00b7, 0x0303, 0x0302, 0x0335, 0x0338, 0x0301, 0x0308,
  0x0304, 0x0313, 0x0315, 0x02bc, 0x0326, 0x0315, 0x030a, 0x0307,
  0x030b, 0x0327, 0x0328, 0x030c, 0x0337, 0x0305, 0x0306, 0x00df,
  0x0138, 0xf801, 0x00c1, 0x00e1, 0x00c2, 0x00e2, 0x00c4, 0x00e4,
  0x00c0, 0x00e0, 0x00c5, 0x00e5, 0x00c6, 0x00e6, 0x00c7, 0x00e7,
  0x00c9, 0x00e9, 0x00ca, 0x00ea, 0x00cb, 0x00eb, 0x00c8, 0x00e8,
  0x00cd, 0x00ed, 0x00ce, 0x00ee, 0x00cf, 0x00ef, 0x00cc, 0x00ec,
  0x00d1, 0x00f1, 0x00d3, 0x00f3, 0x00d4, 0x00f4, 0x00d6, 0x00f6,
  0x00d2, 0x00f2, 0x00da, 0x00fa, 0x00db, 0x00fb, 0x00dc, 0x00fc,
  0x00d9, 0x00f9, 0x0178, 0x00ff, 0x00c3, 0x00e3, 0x0110, 0x0111,
  0x00d8, 0x00f8, 0x00d5, 0x00f5, 0x00dd, 0x00fd, 0x00d0, 0x00f0,
  0x00de, 0x00fe, 0x0102, 0x0103, 0x0100, 0x0101, 0x0104, 0x0105,
  0x0106, 0x0107, 0x010c, 0x010d, 0x0108, 0x0109, 0x010a, 0x010b,
  0x010e, 0x010f, 0x011a, 0x011b, 0x0116, 0x0117, 0x0112, 0x0113,
  0x0118, 0x0119, 0x0047, 0x0067, 0x011e, 0x011f, 0x0047, 0x0067,
  0x0122, 0x0123, 0x011c, 0x011d, 0x0120, 0x0121, 0x0124, 0x0125,
  0x0126, 0x0127, 0x0130, 0x0069, 0x012a, 0x012b, 0x012e, 0x012f,
  0x0128, 0x0129, 0x0132, 0x0133, 0x0134, 0x0135, 0x0136, 0x0137,
  0x0139, 0x013a, 0x013d, 0x013e, 0x013b, 0x013c, 0x013f, 0x0140,
  0x0141, 0x0142, 0x0143, 0x0144, 0xf802, 0x0149, 0x0147, 0x0148,
  0x0145, 0x0146, 0x0150, 0x0151, 0x014c, 0x014d, 0x0152, 0x0153,
  0x0154, 0x0155, 0x0158, 0x0159, 0x0156, 0x0157, 0x015a, 0x015b,
  0x0160, 0x0161, 0x015e, 0x015f, 0x015c, 0x015d, 0x0164, 0x0165,
  0x0162, 0x0163, 0x0166, 0x0167, 0x016c, 0x016d, 0x0170, 0x0171,
  0x016a, 0x016b, 0x0172, 0x0173, 0x016e, 0x016f, 0x0168, 0x0169,
  0x0174, 0x0175, 0x0176, 0x0177, 0x0179, 0x017a, 0x017d, 0x017e,
  0x017b, 0x017c, 0x014a, 0x014b, 0xf000, 0xf001, 0xf002, 0xf003,
  0xf004, 0xf005, 0xf006, 0xf007, 0xf008, 0xf009, 0xf00a, 0xf00b,
  0xf00c, 0xf00d, 0xf00e, 0xf00f, 0x010e, 0x010f, 0x01a0, 0x01a1,
  0x01af, 0x01b0, 0x0114, 0x0115, 0x012c, 0x012d, 0x0049, 0x0131,
  0x014e, 0x014f
};

// WP phonetic symbol (charset 2)
static unsigned phonetic_map[] = {
  0x02b9, 0x02ba, 0x02bb, 0xf813, 0x02bd, 0x02bc, 0xf814, 0x02be,
  0x02bf, 0x0310, 0x02d0, 0x02d1, 0x0306, 0x032e, 0x0329, 0x02c8,
  0x02cc, 0x02c9, 0x02ca, 0x02cb, 0x02cd, 0x02ce, 0x02cf, 0x02c6,
  0x02c7, 0x02dc, 0x0325, 0x02da, 0x032d, 0x032c, 0x0323, 0x0308,
  0x0324, 0x031c, 0x031d, 0x031e, 0x031f, 0x0320, 0x0321, 0x0322,
  0x032a, 0x032b, 0x02d2, 0x02d3, 0xf815, 0xf816, 0x005f, 0x2017,
  0x033e, 0x02db, 0x0327, 0x0233, 0x030d, 0x02b0, 0x02b6, 0x0250,
  0x0251, 0x0252, 0x0253, 0x0299, 0x0254, 0x0255, 0x0297, 0x0256,
  0x0257, 0x0258, 0x0259, 0x025a, 0x025b, 0x025c, 0x025d, 0x029a,
  0x025e, 0x025f, 0x0278, 0x0261, 0x0260, 0x0262, 0x029b, 0x0263,
  0x0264, 0x0265, 0x0266, 0x0267, 0x029c, 0x0268, 0x026a, 0x0269,
  0x029d, 0x029e, 0x026b, 0x026c, 0x026d, 0x029f, 0x026e, 0x028e,
  0x026f, 0x0270, 0x0271, 0x0272, 0x0273, 0x0274, 0x0276, 0x0277,
  0x02a0, 0x0279, 0x027a, 0x027b, 0x027c, 0x027d, 0x027e, 0x027f,
  0x0280, 0x0281, 0x0282, 0x0283, 0x0284, 0x0285, 0x0286, 0x0287,
  0x0288, 0x0275, 0x0289, 0x028a, 0x028c, 0x028b, 0x028d, 0x03c7,
  0x028f, 0x0290, 0x0291, 0x0292, 0x0293, 0x0294, 0x0295, 0x0296,
  0x02a1, 0x02a2, 0x0298, 0x02a3, 0x02a4, 0x02a5, 0x02a6, 0x02a7,
  0x02a8
};

// WP typographic symbol (charset 4)
static unsigned typographic_map[] = {
  0x25cf, 0x25cb, 0x25a0, 0x2022, 0xf817, 0x00b6, 0x00a7, 0x00a1,
  0x00bf, 0x00ab, 0x00bb, 0x00a3, 0x00a5, 0x20a7, 0x0192, 0x00aa,
  0x00ba, 0x00bd, 0x00bc, 0x00a2, 0x00b2, 0x207f, 0x00ae, 0x00a9,
  0x00a4, 0x00be, 0x00b3, 0x201b, 0x2019, 0x2018, 0x201f, 0x201d,
  0x201c, 0x2013, 0x2014, 0x2039, 0x203a, 0x25cb, 0x25a1, 0x2020,
  0x2021, 0x2122, 0x2120, 0x211e, 0x25cf, 0x25e6, 0x25a0, 0x25aa,
  0x25a1, 0x25ab, 0x2012, 0xfb00, 0xfb03, 0xfb04, 0xfb01, 0xfb02,
  0x2026, 0x0024, 0x20a3, 0x20a2, 0x20a0, 0x20a4, 0x201a, 0x201e,
  0x2153, 0x2154, 0x215b, 0x215c, 0x215d, 0x215e, 0x24c2, 0x24c5,
  0x20ac, 0x2105, 0x2106, 0x2030, 0x2116, 0xf818, 0x00b9, 0x2409,
  0x240c, 0x240d, 0x240a, 0x2424, 0x240b, 0xf819, 0x20a9, 0x20a6,
  0x20a8, 0xf81a, 0xf81b, 0xf81c, 0xf81d, 0xf81e, 0xf81f, 0xf820,
  0xf821, 0xf822, 0xf823, 0xf824, 0xf825, 0xf826
};

// WP iconic symbol (charset 5)
static unsigned iconic_map[] = {
  0x2661, 0x2662, 0x2667, 0x2664, 0x2642, 0x2640, 0x263c, 0x263a,
  0x263b, 0x266a, 0x266c, 0x25ac, 0x2302, 0x203c, 0x221a, 0x21a8,
  0x2310, 0x2319, 0x25d8, 0x25d9, 0x21b5, 0x2104, 0x261c, 0x2007,
  0x2610, 0x2612, 0x2639, 0x266f, 0x266d, 0x266e, 0x260e, 0x231a,
  0x231b, 0x2701, 0x2702, 0x2703, 0x2704, 0x260e, 0x2706, 0x2707,
  0x2708, 0x2709, 0x261b, 0x261e, 0x270c, 0x270d, 0x270e, 0x270f,
  0x2710, 0x2711, 0x2712, 0x2713, 0x2714, 0x2715, 0x2716, 0x2717,
  0x2718, 0x2719, 0x271a, 0x271b, 0x271c, 0x271d, 0x271e, 0x271f,
  0x2720, 0x2721, 0x2722, 0x2723, 0x2724, 0x2725, 0x2726, 0x2727,
  0x2605, 0x2606, 0x272a, 0x272b, 0x272c, 0x272d, 0x272e, 0x272f,
  0x2730, 0x2731, 0x2732, 0x2733, 0x2734, 0x2735, 0x2736, 0x2737,
  0x2738, 0x2739, 0x273a, 0x273b, 0x273c, 0x273d, 0x273e, 0x273f,
  0x2740, 0x2741, 0x2742, 0x2743, 0x2744, 0x2745, 0x2746, 0x2747,
  0x2748, 0x2749, 0x274a, 0x274b, 0x25cf, 0x274d, 0x25a0, 0x274f,
  0x2750, 0x2751, 0x2752, 0x25b2, 0x25bc, 0x25c6, 0x2756, 0x25d7,
  0x2758, 0x2759, 0x275a, 0x275b, 0x275c, 0x275d, 0x275e, 0x2036,
  0x2033, 0xf827, 0xf828, 0xf829, 0xf82a, 0x2329, 0x232a, 0x005b,
  0x005d, 0xf82b, 0xf82c, 0xf82d, 0xf82e, 0xf82f, 0xf830, 0xf831,
  0x2190, 0xf832, 0xf833, 0xf834, 0xf835, 0xf836, 0x21e8, 0x21e6,
  0x2794, 0xf838, 0xf839, 0xf83a, 0xf83b, 0xf83c, 0x25d6, 0xf83d,
  0xf83e, 0x2761, 0x2762, 0x2763, 0x2764, 0x2765, 0x2766, 0x2767,
  0x2663, 0x2666, 0x2665, 0x2660, 0x2780, 0x2781, 0x2782, 0x2783,
  0x2784, 0x2785, 0x2786, 0x2787, 0x2788, 0x2789, 0x2776, 0x2777,
  0x2778, 0x2779, 0x277a, 0x277b, 0x277c, 0x277d, 0x277e, 0x277f,
  0x2780, 0x2781, 0x2782, 0x2783, 0x2784, 0x2785, 0x2786, 0x2787,
  0x2788, 0x2789, 0x278a, 0x278b, 0x278c, 0x278d, 0x278e, 0x278f,
  0x2790, 0x2791, 0x2792, 0x2793, 0x2794, 0x2192, 0x2194, 0x2195,
  0x2798, 0x2799, 0x279a, 0x279b, 0x279c, 0x279d, 0x279e, 0x279f,
  0x27a0, 0x27a1, 0x27a2, 0x27a3, 0x27a4, 0x27a5, 0x27a6, 0x27a7,
  0x27a8, 0x27a9, 0x27aa, 0x27ab, 0x27ac, 0x27ad, 0x27ae, 0x27af,
  0xf83f, 0x27b1, 0x27b2, 0x27b3, 0x27b4, 0x27b5, 0x27b6, 0x27b7,
  0x27b8, 0x27b9, 0x27ba, 0x27bb, 0x27bc, 0x27bd, 0x27be
};

// WP math/scientific (charset 6)
static unsigned math_map[] = {
  0x2212, 0x00b1, 0x2264, 0x2265, 0x221d, 0x01c0, 0x2215, 0x2216,
  0x00f7, 0x2223, 0x2329, 0x232a, 0x223c, 0x2248, 0x2261, 0x2208,
  0x2229, 0x2225, 0x2211, 0x221e, 0x00ac, 0x2192, 0x2190, 0x2191,
  0x2193, 0x2194, 0x2195, 0x25b8, 0x25c2, 0x25b4, 0x25be, 0x22c5,
  0xf850, 0x2218, 0x2219, 0x212b, 0x00b0, 0x00b5, 0x203e, 0x00d7,
  0x222b, 0x220f, 0x2213, 0x2207, 0x2202, 0x02b9, 0x02ba, 0x2192,
  0x212f, 0x2113, 0x210f, 0x2111, 0x211c, 0x2118, 0x21c4, 0x21c6,
  0x21d2, 0x21d0, 0x21d1, 0x21d3, 0x21d4, 0x21d5, 0x2197, 0x2198,
  0x2196, 0x2199, 0x222a, 0x2282, 0x2283, 0x2286, 0x2287, 0x220d,
  0x2205, 0x2308, 0x2309, 0x230a, 0x230b, 0x226a, 0x226b, 0x2220,
  0x2297, 0x2295, 0x2296, 0xf851, 0x2299, 0x2227, 0x2228, 0x22bb,
  0x22a4, 0x22a5, 0x2312, 0x22a2, 0x22a3, 0x25a1, 0x25a0, 0x25ca,
  0xf852, 0xf853, 0xf854, 0x2260, 0x2262, 0x2235, 0x2234, 0x2237,
  0x222e, 0x2112, 0x212d, 0x2128, 0x2118, 0x20dd, 0xf855, 0x25c7,
  0x22c6, 0x2034, 0x2210, 0x2243, 0x2245, 0x227a, 0x227c, 0x227b,
  0x227d, 0x2203, 0x2200, 0x22d8, 0x22d9, 0x228e, 0x228a, 0x228b,
  0x2293, 0x2294, 0x228f, 0x2291, 0x22e4, 0x2290, 0x2292, 0x22e5,
  0x25b3, 0x25bd, 0x25c3, 0x25b9, 0x22c8, 0x2323, 0x2322, 0xf856,
  0x219d, 0x21a9, 0x21aa, 0x21a3, 0x21bc, 0x21bd, 0x21c0, 0x21c1,
  0x21cc, 0x21cb, 0x21bf, 0x21be, 0x21c3, 0x21c2, 0x21c9, 0x21c7,
  0x22d3, 0x22d2, 0x22d0, 0x22d1, 0x229a, 0x229b, 0x229d, 0x2127,
  0x2221, 0x2222, 0x25c3, 0x25b9, 0x25b5, 0x25bf, 0x2214, 0x2250,
  0x2252, 0x2253, 0x224e, 0x224d, 0x22a8, 0xf857, 0x226c, 0x0285,
  0x2605, 0x226e, 0x2270, 0x226f, 0x2271, 0x2241, 0x2244, 0x2247,
  0x2249, 0x2280, 0x22e0, 0x2281, 0x22e1, 0x2284, 0x2285, 0x2288,
  0x2289, 0xf858, 0xf859, 0x22e2, 0x22e3, 0x2226, 0x2224, 0x226d,
  0x2204, 0x2209, 0xf85a, 0x2130, 0x2131, 0x2102, 0xf85b, 0x2115,
  0x211d, 0x225f, 0x221f, 0x220b, 0x22ef, 0xf85c, 0x22ee, 0x22f1,
  0xf85d, 0x20e1, 0x002b, 0x002d, 0x003d, 0x002a, 0xf85e, 0xf85f,
  0xf860, 0x210c, 0x2118, 0x2272, 0x2273, 0xf861
};

// WP math/scientific extended (charset 6)
static unsigned mathext_map[] = {
  0x2320, 0x2321, 0xf702, 0xf703, 0x221a, 0xf705, 0xf706, 0xf707,
  0xf708, 0xf709, 0xf70a, 0xf70b, 0xf70c, 0xf70d, 0xf70e, 0xf70f,
  0xf710, 0xf711, 0xf712, 0xf713, 0xf714, 0xf715, 0xf716, 0xf717,
  0xf718, 0xf719, 0xf71a, 0xf71b, 0xf71c, 0xf71d, 0xf71e, 0xf71f,
  0xf720, 0xf721, 0xf722, 0xf723, 0xf724, 0xf725, 0xf726, 0xf727,
  0xf728, 0xf729, 0xf72a, 0xf72b, 0xf72c, 0xf72d, 0xf72e, 0xf72f,
  0xf730, 0xf731, 0xf732, 0xf733, 0xf734, 0xf735, 0xf736, 0xf737,
  0xf738, 0xf739, 0xf73a, 0xf73b, 0xf73c, 0xf73d, 0xf73e, 0xf73f,
  0xf740, 0xf741, 0xf742, 0xf743, 0xf744, 0xf745, 0xf746, 0xf747,
  0xf748, 0xf749, 0xf74a, 0xf74b, 0xf74c, 0xf74d, 0xf74e, 0xf74f,
  0xf750, 0xf751, 0xf752, 0xf753, 0xf754, 0xf755, 0xf756, 0xf757,
  0xf758, 0xf759, 0xf75a, 0xf75b, 0xf75c, 0xf75d, 0xf75e, 0xf75f,
  0xf760, 0xf761, 0xf762, 0xf763, 0xf764, 0xf765, 0xf766, 0xf767,
  0xf768, 0xf769, 0xf76a, 0xf76b, 0xf76c, 0xf76d, 0xf76e, 0xf76f,
  0xf770, 0xf771, 0xf772, 0xf773, 0xf774, 0xf775, 0xf776, 0xf777,
  0xf778, 0xf779, 0x20aa, 0xf77b, 0xf77c, 0xf77d, 0xf77e, 0xf77f,
  0xf780, 0xf781, 0xf782, 0xf783, 0xf784, 0xf785, 0xf786, 0xf787,
  0xf788, 0xf789, 0xf78a, 0xf78b, 0xf78c, 0xf78d, 0xf78e, 0xf78f,
  0xf790, 0xf791, 0xf792, 0xf793, 0xf794, 0xf795, 0xf796, 0xf797,
  0xf798, 0xf799, 0xf79a, 0xf79b, 0xf79c, 0xf79d, 0xf79e, 0xf79f,
  0xf7a0, 0xf7a1, 0xf7a2, 0xf7a3, 0xf7a4, 0xf7a5, 0xf7a6, 0xf7a7,
  0xf7a8, 0xf7a9, 0xf7aa, 0xf7ab, 0xf7ac, 0xf7ad, 0xf7ae, 0xf7af,
  0xf7b0, 0xf7b1, 0xf7b2, 0xf7b3, 0xf7b4, 0xf7b5, 0xf7b6, 0xf7b7,
  0xf7b8, 0xf7b9, 0xf7ba, 0xf7bb, 0xf7bc, 0xf7bd, 0xf7be, 0xf7bf,
  0xf7c0, 0xf7c1, 0xf7c2, 0xf7c3, 0xf7c4, 0xf7c5, 0xf7c6, 0xf7c7
};

// WP greek (charset 8)
static unsigned int greek_map[] = {
  0x0391, 0x03b1, 0x0392, 0x03b2, 0x0392, 0x03d0, 0x0393, 0x03b3,
  0x0394, 0x03b4, 0x0395, 0x03b5, 0x0396, 0x03b6, 0x0397, 0x03b7,
  0x0398, 0x03b8, 0x0399, 0x03b9, 0x039a, 0x03ba, 0x039b, 0x03bb,
  0x039c, 0x03bc, 0x039d, 0x03bd, 0x039e, 0x03be, 0x039f, 0x03bf,
  0x03a0, 0x03c0, 0x03a1, 0x03c1, 0x03a3, 0x03c3, 0x03a3, 0x03c2,
  0x03a4, 0x03c4, 0x03a5, 0x03c5, 0x03a6, 0x03c6, 0x03a7, 0x03c7,
  0x03a8, 0x03c8, 0x03a9, 0x03c9, 0x0386, 0x03ac, 0x0388, 0x03ad,
  0x0389, 0x03ae, 0x038a, 0x03af, 0x03aa, 0x03ca, 0x038c, 0x03cc,
  0x038e, 0x03cd, 0x03ab, 0x03cb, 0x038f, 0x03ce, 0x03b5, 0x03d1,
  0x03f0, 0x03d6, 0x03f1, 0x03db, 0x03d2, 0x03d5, 0x03d6, 0x03d7,
  0x00b7, 0x0374, 0x0375, 0x0301, 0x0308, 0xf216, 0xf217, 0x0300,
  0x0311, 0x0313, 0x0314, 0x0345, 0x1fce, 0x1fde, 0x1fcd, 0x1fdd,
  0xf200, 0xf201, 0xf022, 0xf021, 0xf202, 0xf203, 0xf204, 0xf300,
  0xf301, 0xf302, 0xf303, 0xf304, 0xf305, 0x1f70, 0xf100, 0x1fb3,
  0x1fb4, 0x1fb2, 0xf205, 0x1f00, 0x1f04, 0x1f02, 0xf206, 0x1f80,
  0x1f84, 0x1f82, 0xf306, 0x1f01, 0x1f05, 0x1f03, 0xf207, 0x1f81,
  0x1f85, 0x1f83, 0xf307, 0x1f72, 0x1f10, 0x1f14, 0x1f12, 0x1f11,
  0x1f15, 0x1f13, 0x1f74, 0xf101, 0x1fc3, 0x1fc4, 0x1fc2, 0xf208,
  0x1f20, 0x1f24, 0x1f22, 0xf209, 0x1f90, 0x1f94, 0x1f92, 0xf308,
  0x1f21, 0x1f25, 0x1f23, 0xf20a, 0x1f91, 0x1f95, 0x1f93, 0xf309,
  0x1f76, 0xf102, 0xf20b, 0xf20c, 0x1f30, 0x1f34, 0x1f32, 0xf20d,
  0x1f31, 0x1f35, 0x1f33, 0xf20e, 0x1f78, 0x1f40, 0x1f44, 0x1f42,
  0x1f41, 0x1f45, 0x1f43, 0x1fe5, 0x1fe4, 0x1f7a, 0xf103, 0xf20f,
  0xf210, 0x1f50, 0x1f54, 0x1f52, 0xf211, 0x1f51, 0x1f55, 0x1f53,
  0xf212, 0x1f7c, 0xf104, 0x1ff3, 0x1ff4, 0x1ff2, 0xf213, 0x1f60,
  0x1f64, 0x1f62, 0xf214, 0x1fa0, 0x1fa4, 0x1fa2, 0xf30a, 0x1f61,
  0x1f65, 0x1f63, 0xf215, 0x1fa1, 0x1fa5, 0x1fa3, 0xf30b, 0x03da,
  0x03dc, 0x03de, 0x03e0
};

// WP hebrew (charset 9)
static unsigned hebrew_map[] = {
  0x05d0, 0x05d1, 0x05d2, 0x05d3, 0x05d4, 0x05d5, 0x05d6, 0x05d7,
  0x05d8, 0x05d9, 0x05da, 0x05db, 0x05dc, 0x05dd, 0x05de, 0x05df,
  0x05e0, 0x05e1, 0x05e2, 0x05e3, 0x05e4, 0x05e5, 0x05e6, 0x05e7,
  0x05e8, 0x05e9, 0x05ea, 0xf862, 0x05c0, 0x05c3, 0x05f3, 0x05f4,
  0x05b0, 0x05b1, 0x05b2, 0x05b3, 0x05b4, 0x05b5, 0x05b6, 0x05b7,
  0x05b8, 0x05b9, 0x05b9, 0x05bb, 0x05bc, 0x05bd, 0x05bf, 0x05b7,
  0xfb1e, 0x05f0, 0x05f1, 0x05f2, 0xf114, 0xf8b0, 0xf863, 0xf864,
  0xf865, 0xf866, 0xf867, 0xf868, 0xf869, 0xf86a, 0xf86b, 0xf86c,
  0xf86d, 0xf86e, 0xf86f, 0xf870, 0xf871, 0xf872, 0xf873, 0xf874,
  0x05f3, 0x05f3, 0x05f4, 0xf876, 0xf877, 0xf878, 0xf879, 0xf87a,
  0xf87b, 0xf87c, 0xf87d, 0xf87e, 0xf115, 0xf116, 0xf87f, 0xf117,
  0xf118, 0xf119, 0xf11a, 0xf11b, 0xf11c, 0xf11d, 0xf11e, 0xf11f,
  0xf120, 0xf121, 0xf122, 0xf123, 0xf124, 0xf125, 0xf126, 0xf127,
  0xf218, 0xf128, 0xf129, 0xf12a, 0xf12b, 0xf12c, 0xf12d, 0xf880,
  0xf12e, 0xf12f, 0xf130, 0xf219, 0x05e9, 0xf131, 0xf132, 0xf140,
  0xf141, 0xf142, 0x20aa
};

// WP cyrillic (charset 10)
static unsigned cyrillic_map[] = {
  0x0410, 0x0430, 0x0411, 0x0431, 0x0412, 0x0432, 0x0413, 0x0433,
  0x0414, 0x0434, 0x0415, 0x0435, 0x0401, 0x0451, 0x0416, 0x0436,
  0x0417, 0x0437, 0x0418, 0x0438, 0x0419, 0x0439, 0x041a, 0x043a,
  0x041b, 0x043b, 0x041c, 0x043c, 0x041d, 0x043d, 0x041e, 0x043e,
  0x041f, 0x043f, 0x0420, 0x0440, 0x0421, 0x0441, 0x0422, 0x0442,
  0x0423, 0x0443, 0x0424, 0x0444, 0x0425, 0x0445, 0x0426, 0x0446,
  0x0427, 0x0447, 0x0428, 0x0448, 0x0429, 0x0449, 0x042a, 0x044a,
  0x042b, 0x044b, 0x042c, 0x044c, 0x042d, 0x044d, 0x042e, 0x044e,
  0x042f, 0x044f, 0x04d8, 0x04d9, 0x0403, 0x0453, 0x0490, 0x0491,
  0x0492, 0x0493, 0x0402, 0x0452, 0x0404, 0x0454, 0x0404, 0x0454,
  0x0496, 0x0497, 0x0405, 0x0455, 0xf159, 0xf889, 0xf15e, 0xf15f,
  0x0406, 0x0456, 0x0407, 0x0457, 0xf88c, 0xf88d, 0x0408, 0x0458,
  0x040c, 0x045c, 0x049a, 0x049b, 0xf160, 0xf161, 0x049c, 0x049d,
  0x0409, 0x0459, 0x04a2, 0x04a3, 0x040a, 0x045a, 0x047a, 0x047b,
  0x0460, 0x0461, 0x040b, 0x045b, 0x040e, 0x045e, 0x04ee, 0x04ef,
  0x04ae, 0x04af, 0x04b0, 0x04b1, 0x0194, 0x0263, 0x04b2, 0x04b3,
  0xf162, 0xf163, 0x04ba, 0x04bb, 0x047e, 0x047f, 0x040f, 0x045f,
  0x04b6, 0x04b7, 0x04b8, 0x04b9, 0xf164, 0xf165, 0x0462, 0x0463,
  0x0466, 0x0467, 0x046a, 0x046b, 0x046e, 0x046f, 0x0470, 0x0471,
  0x0472, 0x0473, 0x0474, 0x0475, 0xf400, 0xf401, 0xf402, 0xf403,
  0xf404, 0xf405, 0xf406, 0xf407, 0xf408, 0xf409, 0xf40a, 0xf40b,
  0xf40c, 0xf40d, 0xf40e, 0xf40f, 0xf410, 0xf411, 0xf412, 0xf413,
  0xf414, 0xf415, 0xf416, 0xf417, 0xf418, 0xf419, 0xf41a, 0xf41b,
  0xf41c, 0xf41d, 0xf41e, 0xf41f, 0xf420, 0xf421, 0xf422, 0xf423,
  0xf424, 0xf425, 0xf426, 0xf427, 0xf428, 0xf429, 0xf42a, 0xf42b,
  0x0301, 0x0300, 0x0308, 0x0306, 0x0326, 0x0328, 0x0304, 0xf893,
  0x201e, 0x201c, 0x10d0, 0x10d1, 0x10d2, 0x10d3, 0x10d4, 0x10d5,
  0x10d6, 0x10f1, 0x10d7, 0x10d8, 0x10d9, 0x10da, 0x10db, 0x10dc,
  0x10f2, 0x10dd, 0x10de, 0x10df, 0x10e0, 0x10e1, 0x10e2, 0x10e3,
  0x10f3, 0x10e4, 0x10e5, 0x10e6, 0x10e7, 0x10e8, 0x10e9, 0x10ea,
  0x10eb, 0x10ec, 0x10ed, 0x10ee, 0x10f4, 0x10ef, 0x10f0, 0x10f5,
  0x10f6, 0xf42c
};

// WP japanese (charset 11)
static unsigned japanese_map[] = {
  0xff61, 0xff62, 0xff63, 0xff64, 0xff65, 0xff66, 0xff67, 0xff68,
  0xff69, 0xff6a, 0xff6b, 0xff6c, 0xff6d, 0xff6e, 0xff6f, 0xff70,
  0xff71, 0xff72, 0xff73, 0xff74, 0xff75, 0xff76, 0xff77, 0xff78,
  0xff79, 0xff7a, 0xff7b, 0xff7c, 0xff7d, 0xff7e, 0xff7f, 0xff80,
  0xff81, 0xff82, 0xff83, 0xff84, 0xff85, 0xff86, 0xff87, 0xff88,
  0xff89, 0xff8a, 0xff8b, 0xff8c, 0xff8d, 0xff8e, 0xff8f, 0xff90,
  0xff91, 0xff92, 0xff93, 0xff94, 0xff95, 0xff96, 0xff97, 0xff98,
  0xff99, 0xff9a, 0xff9b, 0xff9c, 0xff9d, 0xff9e, 0xff9f
};

unsigned
Parser::ExtCharToUnicode (int charset, int charcode)
{
  unsigned ucode = 0;
  int cmax;

  if (charcode < 0)
    return 0;

  switch (charset)
    {
    case 1:
      // multinatinal character
      cmax = sizeof (multinational_map) / sizeof (multinational_map[0]);
      if (charcode < cmax)
        ucode = multinational_map[charcode];
      break;
    case 2:
      // phonetic symbol
      cmax = sizeof (phonetic_map) / sizeof (phonetic_map[0]);
      if (charcode < cmax)
        ucode = phonetic_map[charcode];
      break;
    case 3:
      // TODO box drawing
      break;
    case 4:
      // typographic symbol
      cmax = sizeof (typographic_map) / sizeof (typographic_map[0]);
      if (charcode < cmax)
        ucode = typographic_map[charcode];
      break;
    case 5:
      // iconic symbol
      cmax = sizeof (iconic_map) / sizeof (iconic_map[0]);
      if (charcode < cmax)
        ucode = iconic_map[charcode];
      break;
    case 6:
      // math/scientific
      cmax = sizeof (math_map) / sizeof (math_map[0]);
      if (charcode < cmax)
        ucode = math_map[charcode];
      break;
    case 7:
      // math/scientific extended
      cmax = sizeof (mathext_map) / sizeof (mathext_map[0]);
      if (charcode < cmax)
        ucode = mathext_map[charcode];
      break;
    case 8:
      // greek
      cmax = sizeof (greek_map) / sizeof (greek_map[0]);
      if (charcode < cmax)
        ucode = greek_map[charcode];
      break;
    case 9:
      // hebrew
      cmax = sizeof (hebrew_map) / sizeof (hebrew_map[0]);
      if (charcode < cmax)
        ucode = hebrew_map[charcode];
      break;
    case 10:
      // cyrillic
      cmax = sizeof (cyrillic_map) / sizeof (cyrillic_map[0]);
      if (charcode < cmax)
        ucode = cyrillic_map[charcode];
      break;
    case 11:
      // japanese
      cmax = sizeof (japanese_map) / sizeof (japanese_map[0]);
      if (charcode < cmax)
        ucode = japanese_map[charcode];
      break;
    }

  // FIXME not sure about this but I think codepoint >= 0xf000 is really
  // specific only to WP
  if( ucode >= 0xf000 ) ucode = 0;

  return ucode;
}
