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

#include <qstring.h>
#include <qxml.h>
#include <qfile.h>

#include "wmlparser.h"

// ContentHandler for use with the reader
class WMLHandler: public QXmlDefaultHandler
{
  public:

    WMLHandler( WMLParser *parser ){ m_parser = parser; }

    bool startDocument();
    bool startElement( const QString&, const QString&, const QString& ,
                       const QXmlAttributes& );
    bool endElement( const QString&, const QString&, const QString& );
    bool characters( const QString& ch );

  private:

    WMLParser *m_parser;

    bool m_inBlock;
    QString m_text;

    WMLFormat m_currentFormat;
    WMLFormatList m_formatList;
    
    bool flushParagraph();
};

bool WMLHandler::startDocument()
{
  m_text = "";
  m_inBlock = FALSE;
  return TRUE;
}

bool WMLHandler::startElement( const QString&, const QString&,
                                        const QString& qName,
                                        const QXmlAttributes& attr )
{
  QString tag = qName.lower();

  if( tag == "wml" )
    return m_parser->doOpenDocument();

  if( tag == "card" ) 
  {
    QString card_id = attr.value("id");
    QString card_title = attr.value("title");
    return m_parser->doOpenCard( card_id, card_title );
  }

  if( tag == "p" )
  {
    m_inBlock = TRUE;
    if( m_currentFormat.bold || 
        m_currentFormat.italic ||
        m_currentFormat.underline )
      m_formatList.append( m_currentFormat );
    return TRUE;
  }

  if(( tag == "b" ) || (tag == "strong") )
  {
    m_currentFormat.bold = TRUE;
    m_currentFormat.pos = m_text.length();
    m_formatList.append( m_currentFormat );
    return TRUE;
  }
 
  if(( tag == "i" ) || (tag == "em") )
  {  
    m_currentFormat.italic = TRUE;
    m_currentFormat.pos = m_text.length();  
    m_formatList.append( m_currentFormat );  
    return TRUE;  
  }  

  if( tag == "u" )
  {
    m_currentFormat.underline = TRUE;
    m_currentFormat.pos = m_text.length();
    m_formatList.append( m_currentFormat );
    return TRUE;
  }

  // unhandled element
  return TRUE;
}

bool WMLHandler::endElement( const QString&, const QString&, 
  const QString& qName )
{
  QString tag = qName.lower();

  if( tag == "wml" )
    return m_parser->doCloseDocument();

  if( tag == "card" )
  {
    // forget </p> before </card> ?
    m_inBlock = FALSE;
    if( !m_text.isEmpty() )
      flushParagraph();

    // force to reset formatting
    m_currentFormat = WMLFormat();

    return m_parser->doCloseCard();
  }

  if( tag == "p" )
  {
    m_inBlock = FALSE;
    return flushParagraph();
  }

  if(( tag == "b" ) || (tag == "strong") )
  {
    m_currentFormat.bold = FALSE; 
    m_currentFormat.pos = m_text.length();
    m_formatList.append( m_currentFormat );
    return TRUE;
  }

  if(( tag == "i" ) || (tag == "em") )
  {
    m_currentFormat.italic = FALSE;
    m_currentFormat.pos = m_text.length();
    m_formatList.append( m_currentFormat );
    return TRUE;
  }

  if( tag == "u" )
  {
    m_currentFormat.underline = FALSE;  
    m_currentFormat.pos = m_text.length();
    m_formatList.append( m_currentFormat );
    return TRUE;
  }

  // unhandled
  return TRUE;
}

bool WMLHandler::characters( const QString& ch )
{
  if( m_inBlock )
    m_text.append( ch );

  return TRUE;
}

bool WMLHandler::flushParagraph()
{
  // calc length of each format tag
  for( unsigned i=0; i<m_formatList.count(); i++ )
  {
    int nextpos;
    WMLFormat& format = m_formatList[i];
    if( i < m_formatList.count()-1 )
    {
      WMLFormat& nextformat = m_formatList[i+1];
      nextpos = nextformat.pos;
    }
    else nextpos = m_text.length();
    format.len = nextpos - format.pos;
  }

  bool result = m_parser->doParagraph( m_text, m_formatList );

  // ready for next paragraph
  m_text = "";
  m_formatList.clear();

  // m_currentFormat = WMLFormat();
  // FIXME should we reset formatting ?

  return result;
}

// formatting for the text
WMLFormat::WMLFormat()
{
  pos = len = 0;
  bold = italic = underline = FALSE;
}

void WMLFormat::assign( const WMLFormat& f )
{
  pos = f.pos;
  len = f.len;
  bold = f.bold;
  italic = f.italic;
  underline = f.underline;
}

WMLFormat::WMLFormat( const WMLFormat& f )
{
  assign( f );
}

WMLFormat& WMLFormat::operator=( const WMLFormat& f )
{
  assign( f );
  return *this;
}

// The basic WML parser
void WMLParser::parse( const char* filename )
{
   QFile f( filename );
   QXmlInputSource source( &f );
   QXmlSimpleReader reader;
   WMLHandler handler( this );
   reader.setContentHandler( &handler );
   reader.parse( source );
}

bool WMLParser::doOpenDocument()
{
  return TRUE;
}

bool WMLParser::doCloseDocument()
{
  return TRUE;
}

bool WMLParser::doOpenCard( QString, QString )
{
  return TRUE;
}

bool WMLParser::doCloseCard()
{
  return TRUE;
}

bool WMLParser::doParagraph( QString text, WMLFormatList formatList )
{
  return TRUE;
}
