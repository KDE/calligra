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

#include <config.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <qfile.h>
#include <qregexp.h>
#include <qdatastream.h>
#include <qptrlist.h>
#include <qmemarray.h>

#include <qdom.h>

#include <kdebug.h>

#include "wpimport.h"
#include "wpimport.moc"

// for WP 5.x
const int Code_PageNumber = 0x02;
const int Code_HardReturn = 0x0A;
const int Code_SoftPage = 0x0B;
const int Code_HardPage = 0x0C;
const int Code_SoftReturn = 0x0D;
const int Code_DormantHardReturn = 0x99;
const int Code_HardHypen = 0xA9;
const int Code_ExtendedChar = 0xC0;
const int Code_AttributeOn = 0xC3;
const int Code_AttributeOff = 0xC4;
const int Code_PageFormatGroup = 0xD0;
const int Code_FontGroup = 0xD1;
const int Code_DefinitionGroup = 0xD2;

// NOTE: 1 wpu = 1/1200 inch and 1 inch = 72 pt

static double WPUToPoint( int wpu )
{
  return wpu * 72 / 1200;
}

static int PointToWPU( double point )
{
  return (int)( point * 1200 / 72 );
}

// FIXME make this faster !
static void XMLEscape( QString &text )
{
  text.replace(QRegExp("&"), "&amp;");
  text.replace(QRegExp("<"), "&lt;");
  text.replace(QRegExp(">"), "&gt;");
}

WPImport::WPImport( KoFilter *parent, const char *name ):
                     KoFilter(parent, name)
{
  m_docstart = 0;

}

// this is the main beast
bool WPImport::filter(const QString &fileIn, const QString &fileOut,
                         const QString& from, const QString& to,
                         const QString &)
{
  // check for proper conversion
  if( to!= "application/x-kword" || from != "application/wordperfect" )
     return false;

  // open input file
  QFile in(fileIn);
  if(!in.open(IO_ReadOnly))
    {
      kdError() << "WordPerfect import filter error: Unable to open input file!" << endl;
      in.close();
      return false;
    }

  // redirect input stream
  stream.setDevice( &in );

  // prepare output document
  m_output = "";
  document.packets.setAutoDelete( true );
  document.formats.setAutoDelete( true );
  document.colors.setAutoDelete( true );

  // default title is the filename
  document.summary.desc_name = fileIn;


  // read and check header
  if( !readHeader() )
    {
      kdError() << "WordPerfect import filter: unable to identify header !" << endl;
      return false;
    }

  root = QDomDocument( "DOC" );
  QDomElement docElement = root.createElement( "DOC" );
  root.appendChild( docElement );
  docElement.setAttribute( "editor", "KWord" );
  docElement.setAttribute( "mime", "application/x-kword" );

  QDomElement paper = root.createElement( "PAPER" );
  docElement.appendChild( paper );
  QDomElement paperborders = root.createElement( "PAPERBORDERS" );
  paper.appendChild( paperborders );

  QDomElement attr = root.createElement( "ATTRIBUTES" );
  docElement.appendChild( attr );
  attr.setAttribute( "processing", 0 );
  attr.setAttribute( "standardpage", 1 );
  attr.setAttribute( "hasHeader", 0 );
  attr.setAttribute( "hasFooter", 0 );
  attr.setAttribute( "unit", "mm" );

  QDomElement framesets = root.createElement( "FRAMESETS" );
  docElement.appendChild( framesets );

  // NOTE: frameset is available as protected class member
  frameset = root.createElement( "FRAMESET" );
  framesets.appendChild( frameset );
  frameset.setAttribute( "frameType", 1 );
  frameset.setAttribute( "autoCreateNewFrame", 1 );
  frameset.setAttribute( "frameInfo", 0 );
  frameset.setAttribute( "removable", 0 );

  QDomElement frame = root.createElement( "FRAME" );
  frameset.appendChild( frame );
  frame.setAttribute( "left", 28 );
  frame.setAttribute( "top", 42 );
  frame.setAttribute( "right", 566 );
  frame.setAttribute( "bottom", 798 );

  // parse and convert the document
  if( !parseDocument( ) )
    {
      kdError() << "WordPerfect import filter: unable to parse document !" << endl;
      return false;
    }

  in.close();

  // adjust paper settings
  paper.setAttribute( "format", 1 );
  paper.setAttribute( "width", 595 );
  paper.setAttribute( "height", 841 );
  paper.setAttribute( "orientation", 0 );
  paper.setAttribute( "hType", 0 );
  paper.setAttribute( "fType", 0 );
  paper.setAttribute( "columns", document.pagesettings.columns );
  if( document.pagesettings.columns > 1 )
    paper.setAttribute(" columnspacing", WPUToPoint( document.pagesettings.columnspacing ) );

  // adjust page margins
  paperborders.setAttribute( "left", WPUToPoint( document.pagesettings.leftmargin ) );
  paperborders.setAttribute( "right", WPUToPoint( document.pagesettings.rightmargin ) );
  paperborders.setAttribute( "top", WPUToPoint( document.pagesettings.topmargin ) );
  paperborders.setAttribute( "bottom", WPUToPoint( document.pagesettings.bottommargin ) );

  // prepare storage
  KoStore out=KoStore( QString(fileOut), KoStore::Write );

  // store output document
  if( out.open( "root" ) )
    {
      QCString cstring = root.toString().utf8();
      cstring.prepend( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" );

      out.write( (const char*) cstring, cstring.length() );
      out.close();

    }

  // store document information
  if( out.open("documentinfo.xml") )
    {
      QDomDocument docinfo( "document-info" );
      QDomElement main = docinfo.createElement( "documentinfo" );
      docinfo.appendChild( main );

      QDomElement author = docinfo.createElement( "author" );
      main.appendChild( author );
      QDomElement about = docinfo.createElement( "about" );
      main.appendChild( about );

      QDomElement fullname = docinfo.createElement( "full-name" );
      fullname.appendChild( docinfo.createTextNode( document.summary.author ) );
      author.appendChild( fullname );

      QDomElement title = docinfo.createElement( "title" );
      title.appendChild( docinfo.createTextNode( document.summary.desc_name ) );
      about.appendChild( title );
      QDomElement abstract = docinfo.createElement( "abstract" );
      abstract.appendChild( docinfo.createCDATASection( document.summary.abstract ) );
      about.appendChild( abstract );

      QCString cstring = docinfo.toString().utf8();
      cstring.prepend( "<?xml version=\"1.0\"?>\n" );

      out.write( (const char*)cstring, cstring.length() );
      out.close();
    }

  return true;
}

// return false if error or failed
bool WPImport::readHeader()
{
  kdDebug() << "WordPerfect import filter: checking document header" << endl;

  // get 16-byte header
  QArray<int> header( 16 );
  for( int c = 0; c < 16; c++ )
    header[c] = readByte();

  // check first 4 bytes, must be -1,WPC
  // or (in hex): 0xFF, 0x57, 0x50, 0x43
  if( (header[0] != 0xFF) || (header[1] != 0x57) ||
      (header[2] != 0x50) || (header[3] != 0x43) )
    return false;

  // get document start
  m_docstart = header[4] + (header[5] << 8) + (header[6] << 16) + (header[7] << 24);
  if( m_docstart < 0 ) return false;
  if( m_docstart > stream.device()->size() ) return false;

  int product_type = header[8];
  int file_type = header[9];
  if( ( product_type != 1 ) || ( file_type != 10 ) )
    {
      kdDebug() << "WordPerfect import filter: not a WordPerfect document" << endl;
      return false;
    }

  document.version.major = header[10];
  document.version.minor = header[11];

  int encrypt = header[12] + (header[13] << 13);
  if( encrypt )
    {
      kdDebug() << "WordPerfect import filter: document is encrypted" << endl;
      return false;
    }

  // look for packets in prefix area
  for( long next_block = 16; next_block > 0; )
    {
      QArray<int> buf( 10 );

      stream.device()->at( next_block );
      for( int c = 0; c < 10; c++ )
	buf[c] = readByte();


      int type = buf[0] + ( buf[1] << 8 );
      int count = buf[2] + ( buf[3] << 8 );
      int size = buf[4] + ( buf[5] << 8 );
      next_block = buf[6] + ( buf[7] << 8 ) + ( buf[8] << 16 ) + ( buf[9] << 24 );

      if( type != 0xFFFB) break;
      if( size != count * 10 ) break;

      for( int v = 0; v < count; v++ )
	{
	  for( int i = 0; i < 10; i++ )
	    buf[i] = readByte();

	  int packet_type = buf[0] + ( buf[1] << 8 );
	  long packet_size = buf[2] + ( buf[3] << 8 ) + ( buf[4] << 16 ) + ( buf[5] << 24 );
	  long packet_pos = buf[6] + ( buf[7] << 8 ) + ( buf[8] << 16 ) + ( buf[9] << 24 );

	  if( packet_type == 0 ) break;
	  if( packet_pos <= 0 ) continue;
	  if( packet_size < 0 ) continue;
	  if( packet_pos + packet_size > stream.device()->size() ) continue;

	  WPDocument::Packet* p = new WPDocument::Packet;
	  p->type = packet_type;
	  p->size = packet_size;
	  p->pos = packet_pos;
	  document.packets.append( p );
	}
    }

  // load all packets in prefix area
  for( QPtrListIterator<WPDocument::Packet> it( document.packets ); it; ++it )
    {
      WPDocument::Packet* p = it.current();
      stream.device()->at( p->pos );
      p->data.resize( p->size );
      for( int q = 0; q < p->size; q++)
	p->data[q] = readByte();
    }

  // process known packets
  for( QPtrListIterator<WPDocument::Packet> it( document.packets ); it; ++it )
    {
      WPDocument::Packet* p = it.current();
      if( p->data.size() > 0 )
	handlePacket( p->type, p->data );
    }

  return true;
}

#include <stdio.h>

// parse the document
bool WPImport::parseDocument()
{
  QArray<int> data;

  // initialize
  m_text = "";
  document.pagesettings.leftmargin = 1200;
  document.pagesettings.rightmargin = 1200;
  document.pagesettings.topmargin = 1200;
  document.pagesettings.bottommargin = 1200;
  document.pagesettings.columns = 1;
  document.pagesettings.columnspacing = 400;
  resetTextFormat();
  resetParagraphLayout();

  // seek to start of document area
  stream.device()->at( m_docstart );

  while( !stream.atEnd() )
    {
      int code = readByte();
      data.resize( 0 );

      // invalid characters (should not be here, just skip)
      if( code == 0x00 ) break;
      if( code == 0xFF ) break;

      // 0-31 control characters
      if( code == Code_HardReturn )
	handleFunction( Code_HardReturn, 0, data );
      if( code == Code_SoftReturn ) m_text.append( " " );

      // 32-127 ASCII characters
      if( ( code >= 32 ) && ( code <= 127 ) )
	  m_text.append( code );

      // 128-191 single-byte function
      if( code == Code_HardHypen ) m_text.append( "-" );

      // 192-207 fixed-length multi-byte function
      if( ( code >= 192 ) && ( code <= 207 ) )
	{
	  int lentab[] = { 2, 7, 9, 1, 1, 3, 4, 5, 2, 3, 4, 4, 6, 8, 8, 10 };
	  int length = lentab[ code - 192 ];
	  data.resize( length );
	  for( int c = 0; c < length; c++)
	    data[c] = readByte();
	  readByte(); // should match code ??
	  handleFunction( code, 0, data );
	}

      // 208-255 variable-length multi-byte function
      if( ( code >= 208 ) && ( code < 255 ) )
	{
	  int subfunction = readByte();
	  long length = readByte() + ( readByte() << 8 );
	  data.resize( length );
	  for( int c=0 ; length && !stream.atEnd(); length--, c++ )
	    data[c] = readByte();
	  handleFunction( code, subfunction, data );
	}

    }

  // just in case...
  if ( m_text.length() ) writeParagraph();

  return true;
}

void WPImport::handleFunction( int function, int subfunction, QArray<int>& data )
{
  // hard return: write the paragraph out
  if( function == Code_HardReturn )
    {
      writeParagraph();
      m_text = "";
      resetTextFormat();
      resetParagraphLayout();
    }

  // attribute on
  if( function == Code_AttributeOn )
    {
      WPDocument::TextFormat * p = new WPDocument::TextFormat;
      p->type = data[0];
      p->pos = m_text.length();
      p->len = 0;
      document.formats.append( p );
    }

  // attribute off: look for previous matching attr on
  if( function == Code_AttributeOff )
    {
      int type = data[0];
      QPtrListIterator<WPDocument::TextFormat> it( document.formats );
      for( it.toLast(); it.current(); --it )
	{
	  WPDocument::TextFormat *p = it.current();
	  if( p ) if( p->type == type )
	    p->len = m_text.length() - p->pos;
	}
    }

  // page format group -> left/right margins
  if( ( function == Code_PageFormatGroup ) && ( subfunction == 1 ) )
    {
      document.pagesettings.leftmargin = data[4] + (data[5] << 8);
      document.pagesettings.rightmargin = data[6] + (data[7] << 8);
    }

  // page format group -> linespace
  if( ( function == Code_PageFormatGroup ) && ( subfunction == 2 ) )
    m_layout.linespace = (data[3] << 8) + data[2];

  // page format group -> top/bottom margins
  if( ( function == Code_PageFormatGroup ) && ( subfunction == 5 ) )
    {
      document.pagesettings.topmargin = data[4] + (data[5] << 8);
      document.pagesettings.bottommargin = data[6] + (data[7] << 8);
    }

  // page format group -> justification
  if( ( function == Code_PageFormatGroup ) && ( subfunction == 6 ) )
      m_layout.justification = data[1];

  // definition group -> define columns
  // NOTE: At present, KWords only support equally-width columns
  // (along with the same column spacing)
  if( ( function == Code_DefinitionGroup ) && ( subfunction == 1 ) )
    {
      document.pagesettings.columns = data[97] & 31; // bit 0-4

      // just take space between second and first column
      if( document.pagesettings.columns > 1 )
	{
	  int first_right = data[100] + (data[101] << 8);
	  int second_left = data[102] + (data[103] << 8);
	  document.pagesettings.columnspacing = second_left - first_right;
	}
    }

  // font group -> color
  if( ( function == Code_FontGroup ) && ( subfunction == 0 ) )
    {

      // fix-up the last
      if( document.colors.count() > 0 )
	{
	  QPtrListIterator<WPDocument::FontColor> it( document.colors );
	  WPDocument::FontColor * p = it.toLast();
	  p->len = m_text.length() - p->pos;
	}

      // now add one new entry
      WPDocument::FontColor *f = new WPDocument::FontColor;
      f->pos = m_text.length();
      f->len = 0;
      f->red = data[3];
      f->green = data[4];
      f->blue = data[5];
      document.colors.append( f );

    }

  // FIXME handle extended characters
  // at the moment, this is only a nasty hack...
  if( function == Code_ExtendedChar )
    {
      int charcode = data[0];
      int charset = data[1];

      if( ( charcode == 33 ) && ( charset == 4 ) ) m_text.append( "-" );
      if( ( charcode == 28 ) && ( charset == 4 ) ) m_text.append( 39 );
    }

}

void WPImport::handlePacket( int type, QArray<int>& data )
{

  // document summary
  if( type == 1 )
    {
      bool WP51Marker = data[ data.size()-1] == 0xFF;
      int i;

      QString desc_name;

      int limit = WP51Marker ? 94 : 67;
      for( i = 26; i < limit; i++)
	desc_name.append( data[i] );
      document.summary.desc_name = desc_name.stripWhiteSpace();

      // Description Type is available only in WP 5.1
      if( WP51Marker )
	for( i = 94; (i < data.size() ) && (data[i]); i++)
	  document.summary.desc_type.append( data[i] );

      for( i++; (i < data.size() ) && (data[i]); i++)
	document.summary.subject.append( data[i] );

      for( i++; (i < data.size() ) && (data[i]); i++)
	document.summary.author.append( data[i] );

      for( i++; (i < data.size() ) && (data[i]); i++)
	document.summary.typist.append( data[i] );

      for( i++; (i < data.size() ) && (data[i]); i++)
	document.summary.abstract.append( data[i] );
    }

}

// reset text format
void WPImport::resetTextFormat()
{
  document.formats.clear();
  document.colors.clear();
}

// reset paragraph layout
void WPImport::resetParagraphLayout()
{
  m_layout.justification = 0; // left-justify
  m_layout.linespace = 0;    // not specified
}

// happened after hard-return
void WPImport::writeParagraph()
{
  QDomElement para = root.createElement( "PARAGRAPH" );
  frameset.appendChild( para );

  QDomElement text = root.createElement ( "TEXT" );
  para.appendChild( text );
  text.appendChild( root.createTextNode( m_text ) );

  QDomElement formats = root.createElement( "FORMATS" );
  para.appendChild( formats );

  // handle character attribute as <FORMAT>
  // TODO supports for other attributes such as shadowed and outlined
  for( QPtrListIterator<WPDocument::TextFormat> i( document.formats ); i.current(); ++i )
    {
      WPDocument::TextFormat* p = i.current();
      QDomElement format = root.createElement( "FORMAT" );
      formats.appendChild( format );
      format.setAttribute( "id", 1 );
      format.setAttribute( "pos", p->pos );
      format.setAttribute( "len", p->len );

      QDomElement vertalign = root.createElement( "VERTALIGN" );
      format.appendChild( vertalign );
      vertalign.setAttribute( "value", (p->type == 5) ? 2: (p->type == 6 ) ? 1 : 0 );

      QDomElement italic = root.createElement( "ITALIC" );
      format.appendChild( italic );
      italic.setAttribute( "value", ( p->type == 8 ) ? 1 : 0 );

      // FIXME double-underline now still treated as underline
      QDomElement underline = root.createElement( "UNDERLINE" );
      format.appendChild( underline );
      underline.setAttribute( "value", ( (p->type == 11) || (p->type == 14 ) ) ? 1 : 0 );

      QDomElement weight = root.createElement( "WEIGHT" );
      format.appendChild( weight );
      weight.setAttribute( "value", ( p->type == 12 ) ? 75 : 50 );

      QDomElement strikeout = root.createElement( "STRIKEOUT" );
      format.appendChild( strikeout );
      strikeout.setAttribute( "value", ( p->type == 13 ) ? 1 : 0 );
    }

  // handle font color in <FORMAT>
  for(QPtrListIterator<WPDocument::FontColor> j( document.colors ); j.current(); ++j )
    {
      WPDocument::FontColor* p = j.current();
      QDomElement format = root.createElement( "FORMAT" );
      formats.appendChild( format );
      format.setAttribute( "id", 1 );
      format.setAttribute( "pos", p->pos );
      format.setAttribute( "len", p->len );

      QDomElement color = root.createElement( "COLOR" );
      format.appendChild( color );
      color.setAttribute( "red", p->red );
      color.setAttribute( "green", p->green );
      color.setAttribute( "blue", p->blue );
    }

  // handle paragraph layout
  QDomElement layout = root.createElement( "LAYOUT" );
  para.appendChild( layout );

  QDomElement name = root.createElement( "NAME" );
  layout.appendChild( name );
  name.setAttribute( "value", "Standard" );

  QDomElement flow = root.createElement( "FLOW" );
  layout.appendChild( flow );
  QString kw_flow = ( m_layout.justification == 0 ) ? "left" :
    ( m_layout.justification == 1 ) ? "justify" :
    ( m_layout.justification == 2 ) ? "center" :
    ( m_layout.justification == 3 ) ? "right" : "left";
  flow.setAttribute( "align", kw_flow );

  // handle linespacing
  // WP value: 0x100 = one, 0x200 = double, ...
  // KWord value: 72 = one, 144 = double, ...
  // also handle special case for normal, one-and-half, double
  QDomElement linespacing = root.createElement( "LINESPACING" );
  layout.appendChild( linespacing );
  QString kw_linespacing = QString::number( m_layout.linespace * 72 / 256.0 );
  if( m_layout.linespace == 0 ) kw_linespacing = "0";
  else if( m_layout.linespace == 0x200 ) kw_linespacing = "double";
  else if ( m_layout.linespace == 0x180 ) kw_linespacing = "oneandhalf";
  linespacing.setAttribute( "value", kw_linespacing );

  QDomElement leftborder = root.createElement( "LEFTBORDER" );
  layout.appendChild( leftborder );
  leftborder.setAttribute( "width", 0 );
  leftborder.setAttribute( "style", 0 );

  QDomElement rightborder = root.createElement( "RIGHTBORDER" );
  layout.appendChild( rightborder );
  rightborder.setAttribute( "width", 0 );
  rightborder.setAttribute( "style", 0 );

  QDomElement topborder = root.createElement( "TOPBORDER" );
  layout.appendChild( topborder );
  topborder.setAttribute( "width", 0 );
  topborder.setAttribute( "style", 0 );

  QDomElement borderborder = root.createElement( "BORDERBORDER" );
  layout.appendChild( borderborder );
  borderborder.setAttribute( "width", 0 );
  borderborder.setAttribute( "style", 0 );

  QDomElement layoutformat = root.createElement( "FORMAT" );
  layout.appendChild( layoutformat );
  layoutformat.setAttribute( "id", 1 );

  QDomElement layoutformatweight = root.createElement( "WEIGHT" );
  layoutformat.appendChild( layoutformatweight );
  layoutformatweight.setAttribute( "value", 50 );
  QDomElement layoutformatcolor = root.createElement( "COLOR" );
  layoutformat.appendChild( layoutformatcolor );
  layoutformatcolor.setAttribute( "red", 0 );
  layoutformatcolor.setAttribute( "green", 0 );
  layoutformatcolor.setAttribute( "blue", 0 );
  QDomElement layoutformatfont = root.createElement( "FONT" );
  layoutformat.appendChild( layoutformatfont );
  layoutformatfont.setAttribute( "name", "Helvetica" );
  QDomElement layoutformatsize = root.createElement( "SIZE" );
  layoutformat.appendChild( layoutformatsize );
  layoutformatsize.setAttribute( "value", 11 );
}

// Reads 8-bit byte from stream and returns it as an integer
int WPImport::readByte()
{
  Q_UINT8 c;
  stream >> c;
  return c;
}

