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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "amiproparser.h"

#include <qfile.h>
#include <qstring.h>
#include <qtextstream.h>

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

  // skip the header, look for "[edoc]"
  while( !line.isNull() )
  {
    line = stream.readLine();
    if( line == "[edoc]" ) break;
  }

  // no edoc ?
  if( line.isNull() ) return setResult( InvalidFormat );

  processOpenDocument();

  // now process the main document body
  QString partext;
  for(;;) 
  {
    line = stream.readLine();
    if( line.isNull()) break;

    if( !line.isEmpty() )
       if( line[0] == '>' ) break;

    if( line.isEmpty() )
    {
        parseParagraph( partext );
        partext = "";
    }
    else
    {
        partext.append( line );
    }
  }

  if( !partext.isEmpty() )
    parseParagraph( partext );

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

bool AmiProParser::parseParagraph( const QString& partext )
{
  m_text = "";
  m_formatList.clear();

  for( unsigned i=0; i<partext.length(); i++ )
  {
    QChar ch = partext[i];
   
    // handle a tag
    if( ch == '<' )
    {

      // check for <<, which is decoded as <, not tag
      if( i+1 < partext.length() )
        if( partext[i+1] == '<' )
        {
          m_text.append( '<' );
          i++;
        }

      else
      {
        // this a tag, enclosed with >
        QString tag = "";
        for( i++; (i < partext.length()) && 
           (partext[i] != '>'); i++) tag.append( partext[i] );
        handleTag( tag );
      }

    }

    else

    // handle style change
    if( ch == '@' )
      for( i++; (i < partext.length()) && 
         (partext[i] != '@'); i++);

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
    return m_listener->doParagraph( m_text, m_formatList );

  return true;
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

  return true;
}

// text formatting
AmiProFormat::AmiProFormat()
{
  pos = len = 0;
  bold = italic = underline = 
  word_underline = double_underline = 
  subscript = superscript = strikethrough = FALSE;
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
DO_TRUE_DEFINITION(doParagraph(const QString& text, AmiProFormatList formatList))
