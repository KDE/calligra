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

#include <QRegExp>
#include <QFileInfo>
#include <q3valuelist.h>
#include <QFont>
//Added by qt3to4:
#include <QByteArray>

#include <kdebug.h>
#include <KoFilterChain.h>
#include <kgenericfactory.h>
#include <KoGlobal.h>

#include <amiproimport.h>
#include <amiproparser.h>

typedef KGenericFactory<AmiProImport> AmiProImportFactory;
K_EXPORT_COMPONENT_FACTORY( libamiproimport, AmiProImportFactory( "kofficefilters" ) )

AmiProImport::AmiProImport( QObject* parent, const QStringList& ):
                     KoFilter(parent)
{
}

class AmiProConverter: public AmiProListener
{
  public:
    AmiProConverter();
    QString root, documentInfo;
    virtual bool doOpenDocument();
    virtual bool doCloseDocument();
    virtual bool doDefineStyle( const AmiProStyle& style );
    virtual bool doParagraph( const QString& text, AmiProFormatList formatList,
      AmiProLayout& l );
  private:
    AmiProStyleList styleList;
};

// helper function to escape string for XML-ness
static QString XMLEscape( const QString& str )
{
  QString result;

  for( int i=0; i<str.length(); i++ )
    if( str[i] == '&' ) result += "&amp;";
    else if( str[i] == '<' ) result += "&lt;";
    else if( str[i] == '>' ) result += "&gt;";
    else if( str[i] == '"' ) result += "&quot;";
    else if( str[i] == QChar(39) ) result += "&apos;";
    else result += str[i];

  return result;
}

// helper function to convert AmiPro format to KWord's FORMAT
static QString AmiProFormatAsXML( AmiProFormat format )
{
  QString result;

  QString fontname = format.fontFamily;
  if( fontname.isEmpty() ) fontname = KoGlobal::defaultFont().family();
  QString fontsize = QString::number( format.fontSize );
  QString boldness = format.bold ? "75" : "50";
  QString italic = format.italic ? "1" : "0";
  QString strikeout = format.strikethrough ? "1" : "0";
  QString vertalign = format.superscript ? "2" : format.subscript ? "1" : "0";
  QString underline = format.double_underline ? "double" :
    format.underline|format.word_underline ? "1" : "0";

  result = "<FORMAT id=\"1\" pos=\"" + QString::number(format.pos) +
     "\" len=\"" + QString::number(format.len) + "\">\n";
  result.append( "  <FONT name=\"" + fontname + "\" />\n" );
  result.append( "  <SIZE value=\"" + fontsize + "\" />\n" );
  result.append( "  <WEIGHT value=\"" + boldness + "\" />\n" );
  result.append( "  <ITALIC value=\"" + italic  + "\" />\n" );
  result.append( "  <STRIKEOUT value=\"" + strikeout + "\" />\n" );
  result.append( "  <VERTALIGN value=\"" + vertalign + "\" />\n" );
  result.append( "  <UNDERLINE value=\"" + underline + "\" />\n" );
  result.append( "</FORMAT>\n" );

  return result;
}

// helper function to convert AmiPro list of formats to KWord FORMATS
static QString AmiProFormatListAsXML( AmiProFormatList& formatList )
{
  QString result;

  AmiProFormatList::iterator it;
  for( it=formatList.begin(); it!=formatList.end(); ++it )
  {
    AmiProFormat& format = *it;
    result.append( AmiProFormatAsXML(format) );
  }

  if( !result.isEmpty() )
  {
    result.prepend( "<FORMATS>\n" );
    result.append( "</FORMATS>\n" );
  }

  return result;
}

// helper function to convert AmiPro paragraph layout to KWord's LAYOUT
static QString AmiProLayoutAsXML( const AmiProLayout& layout )
{
  QString result;

  QString referredStyle = layout.name;
  if( referredStyle.isEmpty() ) referredStyle = "Standard";

  QString fontname = layout.fontFamily;
  if( fontname.isEmpty() ) fontname = KoGlobal::defaultFont().family();
  QString fontsize = QString::number( layout.fontSize );
  QString fontcolor = "red=\"" + QString::number( layout.fontColor.red() ) +
    "\"  green=\"" +  QString::number( layout.fontColor.green() ) +
    "\"  blue=\"" + QString::number( layout.fontColor.blue() ) + "\"";
  QString boldness = layout.bold ? "75" : "50";
  QString italic = layout.italic ? "1" : "0";
  QString strikeout = layout.strikethrough ? "1" : "0";
  QString vertalign = layout.superscript ? "2" : layout.subscript ? "1" : "0";
  QString underline = layout.double_underline ? "double" :
    layout.underline|layout.word_underline ? "1" : "0";

  QString align;
  align = layout.align==Qt::AlignLeft ? "left" :
          layout.align==Qt::AlignRight ? "right" :
          layout.align==Qt::AlignCenter ? "center" :
          layout.align==Qt::AlignJustify ? "justify" :
          "left";

  QString offsets;
  offsets = "before=\"" + QString::number(layout.spaceBefore) +
            "\" after=\"" + QString::number(layout.spaceAfter) + "\"";

  QString linespacing;
  linespacing = layout.linespace==AmiPro::LS_Single ? QString::fromLatin1( "0" ) :
                layout.linespace==AmiPro::LS_OneAndHalf ? QString::fromLatin1( "oneandhalf" ) :
                layout.linespace==AmiPro::LS_Double ? QString::fromLatin1( "double" ) :
                  QString::number( layout.linespace );

  result.append( "<LAYOUT>\n" );
  result.append( "  <NAME value=\"" + XMLEscape( referredStyle ) + "\" />\n" );
  result.append( "  <FLOW align=\"" + align + "\" />\n" );
  result.append( "  <LINESPACING value=\"" + linespacing + "\" />\n" );
  result.append( "  <OFFSETS " + offsets + " />\n" );
  result.append( "  <LEFTBORDER width=\"0\" style=\"0\" />\n" );
  result.append( "  <RIGHTBORDER width=\"0\" style=\"0\" />\n" );
  result.append( "  <TOPBORDER width=\"0\" style=\"0\" />\n" );
  result.append( "  <BOTTOMBORDER width=\"0\" style=\"0\" />\n" );
  result.append( "  <INDENTS />\n" );
  result.append( "  <OFFSETS />\n" );
  result.append( "  <PAGEBREAKING />\n" );
  result.append( "  <COUNTER />\n" );
  result.append( "  <FORMAT id=\"1\">\n" );
  result.append( "    <FONT name=\"" + fontname + "\" />\n" );
  result.append( "    <SIZE value=\"" + fontsize + "\" />\n" );
  result.append( "    <COLOR " + fontcolor + " />\n" );
  result.append( "    <WEIGHT value=\"" + boldness + "\" />\n" );
  result.append( "    <ITALIC value=\"" + italic  + "\" />\n" );
  result.append( "    <STRIKEOUT value=\"" + strikeout + "\" />\n" );
  result.append( "    <VERTALIGN value=\"" + vertalign + "\" />\n" );
  result.append( "    <UNDERLINE value=\"" + underline + "\" />\n" );
  result.append( "  </FORMAT>\n" );
  result.append( "</LAYOUT>\n" );

  return result;
}

// helper function to convert AmiPro style to KWord STYLE
static QString AmiProStyleAsXML( const AmiProStyle& style )
{
  QString result;

  QString fontname = style.fontFamily;
  if( fontname.isEmpty() ) fontname = KoGlobal::defaultFont().family();
  QString fontsize = QString::number( style.fontSize );
  QString boldness = style.bold ? "75" : "50";
  QString italic = style.italic ? "1" : "0";
  QString strikeout = style.strikethrough ? "1" : "0";
  QString vertalign = style.superscript ? "2" : style.subscript ? "1" : "0";
  QString underline = style.double_underline ? "double" :
    style.underline|style.word_underline ? "1" : "0";

  QString align;
  align = style.align==Qt::AlignLeft ? "left" :
          style.align==Qt::AlignRight ? "right" :
          style.align==Qt::AlignCenter ? "center" :
          style.align==Qt::AlignJustify ? "justify" :
          "left";

  QString linespacing;
  linespacing = style.linespace==AmiPro::LS_Single ? QString::fromLatin1( "0" ) :
                style.linespace==AmiPro::LS_OneAndHalf ? QString::fromLatin1( "oneandhalf" ) :
                style.linespace==AmiPro::LS_Double ? QString::fromLatin1( "double" ) :
                  QString::number( style.linespace );

  QString offsets;
  offsets = "before=\"" + QString::number(style.spaceBefore) +
            "\" after=\"" + QString::number(style.spaceAfter) + "\"";

  result.append( "<STYLE>\n" );
  result.append( "  <NAME value=\"" + XMLEscape( style.name ) + "\" />\n" );
  result.append( "  <FOLLOWING name=\"Body Text\" />\n" );
  result.append( "  <FLOW align=\"" + align + "\" />\n" );
  result.append( "  <LINESPACING value=\"" + linespacing + "\" />\n" );
  result.append( "  <OFFSETS " + offsets + " />\n" );
  result.append( "  <LEFTBORDER width=\"0\" style=\"0\" />\n" );
  result.append( "  <RIGHTBORDER width=\"0\" style=\"0\" />\n" );
  result.append( "  <TOPBORDER width=\"0\" style=\"0\" />\n" );
  result.append( "  <BOTTOMBORDER width=\"0\" style=\"0\" />\n" );
  result.append( "  <FORMAT id=\"1\">\n" );
  result.append( "    <FONT name=\"" + fontname + "\" />\n" );
  result.append( "    <SIZE value=\"" + fontsize + "\" />\n" );
  result.append( "    <WEIGHT value=\"" + boldness + "\" />\n" );
  result.append( "    <ITALIC value=\"" + italic  + "\" />\n" );
  result.append( "    <STRIKEOUT value=\"" + strikeout + "\" />\n" );
  result.append( "    <VERTALIGN value=\"" + vertalign + "\" />\n" );
  result.append( "    <UNDERLINE value=\"" + underline + "\" />\n" );
  result.append( "  </FORMAT>\n" );
  result.append( "</STYLE>\n" );

  return result;
}

// helper function to convert AmiPro styles to KWord STYLES
static QString AmiProStyleListAsXML( AmiProStyleList& styleList )
{
  QString result;

  AmiProStyleList::iterator it;
  for( it=styleList.begin(); it!=styleList.end(); ++it )
  {
    AmiProStyle& style = *it;
    result.append( AmiProStyleAsXML( style ) );
  }

  if( !result.isEmpty() )
  {
    result.prepend ( "<STYLES>\n" );
    result.append( "</STYLES>\n" );
  }

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
  prolog.append( "<ATTRIBUTES standardpage=\"1\" hasFooter=\"0\" hasHeader=\"0\" processing=\"0\" />\n" );
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
  epilog.append( AmiProStyleListAsXML( styleList ) );
  epilog.append( "</DOC>\n" );

  root.append( epilog );

  return true;
}

bool AmiProConverter::doDefineStyle( const AmiProStyle& style )
{
  styleList.append( style );
  return true;
}

bool AmiProConverter::doParagraph( const QString& text, AmiProFormatList formatList,
  AmiProLayout& layout )
{
  root.append( "<PARAGRAPH>\n" );
  root.append( "<TEXT>" + XMLEscape( text ) + "</TEXT>\n" );
  root.append( AmiProFormatListAsXML( formatList ) );
  root.append( AmiProLayoutAsXML( layout ) );
  root.append( "</PARAGRAPH>\n" );

  return true;
}

KoFilter::ConversionStatus AmiProImport::convert( const QByteArray& from, const QByteArray& to )
{
  // check for proper conversion
  if( to!= "application/x-kword" || from != "application/x-amipro" )
     return KoFilter::NotImplemented;

  // parse/convert input file
  AmiProParser *parser = new AmiProParser;
  AmiProConverter *converter = new AmiProConverter;
  parser->setListener( converter );

  parser->process( m_chain->inputFile() );

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
      QByteArray cstring = root.toUtf8();
      cstring.prepend( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" );
      out->write( cstring );
    }

  // store document info
  out = m_chain->storageFile( "documentinfo.xml", KoStore::Write );
  if ( out )
    {
       QByteArray cstring = documentInfo.toUtf8();
       cstring.prepend( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" );
       out->write( cstring );
     }

  return KoFilter::OK;
}

#include "amiproimport.moc"
