/* This file is part of the KDE project
   Copyright (C) 2003 Ariya Hidayat <ariya@kde.org>

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

#include <excelimport.h>
#include <excelimport.moc>

#include <qstring.h>
#include <qdom.h>
#include <qcolor.h>

#include <kdebug.h>
#include <koFilterChain.h>
#include <koGlobal.h>
#include <koUnit.h>
#include <kgenericfactory.h>
#include <kmessagebox.h>

#include <swinder.h>
#include <iostream>

typedef KGenericFactory<ExcelImport, KoFilter> ExcelImportFactory;
K_EXPORT_COMPONENT_FACTORY( libexcelimport, ExcelImportFactory( "kofficefilters" ) )


// UString -> QConstString conversion. Use .string() to get the QString.
// Always store the QConstString into a variable first, to avoid a deep copy.
inline QConstString string( const Swinder::UString& str ) {
   // Let's hope there's no copying of the QConstString happening...
   return QConstString( reinterpret_cast<const QChar*>( str.data() ), str.length() );
}

ExcelImport::ExcelImport ( QObject*, const char*, const QStringList& )
    : KoFilter()
{
}

// convert from RGB values to "#rrggbb"
static QString convertColor( const Swinder::Color& color )
{
  return QColor( color.red, color.green, color.blue ).name();
}

static QDomElement convertPen( QDomDocument& doc, const Swinder::Pen& pen )
{
  QDomElement penElement = doc.createElement( "pen" );

  unsigned style = 0;
  switch( pen.style )
  {
    case Swinder::Pen::NoLine:         style = 0; break;
    case Swinder::Pen::SolidLine:      style = 1; break;
    case Swinder::Pen::DashLine:       style = 2; break;
    case Swinder::Pen::DotLine:        style = 3; break;
    case Swinder::Pen::DashDotLine:    style = 4; break;
    case Swinder::Pen::DashDotDotLine: style = 5; break;
    default: style = 1; break; // fallback, solid line
  }

  penElement.setAttribute( "style", style );
  penElement.setAttribute( "width", pen.width );
  penElement.setAttribute( "color", convertColor( pen.color ) );

  return penElement;
}

QDomElement convertFormat( QDomDocument& doc, const Swinder::Format& format )
{
  QDomElement e = doc.createElement( "format" );

  unsigned align = 0;
  switch( format.alignment().alignX() )
  {
    case Swinder::Format::Left: align = 1; break;
    case Swinder::Format::Center: align = 2; break;
    case Swinder::Format::Right: align = 3; break;
     default: align = 0; break;
  };

  e.setAttribute( "align", QString::number( align ) );

  QDomElement fontElement = doc.createElement( "font" );
  const Swinder::FormatFont& font = format.font();
  QString fontFamily = string( font.fontFamily()).string();
  double fontSize = font.fontSize();
  fontElement.setAttribute( "family", fontFamily );
  fontElement.setAttribute( "size", fontSize );
  fontElement.setAttribute( "weight", font.bold() ? 75 : 50 );
  fontElement.setAttribute( "bold", font.bold() ? "yes" : "no" );
  fontElement.setAttribute( "italic", font.italic() ? "yes" : "no" );
  fontElement.setAttribute( "underline", font.underline() ? "yes" : "no" );
  fontElement.setAttribute( "strikeout", font.strikeout() ? "yes" : "no" );
  e.appendChild( fontElement );

  QDomElement penElement = doc.createElement( "pen" );
  penElement.setAttribute( "width", 0 );
  penElement.setAttribute( "style", 1 );
  penElement.setAttribute( "color", convertColor( font.color() ) );
  e.appendChild( penElement );


  // cell borders

  const Swinder::FormatBorders& borders = format.borders();

  QDomElement leftBorderElement = doc.createElement( "left-border" );
  leftBorderElement.appendChild( convertPen( doc, borders.leftBorder() ) );
  e.appendChild( leftBorderElement );

  QDomElement rightBorderElement = doc.createElement( "right-border" );
  rightBorderElement.appendChild( convertPen( doc, borders.rightBorder() ) );
  e.appendChild( rightBorderElement );

  QDomElement topBorderElement = doc.createElement( "top-border" );
  topBorderElement.appendChild( convertPen( doc, borders.topBorder() ) );
  e.appendChild( topBorderElement );

  QDomElement bottomBorderElement = doc.createElement( "bottom-border" );
  bottomBorderElement.appendChild( convertPen( doc, borders.bottomBorder() ) );
  e.appendChild( bottomBorderElement );

  return e;
}

static QDomElement convertValue( QDomDocument& doc, const Swinder::Value& value )
{
  QDomElement textElement;
  textElement = doc.createElement( "text" );

  if( value.isBoolean() )
  {
    textElement.setAttribute( "dataType", "Bool" );
    if( value.asBoolean() )
    {
      textElement.setAttribute( "outStr", "True" );
      textElement.appendChild( doc.createTextNode( "true" ) );
    }
    else
    {
      textElement.setAttribute( "outStr", "False" );
      textElement.appendChild( doc.createTextNode( "false" ) );
    }
  }

  else if( value.isFloat() )
  {
    textElement.setAttribute( "dataType", "Num" );
    QString str = QString::number( value.asFloat() );
    textElement.appendChild( doc.createTextNode( str ) );
  }

  else if( value.isInteger() )
  {
    textElement.setAttribute( "dataType", "Num" );
    QString str = QString::number( value.asInteger() );
    textElement.appendChild( doc.createTextNode( str ) );
  }

  else if( value.isString() )
  {
    textElement.setAttribute( "dataType", "Str" );
    QString str = string( value.asString() ).string();
    textElement.appendChild( doc.createTextNode( str ) );
  }

  return textElement;
}



static QString convertField( const QString& s )
{
  QString result = s;

  result.replace( "&A", "<sheet>" );
  result.replace( "&P", "<page>" );
  result.replace( "&N", "<pages>" );
  result.replace( "&D", "<date>" );
  result.replace( "&T", "<time>" );
  result.replace( "&F", "<file>" ); // not really correct

  return result;
}

static QDomElement convertHeader( QDomDocument& doc, const QString& left,
  const QString& center, const QString& right )
{
  QDomElement headElement = doc.createElement( "head" );

  QDomElement headLeftElement = doc.createElement( "left" );
  headLeftElement.appendChild( doc.createTextNode( convertField( left ) ) );
  headElement.appendChild( headLeftElement );

  QDomElement headCenterElement = doc.createElement( "center" );
  headCenterElement.appendChild( doc.createTextNode( convertField( center ) ) );
  headElement.appendChild( headCenterElement );

  QDomElement headRightElement = doc.createElement( "right" );
  headRightElement.appendChild( doc.createTextNode( convertField( right ) ) );
  headElement.appendChild( headRightElement );

  return headElement;
}

static QDomElement convertFooter( QDomDocument& doc, const QString& left,
  const QString& center, const QString& right )
{
  QDomElement footElement = doc.createElement( "foot" );

  QDomElement footLeftElement = doc.createElement( "left" );
  footLeftElement.appendChild( doc.createTextNode( convertField( left ) ) );
  footElement.appendChild( footLeftElement );

  QDomElement footCenterElement = doc.createElement( "center" );
  footCenterElement.appendChild( doc.createTextNode( convertField( center ) ) );
  footElement.appendChild( footCenterElement );

  QDomElement footRightElement = doc.createElement( "right" );
  footRightElement.appendChild( doc.createTextNode( convertField( right ) ) );
  footElement.appendChild( footRightElement );

  return footElement;
}

static QDomElement convertPaper( QDomDocument& doc, double leftMargin,
  double rightMargin, double topMargin, double bottomMargin )
{
  QDomElement paperElement = doc.createElement( "paper" );
  paperElement.setAttribute( "format", "A4" );
  paperElement.setAttribute( "orientation", "Portrait" );

  QDomElement bordersElement = doc.createElement( "borders" );
  bordersElement.setAttribute( "left", leftMargin );
  bordersElement.setAttribute( "right", rightMargin );
  bordersElement.setAttribute( "top", topMargin );
  bordersElement.setAttribute( "bottom", bottomMargin );
  paperElement.appendChild( bordersElement );

  return paperElement;
}


KoFilter::ConversionStatus ExcelImport::convert( const QCString& from, const QCString& to )
{
  if (to != "application/x-kspread" || from != "application/msexcel")
    return KoFilter::NotImplemented;

  QString inputFile = m_chain->inputFile();

  Swinder::Workbook* workbook = new Swinder::Workbook;
  if( workbook->load( inputFile.local8Bit() ) )
  {
    KMessageBox::sorry( 0, i18n("Could not read from file." ) );
    delete workbook;
    return KoFilter::StupidError;
  }

  QString root, documentInfo;

  QDomDocument mainDocument( "spreadsheet" );
  mainDocument.appendChild( mainDocument.createProcessingInstruction(
    "xml","version=\"1.0\" encoding=\"UTF-8\"" ) );

  QDomElement spreadsheet;
  spreadsheet = mainDocument.createElement( "spreadsheet" );
  spreadsheet.setAttribute( "editor","MS Excel Import Filter" );
  spreadsheet.setAttribute( "mime","application/x-kspread" );
  mainDocument.appendChild( spreadsheet );

  QDomElement map;
  map = mainDocument.createElement( "map" );
  map.setAttribute( "activeTable", "Table1" ); // dummy
  spreadsheet.appendChild( map );

  for( unsigned i=0; i < workbook->sheetCount(); i++ )
  {
    Swinder::Sheet* sheet = workbook->sheet( i );

    if( !sheet ) break;

    QDomElement table;
    table = mainDocument.createElement( "table" );
    table.setAttribute( "name", string( sheet->name() ).string() );
    table.setAttribute( "hide", sheet->visible() ? 0 : 1 );
    map.appendChild( table );

    // FIXME the real active sheet
    if( i == 0 ) map.setAttribute( "activeTable",string( sheet->name() ).string() );

    // paper settings, i.e <paper>
    QDomElement pe = convertPaper( mainDocument,
      POINT_TO_MM ( sheet->leftMargin() ), POINT_TO_MM ( sheet->rightMargin() ),
      POINT_TO_MM ( sheet->topMargin() ), POINT_TO_MM ( sheet->bottomMargin() ) );
    table.appendChild( pe );
    pe.appendChild( convertHeader( mainDocument, string( sheet->leftHeader() ).string(),
      string( sheet->centerHeader() ).string(), string( sheet->rightHeader() ).string() ) );
    pe.appendChild( convertFooter( mainDocument, string( sheet->leftFooter() ).string(),
      string( sheet->centerFooter() ).string(), string( sheet->rightFooter() ).string() ) );

    // columns, i.e <column>
    for( unsigned i = 0; i <= sheet->maxColumn(); i++ )
    {
      Swinder::Column* column = sheet->column( i, false );
      if( column )
      {
        QDomElement e;
        e = mainDocument.createElement( "column" );
        e.setAttribute( "column", i+1 );
        e.setAttribute( "width", column->width() );
        e.setAttribute( "hide", column->visible() ? 0 : 1 );
        table.appendChild( e );
        e.appendChild( convertFormat( mainDocument, column->format() ) );
      }
    }

    // rows, i.e <row>
    for( unsigned i = 0; i <= sheet->maxRow(); i++ )
    {
      Swinder::Row* row = sheet->row( i, false );
      if( row )
      {
        QDomElement e;
        e = mainDocument.createElement( "row" );
        e.setAttribute( "row", i+1 );
        e.setAttribute( "height", POINT_TO_MM ( row->height() ) );
        e.setAttribute( "hide", row->visible() ? 0 : 1 );
        table.appendChild( e );
        e.appendChild( convertFormat( mainDocument, row->format() ) );
      }
    }

    // cells, i.e <cell>
    for( unsigned row = 0; row <= sheet->maxRow(); row++ )
      for( unsigned col = 0; col <= sheet->maxColumn(); col++ )
      {
        Swinder::Cell* cell = sheet->cell( col, row, false );
        if( cell )
        {
          QDomElement ce;
          ce = mainDocument.createElement( "cell" );
          ce.setAttribute( "row", row+1 );
          ce.setAttribute( "column", col+1 );
          table.appendChild( ce );

          ce.appendChild( convertValue( mainDocument, cell->value() )  );

          QDomElement fe = convertFormat( mainDocument, cell->format() );
          if( cell->columnSpan() > 1 )
            fe.setAttribute( "colspan", cell->columnSpan()-1 );
          if( cell->rowSpan() > 1 )
            fe.setAttribute( "rowspan", cell->rowSpan()-1 );
          ce.appendChild( fe );

        }
      }

  }

  // prepare storage
  KoStoreDevice* out=m_chain->storageFile( "root", KoStore::Write );

  // store output document
  if( out )
    {
      QCString cstring = mainDocument.toCString();
      out->writeBlock( (const char*) cstring, cstring.length() );
      out->close();
    }

  // store document info
  out = m_chain->storageFile( "documentinfo.xml", KoStore::Write );
  if ( out )
    {
       QCString cstring = documentInfo.utf8();
       cstring.prepend( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" );
       out->writeBlock( (const char*) cstring, cstring.length() );
       out->close();
     }

  delete workbook;

  return KoFilter::OK;
}
