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

#include <config.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <qregexp.h>
#include <qfileinfo.h>
#include <qvaluelist.h>

#include <kdebug.h>
#include <koFilterChain.h>
#include <kgenericfactory.h>

#include <amiproimport.h>
#include <amiproparser.h>

typedef KGenericFactory<AmiProImport, KoFilter> AmiProImportFactory;
K_EXPORT_COMPONENT_FACTORY( libamiproimport, AmiProImportFactory( "amiproimport" ) );

AmiProImport::AmiProImport( KoFilter *, const char *, const QStringList& ):
                     KoFilter()
{
}

class AmiProConverter: public AmiProListener
{
  public:
    AmiProConverter();
    QString root, documentInfo;
    virtual bool doOpenDocument();
    virtual bool doCloseDocument();
    virtual bool doParagraph( const QString& text, AmiProFormatList formatList );
};

static QString AmiProFormatAsXML( AmiProFormat format )
{
  QString result;

  if( format.bold ) result.append("<WEIGHT value=\"75\" />\n" );
  if( format.italic ) result.append( "<ITALIC value=\"1\" />\n" );
  if( format.underline ) result.append( "<UNDERLINE value=\"1\" />\n" );
  if( format.strikethrough ) result.append( "    <STRIKEOUT value=\"1\" />\n" );
  if( format.subscript ) result.append( "<VERTALIGN value=\"1\" />\n" );
  if( format.superscript ) result.append( "<VERTALIGN value=\"2\" />\n" );

  // both word underline and double underline still treat as underline
  // just wait until KWord has these features
  if( format.word_underline ) result.append( "<UNDERLINE value=\"1\" />\n" );
  if( format.double_underline ) result.append( "<UNDERLINE value=\"1\" />\n" );

  result.prepend("<FORMAT id=\"1\" pos=\"" + QString::number(format.pos) +
    "\" len=\"" + QString::number(format.len) + "\">\n");
  result.append( "</FORMAT>\n" );

  return result;
}

AmiProConverter::AmiProConverter()
{
  root = "";
}

bool AmiProConverter::doOpenDocument()
{
  QString prolog = "<!DOCTYPE DOC>\n";

  prolog.append( "<DOC mime=\"application/x-kword\" syntaxVersion=\"2\" editor=\"KWord\">\n");
  prolog.append( "<PAPER width=\"595\" height=\"841\" format=\"1\" fType=\"0\" orientation=\"0\" hType=\"0\" columns=\"1\">\n" );
  prolog.append( " <PAPERBORDERS left=\"36\" right=\"36\" top=\"36\" bottom=\"36\" />\n" );
  prolog.append( "</PAPER>\n" );
  prolog.append( "<ATTRIBUTES standardpage=\"1\" unit=\"mm\" hasFooter=\"0\" hasHeader=\"0\" processing=\"0\" />\n" );
  prolog.append( "<FRAMESETS>\n" );
  prolog.append( "<FRAMESET removable=\"0\" frameType=\"1\" frameInfo=\"0\" autoCreateNewFrame=\"1\">\n" );
  prolog.append( "<FRAME right=\"567\" left=\"28\" top=\"42\" bottom=\"799\" />\n" );
  root = prolog;

  return true;
}

bool AmiProConverter::doCloseDocument()
{
  QString epilog = "</FRAMESET>\n";
  epilog.append( "</FRAMESETS>\n" );
  epilog.append( "</DOC>\n" );

  root.append( epilog );

  return true;
}

bool AmiProConverter::doParagraph( const QString& _text, AmiProFormatList formatList )
{
  QString text, formats, layout, result;

  // encode text for XML-ness
  // FIXME could be faster without QRegExp
  text = _text;
  text.replace( QRegExp("&"), "&amp;" );
  text.replace( QRegExp("<"), "&lt;" );
  text.replace( QRegExp(">"), "&gt;" );
  text.replace( QRegExp("\""), "&quot;" );
  text.replace( QRegExp("'"), "&apos;" );

  // formats, taken from formatList
  AmiProFormatList::iterator it;
  for( it=formatList.begin(); it!=formatList.end(); ++it )
  {
    AmiProFormat& format = *it;
    formats.append( AmiProFormatAsXML(format) );
  }

  // default LAYOUT (mostly still hard-coded)
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
  layout.append( "    <SIZE value=\"12\" />\n" );
  layout.append( "    <WEIGHT value=\"50\" />\n" );
  layout.append( "    <ITALIC value=\"0\" />\n" );
  layout.append( "    <UNDERLINE value=\"0\" />\n" );
  layout.append( "    <STRIKEOUT value=\"0\" />\n" );
  layout.append( "    <CHARSET value=\"0\" />\n" );
  layout.append( "    <VERTALIGN value=\"0\" />\n" );
  layout.append( "    <FONT name=\"Helvetica\" />\n" );
  layout.append( "  </FORMAT>\n" );
  layout.append( "</LAYOUT>\n" );

  // assemble
  root.append( "<PARAGRAPH>\n" );
  root.append( "<TEXT>" + text + "</TEXT>\n" );
  root.append( "<FORMATS>" + formats + "</FORMATS>\n" );
  root.append( layout );
  root.append( "</PARAGRAPH>\n" );
 
  return true;
}

KoFilter::ConversionStatus AmiProImport::convert( const QCString& from, const QCString& to )
{
  // check for proper conversion
  if( to!= "application/x-kword" || from != "application/x-amipro" )
     return KoFilter::NotImplemented;

  // parse/convert input file
  AmiProParser *parser = new AmiProParser;
  AmiProConverter *converter = new AmiProConverter;
  parser->setListener( converter );

  parser->process( m_chain->inputFile().latin1() );

  if( converter->root.isEmpty() )
    return KoFilter::StupidError;

  QString root = converter->root;
  QString documentInfo = converter->documentInfo;

  delete converter;
  delete parser;

  // prepare storage
  KoStoreDevice* out=m_chain->storageFile( "root", KoStore::Write );

  // store output document
  if( out )
    {
      QCString cstring = root.utf8();
      cstring.prepend( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" );
      out->writeBlock( (const char*) cstring, cstring.length() );
    }

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

#include "amiproimport.moc"
