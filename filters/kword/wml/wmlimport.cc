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

#include <config.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <qfileinfo.h>
#include <qvaluelist.h>
#include <qfont.h>

#include <kdebug.h>
#include <KoFilterChain.h>
#include <kgenericfactory.h>
#include <KoGlobal.h>

#include <wmlimport.h>
#include <wmlparser.h>

typedef KGenericFactory<WMLImport, KoFilter> WMLImportFactory;
K_EXPORT_COMPONENT_FACTORY( libwmlimport, WMLImportFactory( "kofficefilters" ) )

WMLImport::WMLImport( KoFilter *, const char *, const QStringList& ):
                     KoFilter()
{
}

// converts WML to KWord document
class WMLConverter: public WMLParser
{
  public:
    QString root;
    QString documentInfo;
    WMLConverter();
    virtual void parse( const char* filename );
    virtual bool doOpenCard( QString, QString );
    virtual bool doCloseCard();
    virtual bool doParagraph( QString text, WMLFormatList formatList,
      WMLLayout layout );
  private:
    QString m_title;
};

WMLConverter::WMLConverter()
{
  root = "";
}

static QString WMLFormatAsXML( WMLFormat format )
{
  QString result;

  if( format.href.isEmpty() )
  {
    QFont font = KoGlobal::defaultFont();
    QString fontFamily = font.family();
    QString fontSize = QString::number( 
      format.fontsize == WMLFormat::Big ? font.pointSizeFloat()+3 :
      format.fontsize == WMLFormat::Small ? font.pointSizeFloat()-3 : font.pointSizeFloat() );
    QString boldness = format.bold ? "75" : "50";
    QString italic = format.italic ? "1" : "0";
    QString underline = format.underline ? "1" : "0";

    result = "<FORMAT id=\"1\" pos=\"" + QString::number(format.pos) +
       "\" len=\"" + QString::number(format.len) + "\">\n";
    result.append( "  <FONT name=\"" + fontFamily + "\" />\n" );
    result.append( "  <SIZE value=\"" + fontSize + "\" />\n" );
    result.append( "  <WEIGHT value=\"" + boldness + "\" />\n" );
    result.append( "  <ITALIC value=\"" + italic  + "\" />\n" );
    result.append( "  <UNDERLINE value=\"" + underline + "\" />\n" );
    result.append( "</FORMAT>\n" );
  }
  else
  {
    // hyperlink
    result.append( "<FORMAT id=\"4\" pos=\"" + QString::number(format.pos) +
     "\" len=\"" + QString::number(format.len) + "\">\n");
    result.append( "<VARIABLE>\n" );
    result.append( "  <TYPE key=\"STRING\" type=\"9\" text=\"" + format.link + "\" />\n" );
    result.append( "  <LINK linkName=\"" + format.link + "\" hrefName=\"" + 
      format.href + "\" />\n" );
    result.append( "</VARIABLE>\n" );
    result.append( "</FORMAT>\n" );
  }


  return result;
}

static QString WMLLayoutAsXML( WMLLayout layout )
{
  QString result;

  QString align = "left";
  if( layout.align == WMLLayout::Center ) align = "center";
  if( layout.align == WMLLayout::Right ) align = "right";

  QFont font = KoGlobal::defaultFont();
  QString fontFamily = font.family();
  QString fontSize = QString::number( font.pointSizeFloat() );

  result.append( "<LAYOUT>\n" );
  result.append( "  <NAME value=\"Standard\" />\n" );
  result.append( "  <FLOW align=\"" + align + "\" />\n" );
  result.append( "  <LINESPACING value=\"0\" />\n" );
  result.append( "  <LEFTBORDER width=\"0\" style=\"0\" />\n" );
  result.append( "  <RIGHTBORDER width=\"0\" style=\"0\" />\n" );
  result.append( "  <TOPBORDER width=\"0\" style=\"0\" />\n" );
  result.append( "  <BOTTOMBORDER width=\"0\" style=\"0\" />\n" );
  result.append( "  <INDENTS />\n" );
  result.append( "  <OFFSETS />\n" );
  result.append( "  <PAGEBREAKING />\n" );
  result.append( "  <COUNTER />\n" );
  result.append( "  <FORMAT id=\"1\">\n" );
  result.append( "    <WEIGHT value=\"50\" />\n" );
  result.append( "    <ITALIC value=\"0\" />\n" );
  result.append( "    <UNDERLINE value=\"0\" />\n" );
  result.append( "    <STRIKEOUT value=\"0\" />\n" );
  result.append( "    <CHARSET value=\"0\" />\n" );
  result.append( "    <VERTALIGN value=\"0\" />\n" );
  result.append( "    <FONT name=\"" + fontFamily + "\" />\n" );
  result.append( "    <SIZE value=\"" + fontSize + "\" />\n" );
  result.append( "  </FORMAT>\n" );
  result.append( "</LAYOUT>\n" );

  return result;
}

// use the first card title (or id) as document title
bool WMLConverter::doOpenCard( QString id, QString title )
{
  if( m_title.isEmpty() )
    m_title = ( !title.isEmpty() ) ? title : id;

  return TRUE;
}

// FIXME is this right ?
bool WMLConverter::doCloseCard()
{
  // add extra paragraph between cards
  return doParagraph( " ", WMLFormatList(), WMLLayout() );
}

bool WMLConverter::doParagraph( QString atext, WMLFormatList formatList,
  WMLLayout layout  )
{
  QString text, formats;

  // encode the text for XML-ness
  text = atext;
  text.replace( '&', "&amp;" );
  text.replace( '<', "&lt;" );
  text.replace( '>', "&gt;" );

  // formats, taken from formatList
  WMLFormatList::iterator it;
  for( it=formatList.begin(); it!=formatList.end(); ++it )
  {
    WMLFormat& format = *it;
    formats.append( WMLFormatAsXML(format) );
  }

  // assemble
  root.append( "<PARAGRAPH>\n" );
  root.append( "<TEXT>" + text + "</TEXT>\n" );
  root.append( "<FORMATS>" + formats + "</FORMATS>\n" );
  root.append( WMLLayoutAsXML( layout) );
  root.append( "</PARAGRAPH>\n" );

  return TRUE;
}

void WMLConverter::parse( const char* filename )
{
  WMLParser::parse( filename );

  QString prolog;
  prolog += "<!DOCTYPE DOC>\n";
  prolog += "<DOC mime=\"application/x-kword\" syntaxVersion=\"2\" editor=\"KWord\" >\n";
  prolog += "<PAPER width=\"595\" spHeadBody=\"9\" format=\"1\" height=\"841\" fType=\"0\" orientation=\"0\" hType=\"0\" columnspacing=\"2\" spFootBody=\"9\" columns=\"1\" >\n";
  prolog += "<PAPERBORDERS right=\"28\" left=\"28\" bottom=\"42\" top=\"42\" />\n";
  prolog += "</PAPER>\n";
  prolog += "<ATTRIBUTES hasTOC=\"0\" standardpage=\"1\" hasFooter=\"0\" hasHeader=\"0\" processing=\"0\" />\n";
  prolog += "<FRAMESETS>\n";
  prolog += "<FRAMESET frameType=\"1\" frameInfo=\"0\" name=\"Text Frameset 1\" visible=\"1\" >\n";
   prolog += "<FRAME runaround=\"1\" copy=\"0\" right=\"567\" newFrameBehavior=\"0\" left=\"28\" bottom=\"799\" runaroundGap=\"2\" top=\"42\" />\n";

  QString epilog;
  epilog = "</FRAMESET>\n";
  epilog += "</FRAMESETS>\n";
  epilog += "</DOC>\n";

  root.prepend( prolog );
  root.append( epilog );

  // document information (only title though)
  documentInfo = "<!DOCTYPE document-info>\n";
  documentInfo += "<document-info>\n";
  documentInfo += "<log><text></text></log>\n";
  documentInfo += "<author>\n";
  documentInfo += "<full-name></full-name>\n";
  documentInfo += "<title></title>\n";
  documentInfo += "<company></company>\n";
  documentInfo += "<email></email>\n";
  documentInfo += "<telephone></telephone>\n";
  documentInfo += "</author>\n";
  documentInfo += "<about>\n";
  documentInfo += "<abstract></abstract>\n";
  documentInfo += "<title>" + m_title + "</title>\n";
  documentInfo += "</about>\n";
  documentInfo += "</document-info>";

}

KoFilter::ConversionStatus WMLImport::convert( const QCString& from, const QCString& to )
{
  // check for proper conversion
  if( to!= "application/x-kword" || from != "text/vnd.wap.wml" )
     return KoFilter::NotImplemented;

  // parse/convert input file
  WMLConverter filter;
  filter.parse( m_chain->inputFile().latin1() );

  // check for error
  // FIXME better error handling/reporting
  if( filter.root.isEmpty() )
    return KoFilter::StupidError;

  QString root = filter.root;

  // prepare storage
  KoStoreDevice* out=m_chain->storageFile( "root", KoStore::Write );

  // store output document
  if( out )
    {
      QCString cstring = root.utf8();
      cstring.prepend( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" );
      out->writeBlock( (const char*) cstring, cstring.length() );
    }


  QString documentInfo = filter.documentInfo;

  // store document info
  out = m_chain->storageFile( "documentinfo.xml", KoStore::Write );
  if ( out )
    {
       QCString cstring = documentInfo.utf8();
       cstring.prepend( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" );

       out->writeBlock( (const char*) cstring, cstring.length() );
     }

  return KoFilter::OK;
}

#include "wmlimport.moc"
