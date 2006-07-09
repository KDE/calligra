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

#include "amiproparser.h"

#include <qfile.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qtextstream.h>

const float AmiPro::LS_Single = -1;
const float AmiPro::LS_OneAndHalf = -1.5;
const float AmiPro::LS_Double = -2;

// helper function to "unescape" AmiPro string
static QString AmiProUnescape( const QString& str )
{
  QString result;

  for( unsigned i=0; i< str.length(); i++ )
  {
    QChar c = str[i];
    result.append( c );

    // check for "@@", decoded as '@'
    if( c == '@' )
      if( str[i+1] == '@' )
         i++ ; // eat !

    // a few possible escape sequence
    if( c == '<'  )
    {

      // check for "<<", decoded as '<'
      if( str[i+1] == '<' )
      {
        result.truncate( result.length() - 1 ); // remove the '<'
        result.append( '<' );
        i++;
      }

      // check for "<;>", decoded as '>'
      if( str[i+1] == ';' )
      {
        result.truncate( result.length() - 1 ); // remove the '<'
        result.append( '>' );
        i+=2;
      }

      // check for "<[>", decoded as '['
      if( str[i+1] == '[' )
      {
        result.truncate( result.length() - 1 ); // remove the '<'
        result.append( '[' );
        i+=2;
      }

      // some special characters
      if( str[i+1] == '/' )
      {
        if( str[i+2] == 'R' )
        {
          result.truncate( result.length() - 1 ); // remove the '<'
          result.append( '\'' ); // </R> decoded as '
          i += 3;
        }
        else
        {
          result.truncate( result.length() - 1 ); // remove the '<'
          result.append( QChar(str[i+2].unicode() + 0x40 ) );
          i += 3;
        }
      }

      // yet another special characters
      if( str[i+1] == '\\' )
      {
        result.truncate( result.length() - 1 ); // remove the '<'
        result.append( QChar(str[i+2].unicode() | 0x80 ) );
        i += 3;
      }

    }

  }

  return result;
}

AmiProParser::AmiProParser()
{
  m_result = OK;
  m_listener = NULL;
}

AmiProParser::~AmiProParser()
{
}

bool AmiProParser::setResult( int result )
{
  m_result = result;
  return m_result == OK;
}

void AmiProParser::setListener( AmiProListener *listener )
{
  m_listener = listener;
}

bool AmiProParser::process( const QString& filename )
{
  QString line;

  // open input file
  QFile in( filename );
  if( !in.open( IO_ReadOnly))
    return setResult( FileError );

  QTextStream stream;
  stream.setDevice( &in );

  // the first should be "[ver]"
  line = stream.readLine();
  if( line != "[ver]" ) 
    return setResult( InvalidFormat ); 
 
  // get format version, typically 4 
  line = stream.readLine();
  int format_version = line.toInt();

  // FIXME is this necessary ?
  // accept only format version 4
  if( format_version != 4 )
    return setResult( InvalidFormat );

  // initialize
  m_currentFormat = AmiProFormat();
  m_formatList.clear();
  m_styleList.clear();
  m_currentSection = "";
  QStringList lines;

  // parse line-by-line
  for( ;; )
  {

    line = stream.readLine();
    if( line.isNull() ) break;

    QString old_section = m_currentSection;
    bool enter_new_section = false;

    // new main section ?
    if( !line.isEmpty() ) if( line[0] == '[' )
    {
      enter_new_section = true;
      m_currentSection = "";
      for( unsigned i=1; i<line.length(); i++ )
        if( line[i] == ']' ) break;
        else m_currentSection += line[i];
    }

    // leave [tag]
    if( enter_new_section && ( old_section == "tag" ) )
    {
      parseStyle( lines );
      lines.clear();
    }

    // leave [edoc]
    if( enter_new_section && ( old_section == "edoc" ) )
    {
      parseParagraph( lines.join(" ") );
      lines.clear();
    }

    // still in [tag]
    if( !enter_new_section && ( old_section == "tag" ) )
    {
      lines.append( line );
    } 

    // still in [edoc]
    if( !enter_new_section && ( old_section == "edoc" ) )
    {
      if( line.isEmpty() ) 
      {
         parseParagraph( lines );
         lines.clear(); 
      }
        lines.append( line );
    }

    // enter [tag]
    if( enter_new_section && ( m_currentSection == "tag" ) )
    {
      lines.clear();
    }

    // enter [edoc]
    if( enter_new_section && ( m_currentSection == "edoc" ) )
    {
      processOpenDocument();
      lines.clear();
    }

  }

  // in case left-over
  if( lines.count() > 0 ) parseParagraph( lines.join( " " ) );

  processCloseDocument();

  return true;
}

bool AmiProParser::processOpenDocument()
{
  if( m_listener ) return m_listener->doOpenDocument();
  return true;
}

bool AmiProParser::processCloseDocument()
{
  if( m_listener ) 
    return m_listener->doCloseDocument();
  return true;
}

bool AmiProParser::parseParagraph( const QStringList& lines )
{
  m_text = "";
  m_formatList.clear();
  m_layout = AmiProLayout();

  // join the lines, up until first char in a line is '>'
  QString partext = "";
  for( unsigned i=0; i<lines.count(); i++ )
    if( lines[i][0] == '>' ) break;
      else partext.append( lines[i] + "\n" );

  QChar ch = partext[partext.length()-1];
  while( ( ch == '\n' ) || ( ch == '\r' ) )
  {
    partext.remove( partext.length()-1, 1 );
    ch = partext[partext.length()-1];
  }

  // "unescape", process special chars and such
  QString text = AmiProUnescape( partext );

  // apply default style first
  m_layout.applyStyle( findStyle( "Body Text" ) );

  for( unsigned i=0; i<text.length(); i++ )
  {
    QChar ch = text[i];

    // handle a tag
    if( ch == '<' )
    {
        QString tag = "";
        for( i++; (i < text.length()) && 
           (text[i] != '>'); i++) tag.append( text[i] );
        handleTag( tag );
    }

    else

    // handle style change
    if( ch == '@' )
    {
        QString styleName;
        for( i++; (i < partext.length()) && (partext[i] != '@'); i++)
          styleName += partext[i];
        m_layout.name = styleName;
        AmiProStyle style = findStyle( styleName );
        m_currentFormat.applyStyle( style );
        m_formatList.append( m_currentFormat ); 
        m_layout.applyStyle( style );
    }

     else 
       // normal character
       m_text.append( ch ); 
  }

  // calc length of each format tag
  for( unsigned j=0; j<m_formatList.count(); j++ )
  {
    int nextpos;
    AmiProFormat& format = m_formatList[j];
    if( j < m_formatList.count()-1 )
    {
      AmiProFormat& nextformat = m_formatList[j+1];
      nextpos = nextformat.pos;
    }
    else  nextpos = m_text.length();
    format.len = nextpos - format.pos;
  }

  if( m_listener ) 
    return m_listener->doParagraph( m_text, m_formatList, m_layout );

  return true;
}

bool AmiProParser::parseStyle( const QStringList& lines )
{
  AmiProStyle style;

  style.name = AmiProUnescape( lines[0].stripWhiteSpace() );
  if( style.name.isEmpty() ) return true;

  // font
  if( lines[2].stripWhiteSpace() != "[fnt]" ) return true;
  style.fontFamily = lines[3].stripWhiteSpace();
  style.fontSize = lines[4].stripWhiteSpace().toFloat() / 20.0;

  unsigned color = lines[5].stripWhiteSpace().toUInt();
  style.fontColor.setRgb( color&255, (color>>8)&255, (color>>16)&255);

  unsigned flag = lines[6].stripWhiteSpace().toUInt();
  style.bold = flag & 1;
  style.italic = flag & 2;
  style.underline = flag & 4;
  style.word_underline = flag & 8;
  style.double_underline = flag & 64;

  // alignment
  if( lines[7].stripWhiteSpace() != "[algn]" ) return true;
  unsigned align_flag = lines[8].stripWhiteSpace().toUInt();
  if( align_flag & 1 ) style.align = Qt::AlignLeft;
  if( align_flag & 2 ) style.align = Qt::AlignRight;
  if( align_flag & 4 ) style.align = Qt::AlignCenter;
  if( align_flag & 8 ) style.align = Qt::AlignJustify;

  // linespace
  if( lines[13].stripWhiteSpace() != "[spc]" ) return true;
  unsigned ls_flag = lines[14].stripWhiteSpace().toUInt();
  if( ls_flag & 1 ) style.linespace = AmiPro::LS_Single;
  if( ls_flag & 2 ) style.linespace = AmiPro::LS_OneAndHalf;
  if( ls_flag & 4 ) style.linespace = AmiPro::LS_Double;
  if( ls_flag & 8 ) 
    style.linespace = lines[15].stripWhiteSpace().toFloat() / 20.0;
  style.spaceBefore = lines[17].stripWhiteSpace().toFloat() / 20.0;
  style.spaceAfter = lines[18].stripWhiteSpace().toFloat() / 20.0;

  m_styleList.append( style );

  // "Style #0", "Style #1" and such are special styles
  // do not import these styles
  if( style.name.left( 7 ) != "Style #" )
  if( m_listener )
    return m_listener->doDefineStyle( style );
  return true;
}

AmiProStyle AmiProParser::findStyle( const QString& name )
{
  AmiProStyleList::iterator it;
  for( it=m_styleList.begin(); it!=m_styleList.end(); ++it )
  {
    AmiProStyle& style = *it;
    if( style.name == name )
      return style;
  }
  return AmiProStyle();
}

bool AmiProParser::handleTag( const QString& tag )
{
  // > (actually encoded as <;>)
  if( tag == ";" )
    m_text.append( ">" );

  // [ (actually encoded as <[>)
  if( tag == "[" )
    m_text.append( "[" );

  // bold on
  if( tag == "+!" )
  {
    m_currentFormat.bold = true;
    m_currentFormat.pos = m_text.length();
    m_formatList.append( m_currentFormat );
  }

  // bold off
  if( tag == "-!" )
  {
    m_currentFormat.bold = false;
    m_currentFormat.pos = m_text.length();
    m_formatList.append( m_currentFormat );
  }

  // italic on
  if( tag == "+\"" )
  {
    m_currentFormat.italic = true;
    m_currentFormat.pos = m_text.length();
    m_formatList.append( m_currentFormat );
  }

  // italic off
  if( tag == "-\"" )
  {
    m_currentFormat.italic = false;
    m_currentFormat.pos = m_text.length();
    m_formatList.append( m_currentFormat );
  }

  // underline on
  if( tag == "+#" )
  {
    m_currentFormat.underline = true;
    m_currentFormat.pos = m_text.length();
    m_formatList.append( m_currentFormat );
  }

  // underline off
  if( tag == "-#" )
  {
    m_currentFormat.underline = false;
    m_currentFormat.pos = m_text.length();
    m_formatList.append( m_currentFormat );
  }

  // double underline on
  if( tag == "+)" )
  {
    m_currentFormat.double_underline = true; 
    m_currentFormat.pos = m_text.length();
    m_formatList.append( m_currentFormat );
  }

  // double underline off
  if( tag == "-)" )
  {
    m_currentFormat.double_underline = false;
    m_currentFormat.pos = m_text.length();
    m_formatList.append( m_currentFormat );
  }

 // word underline on
  if( tag == "+$" )
  {
    m_currentFormat.word_underline = true; 
    m_currentFormat.pos = m_text.length();
    m_formatList.append( m_currentFormat );
  }

  // word underline off
  if( tag == "-$" )
  {
    m_currentFormat.word_underline = false;
    m_currentFormat.pos = m_text.length();
    m_formatList.append( m_currentFormat );
  }

  // superscript on
  if( tag == "+&" )
  {
    m_currentFormat.superscript = true; 
    m_currentFormat.pos = m_text.length();
    m_formatList.append( m_currentFormat );
  }

  // superscript off
  if( tag == "-&" )
  {
    m_currentFormat.superscript = false;
    m_currentFormat.pos = m_text.length();
    m_formatList.append( m_currentFormat );
  }

  // subscript on
  if( tag == "+'" )
  {
    m_currentFormat.subscript = true;
    m_currentFormat.pos = m_text.length();
    m_formatList.append( m_currentFormat );
  }

  // subscript off
  if( tag == "-'" )
  {
    m_currentFormat.subscript = false;
    m_currentFormat.pos = m_text.length();
    m_formatList.append( m_currentFormat );
  }

  // strikethrough on
  if( tag == "+%" )
  {
    m_currentFormat.strikethrough = true;
    m_currentFormat.pos = m_text.length();
    m_formatList.append( m_currentFormat );
  }

  // strikethrough off
  if( tag == "-%" )
  {
    m_currentFormat.strikethrough = false;
    m_currentFormat.pos = m_text.length();
    m_formatList.append( m_currentFormat );
  }

  // paragraph left-align
  if( tag == "+@" )
    m_layout.align = Qt::AlignLeft;

  // paragraph right-align
  if( tag == "+A" )
    m_layout.align = Qt::AlignRight;

  // paragraph center
  if( tag == "+B" )
    m_layout.align = Qt::AlignCenter;

  // paragraph justify
  if( tag == "+C" )
    m_layout.align = Qt::AlignJustify;

  // linespace
  if( tag.left( 3 ) == ":S+" )
  {
    float ls = tag.right( tag.length() - 3 ).toFloat();
    m_layout.linespace = (ls == -1) ? AmiPro::LS_Single :
     (ls == -2) ? AmiPro::LS_OneAndHalf :
     (ls == -3) ? AmiPro::LS_Double : ls / 20.0;
  }

  // font
  if( tag.left( 2 ) == ":f" )
  {
    QString fontdesc = tag.right( tag.length()-2 );
    QStringList desc = QStringList::split( ",", fontdesc );
    if( desc.count() > 0 ) m_currentFormat.fontSize = desc[0].toFloat() / 20.0;
    if( desc.count() > 1 )
    {
      QString fontFamily = desc[1];
      if( fontFamily[0].isDigit() ) fontFamily.remove( 0, 1 );
      m_currentFormat.fontFamily = fontFamily;
    }
    if( desc.count() > 4 )
    {
      unsigned red = desc[2].toUInt();
      unsigned green = desc[3].toUInt();
      unsigned blue = desc[4].toUInt();
      m_currentFormat.fontColor.setRgb( red, green, blue );
    }
    m_formatList.append( m_currentFormat );
  }

  return true;
}

// text formatting
AmiProFormat::AmiProFormat()
{
  pos = len = 0;
  bold = italic = underline = 
  word_underline = double_underline = 
  subscript = superscript = strikethrough = FALSE;
  fontFamily = "";
  fontSize = 12;
  fontColor = Qt::black;
}

void AmiProFormat::assign( const AmiProFormat& f )
{
  pos = f.pos;
  len = f.len;
  bold = f.bold;
  italic = f.italic;
  underline = f.underline;
  word_underline = f.word_underline;
  double_underline = f.double_underline;
  subscript = f.subscript;
  superscript = f.superscript;
  strikethrough = f.strikethrough;
  fontFamily = f.fontFamily;
  fontSize = f.fontSize;
  fontColor = f.fontColor;
}

AmiProFormat::AmiProFormat( const AmiProFormat& f )
{
  assign( f );
}

AmiProFormat& AmiProFormat::operator=(  const AmiProFormat& f )
{
  assign( f );
  return *this;
}

void AmiProFormat::applyStyle( const AmiProStyle& style )
{
  fontFamily = style.fontFamily;
  fontSize = style.fontSize;
  fontColor = style.fontColor;
  bold = style.bold;
  italic = style.italic;
  underline = style.underline;
  word_underline = style.word_underline;
  double_underline = style.double_underline;
  subscript = style.subscript;
  superscript = style.superscript;
  strikethrough = style.strikethrough;
}

// paragraph layout
AmiProLayout::AmiProLayout()
{
  name = "";
  fontFamily = "";
  fontSize = 12;
  fontColor = Qt::black;
  bold = italic = underline = 
  word_underline = double_underline = 
  subscript = superscript = strikethrough = FALSE;
  align = Qt::AlignLeft;
  linespace = AmiPro::LS_Single;
  spaceBefore = spaceAfter = 0;
}

void AmiProLayout::assign( const AmiProLayout &l )
{
  name = l.name;
  fontFamily = l.fontFamily;
  fontSize = l.fontSize;
  fontColor = l.fontColor;
  bold = l.bold;
  italic = l.italic;
  underline = l.underline;
  word_underline = l.word_underline;
  double_underline = l.double_underline;
  subscript = l.subscript;
  superscript = l.superscript;
  strikethrough = l.strikethrough;
  align = l.align;
  linespace = l.linespace;
  spaceBefore = l.spaceBefore;
  spaceAfter = l.spaceAfter;
}

AmiProLayout::AmiProLayout( const AmiProLayout& l )
{
  assign( l );
}

AmiProLayout& AmiProLayout::operator=( const AmiProLayout& l )
{
  assign( l );
  return *this;
}

void AmiProLayout::applyStyle( const AmiProStyle& style )
{
  fontFamily = style.fontFamily;
  fontSize = style.fontSize;
  fontColor = style.fontColor;
  bold = style.bold;
  italic = style.italic;
  underline = style.underline;
  word_underline = style.word_underline;
  double_underline = style.double_underline;
  subscript = style.subscript;
  superscript = style.superscript;
  strikethrough = style.strikethrough;
  align = style.align;
  linespace = style.linespace;
  spaceBefore = style.spaceBefore;
  spaceAfter = style.spaceAfter;
}

// style definition
AmiProStyle::AmiProStyle()
{
  name = "Unnamed";
  fontFamily = "";
  fontSize = 12;
  fontColor = Qt::black;
  bold = italic = underline = 
  word_underline = double_underline = 
  subscript = superscript = strikethrough = FALSE;
  linespace = AmiPro::LS_Single;
  spaceBefore = spaceAfter = 0;
}

void AmiProStyle::assign( const AmiProStyle& s )
{
  name = s.name;
  fontFamily = s.fontFamily;
  fontSize = s.fontSize;
  fontColor = s.fontColor;
  bold = s.bold;
  italic = s.italic;
  underline = s.underline;
  word_underline = s.word_underline;
  double_underline = s.double_underline;
  subscript = s.subscript;
  superscript = s.superscript;
  strikethrough = s.strikethrough;
  align = s.align;
  linespace = s.linespace;
  spaceBefore = s.spaceBefore;
  spaceAfter = s.spaceAfter;
}

AmiProStyle::AmiProStyle( const AmiProStyle& s )
{
  assign( s );
}

AmiProStyle& AmiProStyle::operator=( const AmiProStyle& s )
{
  assign( s );
  return *this;
}

// base listener for the parser
AmiProListener::AmiProListener()
{
}

AmiProListener::~AmiProListener()
{
}

#define DO_TRUE_DEFINITION(string) \
    bool AmiProListener::string \
    {\
        return true;\
    }

DO_TRUE_DEFINITION(doOpenDocument())
DO_TRUE_DEFINITION(doCloseDocument())
DO_TRUE_DEFINITION(doDefineStyle(const AmiProStyle& style))
DO_TRUE_DEFINITION(doParagraph(const QString& text, AmiProFormatList formatList,
  AmiProLayout& ))
