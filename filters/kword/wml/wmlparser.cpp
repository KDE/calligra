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

#include <QString>
#include <QtXml>
#include <QFile>
#include <q3valuestack.h>

#include <wmlparser.h>

class WMLParseState
{
  public:
    unsigned tableRow, tableCol;
    WMLFormat currentFormat;
    WMLFormatList formatList;
    WMLLayout currentLayout;
    WMLParseState();
    WMLParseState( const WMLParseState& );
    WMLParseState& operator=( const WMLParseState& );
    void assign( const WMLParseState& );
};

WMLParseState::WMLParseState()
{
  tableRow = tableCol = 0;
}

WMLParseState::WMLParseState( const WMLParseState& state )
{
  assign( state );
}

WMLParseState& WMLParseState::operator=( const WMLParseState& state )
{
  assign( state );
  return *this ;
}

void WMLParseState::assign( const WMLParseState& state )
{
  tableRow = state.tableRow;
  tableCol = state.tableCol;
  currentFormat = state.currentFormat;
  formatList = state.formatList;
  currentLayout = state.currentLayout;
}

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

    bool m_inLink;
    QString m_link;
    QString m_href;

    WMLParseState m_state;
    Q3ValueStack<WMLParseState> m_stateStack;
    
    bool flushParagraph();
    void pushState();
    void popState();
};

bool WMLHandler::startDocument()
{
  m_text = "";
  m_inBlock = false;

  m_link = "";
  m_href = "";
  
  return true;
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
    m_state = WMLParseState();
    QString card_id = attr.value("id");
    QString card_title = attr.value("title");
    return m_parser->doOpenCard( card_id, card_title );
  }

  if( tag == "p" )
  {
    m_state.currentLayout = WMLLayout();
    m_inBlock = TRUE;
    if( m_state.currentFormat.bold || 
        m_state.currentFormat.italic ||
        m_state.currentFormat.underline ||
        (m_state.currentFormat.fontsize != WMLFormat::Normal) )
      m_state.formatList.append( m_state.currentFormat );

    QString align = attr.value("align").lower();
    if( align == "right" )
      m_state.currentLayout.align =  WMLLayout::Right;
    if( align == "center" )
      m_state.currentLayout.align =  WMLLayout::Center;

    return TRUE;
  }

  if(( tag == "b" ) || (tag == "strong") )
  {
    m_state.currentFormat.bold = TRUE;
    m_state.currentFormat.pos = m_text.length();
    m_state.formatList.append( m_state.currentFormat );
    return TRUE;
  }
 
  if(( tag == "i" ) || (tag == "em") )
  {  
    m_state.currentFormat.italic = TRUE;
    m_state.currentFormat.pos = m_text.length();  
    m_state.formatList.append( m_state.currentFormat );  
    return TRUE;  
  }  

  if( tag == "u" )
  {
    m_state.currentFormat.underline = TRUE;
    m_state.currentFormat.pos = m_text.length();
    m_state.formatList.append( m_state.currentFormat );
    return TRUE;
  }

  if( tag == "big" )
  {
    m_state.currentFormat.fontsize = WMLFormat::Big;
    m_state.currentFormat.pos = m_text.length();
    m_state.formatList.append( m_state.currentFormat );
    return TRUE;
  }

  if( tag == "small" ) 
  {
    m_state.currentFormat.fontsize = WMLFormat::Small;
    m_state.currentFormat.pos = m_text.length();
    m_state.formatList.append( m_state.currentFormat );
    return TRUE;
  }

  if( tag == "a" )
  {
    QString href = attr.value("href");
    if( !href.isEmpty() )
    {
      m_inBlock = false;
      m_inLink = true;
      m_state.currentFormat.link = "";
      m_state.currentFormat.href = href;
      m_state.currentFormat.pos = m_text.length();
      m_state.currentFormat.len = 1;
      m_text.append( "#" ); // inline char
      return true;
    }
  }

  // open new table
  if( tag == "table" )
  {
    pushState();
    return m_parser->doBeginTable();
  }

  // open table row
  if( tag == "tr" )
  {
    m_state.tableRow++;
    return TRUE;
  }

  // open table cell, keep in sync with <p> above
  if( tag == "td" )
  {
    m_state.tableCol++;
    m_state.currentLayout = WMLLayout();
    m_inBlock = TRUE;
    m_state.formatList.append( m_state.currentFormat );
    return m_parser->doTableCell( m_state.tableRow, m_state.tableCol );
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
    return m_parser->doCloseCard();
  }

  if( tag == "p" )
  {
    m_inBlock = FALSE;
    return flushParagraph();
  }

  if(( tag == "b" ) || (tag == "strong") )
  {
    m_state.currentFormat.bold = FALSE; 
    m_state.currentFormat.pos = m_text.length();
    m_state.formatList.append( m_state.currentFormat );
    return TRUE;
  }

  if(( tag == "i" ) || (tag == "em") )
  {
    m_state.currentFormat.italic = FALSE;
    m_state.currentFormat.pos = m_text.length();
    m_state.formatList.append( m_state.currentFormat );
    return TRUE;
  }

  if( tag == "u" )
  {
    m_state.currentFormat.underline = FALSE;  
    m_state.currentFormat.pos = m_text.length();
    m_state.formatList.append( m_state.currentFormat );
    return TRUE;
  }

  if( tag == "big" )
  {
    m_state.currentFormat.fontsize = WMLFormat::Normal;
    m_state.currentFormat.pos = m_text.length();
    m_state.formatList.append( m_state.currentFormat );
    return TRUE;
  }

  if( tag == "small" )
  {
    m_state.currentFormat.fontsize = WMLFormat::Normal;
    m_state.currentFormat.pos = m_text.length();
    m_state.formatList.append( m_state.currentFormat );
    return TRUE;
  }

  if( tag == "a" )
  {
    m_inBlock = true;
    m_inLink = false;
    m_state.formatList.append( m_state.currentFormat );
    return true;
  }

  // close table
  if( tag == "table" )
  {
    popState();
    return m_parser->doEndTable();
  }

  // close table row
  if( tag == "tr" )
    return TRUE; //skip

  // close table cell, like </p>
  if( tag == "td" )
  {
    m_inBlock = FALSE;
    return flushParagraph();
  }

  // unhandled
  return TRUE;
}

bool WMLHandler::characters( const QString& ch )
{
  if( m_inBlock )
    m_text.append( ch );

  if( m_inLink )
    m_state.currentFormat.link.append( ch );

  return TRUE;
}

bool WMLHandler::flushParagraph()
{
  // calc length of each format tag
  for( unsigned i=0; i<m_state.formatList.count(); i++ )
  {
    int nextpos;
    WMLFormat& format = m_state.formatList[i];
    if( i < m_state.formatList.count()-1 )
    {
      WMLFormat& nextformat = m_state.formatList[i+1];
      nextpos = nextformat.pos;
    }
    else nextpos = m_text.length();
    if( format.len <= 0 )
      format.len = nextpos - format.pos;
  }

  bool result = m_parser->doParagraph( m_text, m_state.formatList, m_state.currentLayout );

  // ready for next paragraph
  m_text = "";
  m_state.formatList.clear();
  m_state.currentLayout = WMLLayout();

  // m_state.currentFormat = WMLFormat();
  // FIXME should we reset formatting ?

  return result;
}

void WMLHandler::pushState()
{
  m_stateStack.push( m_state );
}

void WMLHandler::popState()
{
  if( !m_stateStack.isEmpty() )
    m_state = m_stateStack.pop();
}

// formatting for the text
WMLFormat::WMLFormat()
{
  pos = len = 0;
  fontsize = Normal;
  bold = italic = underline = FALSE;
  link = "";
  href = "";
}

void WMLFormat::assign( const WMLFormat& f )
{
  pos = f.pos;
  len = f.len;
  bold = f.bold;
  italic = f.italic;
  underline = f.underline;
  fontsize = f.fontsize;
  link = f.link;
  href= f.href;
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

// paragraph layout info
WMLLayout::WMLLayout()
{
  align = Left;
}

void WMLLayout::assign( const WMLLayout& l )
{
  align = l.align;
}

WMLLayout::WMLLayout( const WMLLayout& l )
{
  assign( l );
}

WMLLayout& WMLLayout::operator=( const WMLLayout& l )
{
  assign( l );
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

bool WMLParser::doParagraph( QString, WMLFormatList, WMLLayout )
{
  return TRUE;
}

bool WMLParser::doBeginTable()
{
  return TRUE;
}

bool WMLParser::doEndTable()
{
  return TRUE;
}

bool WMLParser::doTableCell( unsigned, unsigned )
{
  return TRUE;
}
