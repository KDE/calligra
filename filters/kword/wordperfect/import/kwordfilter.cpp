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


#include "kwordfilter.h"
#include "parser.h"

#include <qfileinfo.h>
#include <q3ptrlist.h>
#include <QString>
#include <QRegExp>

using namespace WP;

// NOTE: 1 wpu = 1/1200 inch and 1 inch = 72 point
static double WPUToPoint( unsigned wpu )
{
    return wpu * 72 / 1200;
}

// helper class
class KWordFormat
{
  public:
    bool bold, italic, underline, doubleunderline;
    bool striked, superscript, subscript, redline;
    bool color, highlight;
    int red, green, blue;
    int bgred, bggreen, bgblue;
    double fontsize;
    QString fontface;

    KWordFormat();
    QString asXML();
};

KWordFormat::KWordFormat()
{
  bold = italic = underline = doubleunderline = FALSE;
  striked = superscript = subscript = redline = FALSE;
  color = highlight = FALSE;
  red = green = blue = 0;
  bgred = bggreen = bgblue = 255;
  fontsize = 0.0;
  fontface = "";
}

QString KWordFormat::asXML()
{
  QString result;

  if( bold ) result.append( "    <WEIGHT value=\"75\" />\n" );
  if( italic ) result.append( "    <ITALIC value=\"1\" />\n" );
  if( underline ) result.append( "    <UNDERLINE value=\"1\" />\n" );
  if( doubleunderline ) result.append( "    <UNDERLINE value=\"double\" />\n" );
  if( striked ) result.append( "    <STRIKEOUT value=\"1\" />\n" );
  if( subscript ) result.append( "<VERTALIGN value=\"1\" />\n" );
  if( superscript ) result.append( "<VERTALIGN value=\"2\" />\n" );

  if( !fontface.isEmpty() )
    result.append( "<FONT name=\"" + fontface + "\" />\n" );

  if( fontsize > 0.0 )
    result.append( "    <SIZE value=\"" + QString::number(fontsize) + "\" />\n" );

  if( color )
    result.append( "    <COLOR red=\"" + QString::number(red) +
                   "\" green=\"" + QString::number(green) +
                   "\" blue=\"" + QString::number(blue) + "\" />\n" );
  
  if( highlight )
    result.append( "    <TEXTBACKGROUNDCOLOR red=\"" + QString::number(bgred) +
                   "\" green=\"" + QString::number(bggreen) +
                   "\" blue=\"" + QString::number(bgblue) + "\" />\n" );

  return result;
}

static QString mapAlign( Token::Align align )
{
  switch( align )
  {
  case Token::Left: return "left";
  case Token::Right: return "right";
  case Token::Center: return "center";
  case Token::Full: return "justify";
  case Token::All: return "justify";
  }
  return "left";
}

// NOTE: KWord value for linespace: 72=one, 144=double, ..
// Special case: "0" is normal, "oneandhalf" is 108, "double" is 144.
static QString mapLinespace( double linespace )
{
  return QString::number( linespace );
}

KWordFilter::KWordFilter ():Parser ()
{
}

bool
KWordFilter::parse (const QString & filename)
{
  int frameLeftMargin = 36, frameRightMargin = 36; // quick hack

  if (!Parser::parse (filename))
    return FALSE;

  // this will force very last text and formatting to be flushed as well
  tokens.append( new Token( Token::HardReturn ) );

  QString text;
  QString layout;
  QString formats;
  int LeftMargin = 0;
  int TopMargin = 36;
  int RightMargin = 0;
  int BottomMargin = 36;
  int LeftMarginAdjust = 0;
  int RightMarginAdjust = 0;
  int lm = 0, rm = 0;
  Token::Align align = Token::Left;
  double linespace = 1.0;

  root = "";

  KWordFormat flag;
  int format_pos;
  QString fmt;

  // FIXME replace with doc initial code or default style
  format_pos = 0;
  fmt = flag.asXML();

  for (Q3PtrListIterator < Token > it (tokens); it; ++it)
    {
      unsigned int ucode;
      int attr;
      int len;
      Token *t = it.current ();
      Token::Type type = t->type ();

      switch (type)
        {

        case Token::Text:
          text.append( t->text() );
          break;

        case Token::SoftSpace:
        case Token::HardSpace:
          text.append( " " );
          break;

        case Token::SoftReturn:
          // ignore
          break;

        case Token::AttrOff:
        case Token::AttrOn:
        case Token::FontColor:
        case Token::FontSize:
        case Token::FontFace:
        case Token::HighlightOn:
        case Token::HighlightOff:

          if( type == Token::FontColor )
          {
            flag.color = true;
            flag.Qt::red = t->Qt::red();
            flag.Qt::green = t->Qt::green();
            flag.Qt::blue= t->Qt::blue();
          }
          else if( type == Token::HighlightOn )
          {
            flag.highlight = true;
            flag.bgred = t->Qt::red();
            flag.bggreen = t->Qt::green();
            flag.bgblue = t->Qt::blue();
          }
          else if( type == Token::HighlightOff )
          {
            // RGB in the data is last used highlight color
            // to go back to normal color, simply XOR would do the trick
            flag.highlight = false;
            flag.bgred ^= t->Qt::red();
            flag.bggreen ^= t->Qt::green();
            flag.bgblue ^= t->Qt::blue();
          }
          else if( type == Token::FontSize )
          {
            // WP font size is 1/3600th inch
            // NOTE 72 pt is 1 inch
            if( t->value() > 50 )
              flag.fontsize = t->value()*72.0/3600;
          }
          else if( type == Token::FontFace )
          {
            flag.fontface = t->fontface();
          }
          else
          {
            attr = t->attr();
            if( attr == Token::Bold ) flag.bold = ( type == Token::AttrOn );
            if( attr == Token::Italic ) flag.italic = ( type == Token::AttrOn );
            if( attr == Token::Underline) flag.underline = ( type == Token::AttrOn );
            if( attr == Token::DoubleUnderline ) flag.doubleunderline = ( type == Token::AttrOn );
            if( attr == Token::StrikedOut ) flag.striked = ( type == Token::AttrOn );
            if( attr == Token::Subscript ) flag.subscript = ( type == Token::AttrOn );
            if( attr == Token::Superscript ) flag.superscript = ( type == Token::AttrOn );
            if( attr == Token::Redline ) flag.redline = ( type == Token::AttrOn );
          }

          // process previous fmt first
          len = text.length() - format_pos;
          formats.append ( "<FORMAT id=\"1\" pos=\"" + QString::number( format_pos ) +
                             "\" len=\"" + QString::number( len )+ "\">\n" );
          formats.append( fmt );
          formats.append ( "</FORMAT>\n" );

          // now current format
          fmt = flag.asXML();
          format_pos= text.length();

          break;

        case Token::HardReturn:
        case Token::DormantHardReturn:

          // last formatting not flushed
          // SEE ABOVE
          len = text.length() - format_pos;
          formats.append ( "  <FORMAT id=\"1\" pos=\"" + QString::number( format_pos ) +
                             "\" len=\"" + QString::number( len )+ "\">\n" );
          formats.append( "  " + fmt );
          formats.append ( "  </FORMAT>\n" );

          layout = "";
          layout.append( "<LAYOUT>\n" );
          layout.append( "  <NAME value=\"Standard\" />\n" );
          layout.append( "  <FLOW align=\"" + mapAlign( align ) + "\" />\n" );
          layout.append( "  <LINESPACING value=\"" + mapLinespace( linespace) + "\" />\n" );
          layout.append( "  <LEFTBORDER width=\"0\" style=\"0\" />\n" );
          layout.append( "  <RIGHTBORDER width=\"0\" style=\"0\" />\n" );
          layout.append( "  <TOPBORDER width=\"0\" style=\"0\" />\n" );
          layout.append( "  <BOTTOMBORDER width=\"0\" style=\"0\" />\n" );
          lm = LeftMargin + LeftMarginAdjust - frameLeftMargin;
          rm = RightMargin + RightMarginAdjust - frameRightMargin;
          layout.append( "  <INDENTS left=\"" + QString::number( qMax( 0, lm ) ) + "\"" + 
                         " right=\"" + QString::number( qMax( 0 , rm ) ) + "\"" +
                         " first=\"0\" />\n" );
          layout.append( "  <OFFSETS />\n" );
          layout.append( "  <PAGEBREAKING />\n" );
          layout.append( "  <COUNTER />\n" );
          layout.append( "  <FORMAT id=\"1\">\n" );
          layout.append( "    <WEIGHT value=\"50\" />\n" );
          layout.append( "    <ITALIC value=\"0\" />\n" );
          layout.append( "    <UNDERLINE value=\"0\" />\n" );
          layout.append( "    <STRIKEOUT value=\"0\" />\n" );
          layout.append( "    <CHARSET value=\"0\" />\n" );
          layout.append( "    <VERTALIGN value=\"0\" />\n" );
          layout.append( "  </FORMAT>\n" );
          layout.append( "</LAYOUT>\n" );

          // encode text for XML-ness
          // FIXME could be faster without QRegExp
          text.replace( QRegExp("&"), "&amp;" );
          text.replace( QRegExp("<"), "&lt;" );
          text.replace( QRegExp(">"), "&gt;" );
          text.replace( QRegExp("\""), "&quot;" );
          text.replace( QRegExp("'"), "&apos;" );

          // construct the <PARAGRAPH>
          root.append( "<PARAGRAPH>\n" );
          root.append( "<TEXT>" + text + "</TEXT>\n" );
          root.append( "<FORMATS>\n");
          root.append( formats );
          root.append( "</FORMATS>\n");
          root.append( layout );
          root.append( "</PARAGRAPH>\n" );

          // for the next paragraph
          text = "";
          formats = "";
          format_pos = 0;
          fmt = flag.asXML();

          break;

        case Token::HardHyphen:
          text.append( "-" );
          break;

        case Token::LeftMargin:
          LeftMargin = (int) WPUToPoint( t->value() );
          break;

        case Token::RightMargin:
          RightMargin = (int) WPUToPoint( t->value() );
          break;

        case Token::TopMargin:
          TopMargin = (int) WPUToPoint( t->value() );
          break;

        case Token::BottomMargin:
          BottomMargin = (int) WPUToPoint( t->value() );
          break;

        case Token::LeftMarginAdjust:
          LeftMarginAdjust = (int)WPUToPoint( t->value() );
          break;

        case Token::RightMarginAdjust:
          RightMarginAdjust = (int)WPUToPoint( t->value() );
          break;

        case Token::Justification:
          align = t->align();
          break;

        case Token::Linespace:
          // NOTE assume 1.0 = 12 pt, 2.0 = 24 pt, 1.5=18
          // from parser.cpp, linespace is stored as 1/65536th inch
          linespace = t->value() * 12.0 / 65536;
          break;

        case Token::ExtChar:
          ucode = Parser::ExtCharToUnicode (t->charset (), t->charcode ());
          if (ucode == 0) ucode = 32;
          text.append( QChar (ucode) );
          break;

        case Token::TabHardFlushRight:
          // FIXME
          text.append( "    " );
          break;

        case Token::None:
        default:
          break;
        };

    }

  QString content = root;

  root = "<!DOCTYPE DOC>\n";
  root.append( "<DOC mime=\"application/x-kword\" syntaxVersion=\"2\" editor=\"KWord\">\n");

  // quick hack, think of something better in the future
  LeftMargin = RightMargin = 36;

  // paper definition
  root.append( "<PAPER width=\"595\" height=\"841\" format=\"1\" fType=\"0\" orientation=\"0\" hType=\"0\" columns=\"1\">\n" );
  root.append( " <PAPERBORDERS left=\"" + QString::number(frameLeftMargin) +
               "\" right=\"" + QString::number(frameRightMargin) +
               "\" top=\"" + QString::number(TopMargin) +
               "\" bottom=\"" + QString::number(BottomMargin) + "\" />\n" );
  root.append( "</PAPER>\n" );

  root.append( "<ATTRIBUTES standardpage=\"1\" hasFooter=\"0\" hasHeader=\"0\" processing=\"0\" />\n" );

  root.append( "<FRAMESETS>\n" );
  root.append( "<FRAMESET removable=\"0\" frameType=\"1\" frameInfo=\"0\" autoCreateNewFrame=\"1\">\n" );
  root.append( "<FRAME right=\"567\" left=\"28\" top=\"42\" bottom=\"799\" />\n" );
  root.append( content );
  root.append( "</FRAMESET>\n" );
  root.append( "</FRAMESETS>\n" );

  root.append( "</DOC>\n" );

  // in case no document summary is available, then make default
  // set so that basename of the filename becomes the document title
  // e.g /home/ariya/test/resume.wpd will have 'resume' as the title
  if( docTitle.isEmpty() )
  {
    QFileInfo info( filename );
    docTitle = info.baseName();
  }

  // create document information
  documentInfo = "<!DOCTYPE document-info>\n";

  documentInfo += "<document-info>\n";
  documentInfo += "<log><text></text></log>\n";

  documentInfo += "<author>\n";
  documentInfo += "<full-name>" + docAuthor + "</full-name>\n";
  documentInfo += "<title></title>\n";
  documentInfo += "<company></company>\n";
  documentInfo += "<email></email>\n";
  documentInfo += "<telephone></telephone>\n";
  documentInfo += "</author>\n";

  documentInfo += "<about>\n";
  documentInfo += "<abstract><![CDATA[" + docAbstract + "]]></abstract>\n";
  documentInfo += "<title>" + docTitle + "</title>\n";
  documentInfo += "</about>\n";

  documentInfo += "</document-info>";

  return TRUE;
}
