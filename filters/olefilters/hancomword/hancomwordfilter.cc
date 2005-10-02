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

#include <hancomwordfilter.h>
#include <qfont.h>
#include <qstring.h>
#include <qtextstream.h>

#include <filterbase.h>
#include <koGlobal.h>

// helper function to escape string for XML-ness
static QString XMLEscape( const QString& str )
{
  QString result; 

  for( unsigned i=0; i<str.length(); i++ )
    if( str[i] == '&' ) result += "&amp;";
    else if( str[i] == '<' ) result += "&lt;"; 
    else if( str[i] == '>' ) result += "&gt;"; 
    else if( str[i] == '"' ) result += "&quot;";
    else if( str[i] == QChar(39) ) result += "&apos;";
    else result += str[i]; 

  return result; 
}

HancomWordFilter::HancomWordFilter( const QByteArray &prvText ):
        FilterBase()
{
  // still a nasty hack, HancomWord uses UCS4 (UTF32) but we pretend it's UCS2 (utf16)
  // well, not much char > 65536 anyway at the moment :-)
  QTextStream stream( prvText, IO_ReadOnly );
  stream.setEncoding( QTextStream::Unicode );
  m_text = stream.read();
}

HancomWordFilter::~HancomWordFilter()
{
}

bool HancomWordFilter::filter()
{
    return !m_text.isNull();
}

static QString processPlainParagraph( QString text )
{
  QString formats, layout, result;

  // specify FORMAT (just empty element)
  formats.append ( "  <FORMAT id=\"1\" pos=\"0\" len=\"" +
    QString::number( text.length() )+ "\">\n" );
  formats.append ( "  </FORMAT>\n" );

  QFont font = KoGlobal::defaultFont();
  QString fontFamily = font.family();
  double fontSize = font.pointSizeFloat();

  // default LAYOUT
  layout.append( "<LAYOUT>\n" );
  layout.append( "  <NAME value=\"Standard\" />\n" );
  layout.append( "  <FLOW align=\"left\" />\n" );
  layout.append( "  <LINESPACING value=\"0\" />\n" );
  layout.append( "  <LEFTBORDER width=\"0\" style=\"0\" />\n" );
  layout.append( "  <RIGHTBORDER width=\"0\" style=\"0\" />\n" );
  layout.append( "  <TOPBORDER width=\"0\" style=\"0\" />\n" );
  layout.append( "  <BOTTOMBORDER width=\"0\" style=\"0\" />\n" );
  layout.append( "  <INDENTS />\n" );
  layout.append( "  <OFFSETS />\n" );
  layout.append( "  <PAGEBREAKING />\n" );
  layout.append( "  <COUNTER />\n" );
  layout.append( "  <FORMAT id=\"1\">\n" );
  layout.append( "    <SIZE value=\"" + QString::number( fontSize ) + "\" />\n" );
  layout.append( "    <WEIGHT value=\"50\" />\n" );
  layout.append( "    <ITALIC value=\"0\" />\n" );
  layout.append( "    <UNDERLINE value=\"0\" />\n" );
  layout.append( "    <STRIKEOUT value=\"0\" />\n" );
  layout.append( "    <CHARSET value=\"0\" />\n" );
  layout.append( "    <VERTALIGN value=\"0\" />\n" );
  layout.append( "    <FONT name=\"" + fontFamily + "\" />\n" );
  layout.append( "  </FORMAT>\n" );
  layout.append( "</LAYOUT>\n" );

  // construct the <PARAGRAPH>
  result.append( "<PARAGRAPH>\n" );
  result.append( "<TEXT>" + XMLEscape( text ) + "</TEXT>\n" );
  result.append( "<FORMATS>\n");
  result.append( formats );
  result.append( "</FORMATS>\n");
  result.append( layout );
  result.append( "</PARAGRAPH>\n" );

  return result;
}

static QString processPlainDocument( QString plaindoc )
{
  QString prolog, content, epilog;
  QStringList paragraphs;

  paragraphs = QStringList::split( "\n", plaindoc, TRUE );
  for( int i = 0; i < paragraphs.count(); i++ )
  {
      QString text = paragraphs[i];
      text.replace( '\r', ' ' );
      content.append( processPlainParagraph( text ) );
  }

  prolog = "<!DOCTYPE DOC>\n";
  prolog.append( "<DOC mime=\"application/x-kword\" syntaxVersion=\"2\" editor=\"KWord\">\n");

  prolog.append( "<PAPER width=\"595\" height=\"841\" format=\"1\" fType=\"0\" orientation=\"0\" hType=\"0\" columns=\"1\">\n" );
  prolog.append( " <PAPERBORDERS left=\"36\" right=\"36\" top=\"36\" bottom=\"36\" />\n" );
  prolog.append( "</PAPER>\n" );

  prolog.append( "<ATTRIBUTES standardpage=\"1\" unit=\"mm\" hasFooter=\"0\" hasHeader=\"0\" processing=\"0\" />\n" );

  prolog.append( "<FRAMESETS>\n" );
  prolog.append( "<FRAMESET removable=\"0\" frameType=\"1\" frameInfo=\"0\" autoCreateNewFrame=\"1\">\n" );
  prolog.append( "<FRAME right=\"567\" left=\"28\" top=\"42\" bottom=\"799\" />\n" );

  epilog = "</FRAMESET>\n";
  epilog.append( "</FRAMESETS>\n" );
  epilog.append( "</DOC>\n" );

  return prolog + content + epilog;
}

// still not well-formatted
QCString HancomWordFilter::CString() const
{
  QCString cstring = processPlainDocument( m_text ).utf8();
  cstring.prepend( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" );
  return cstring;
}

#include <hancomwordfilter.moc>
