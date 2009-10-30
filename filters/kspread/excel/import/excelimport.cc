/* This file is part of the KDE project
   Copyright (C) 2003-2006 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2006 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>

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

#include <excelimport.h>
#include <excelimport.moc>

#include <QString>
#include <QDate>
#include <QBuffer>

#include <kdebug.h>
#include <KoFilterChain.h>
#include <KoGlobal.h>
#include <KoUnit.h>
#include <kgenericfactory.h>

#include <KoXmlWriter.h>
#include <KoOdfWriteStore.h>
#include <KoGenStyles.h>
#include <KoGenStyle.h>
#include <KoOdfNumberStyles.h>

#include "swinder.h"
#include <iostream>

typedef KGenericFactory<ExcelImport> ExcelImportFactory;
K_EXPORT_COMPONENT_FACTORY( libexcelimport, ExcelImportFactory( "kofficefilters" ) )


// UString -> QConstString conversion. Use  to get the QString.
// Always store the QConstString into a variable first, to avoid a deep copy.
inline QString string( const Swinder::UString& str ) {
   // Let's hope there's no copying of the QConstString happening...
    return QString::fromRawData( reinterpret_cast<const QChar*>( str.data() ), str.length() );
}

using namespace Swinder;

class ExcelImport::Private
{
public:
  QString inputFile;
  QString outputFile;

  Workbook *workbook;

  KoGenStyles *styles;
  QList<QString> cellStyles;
  QList<QString> rowStyles;
  QList<QString> colStyles;
  QList<QString> sheetStyles;

  bool createStyles( KoOdfWriteStore* store );
  bool createContent( KoOdfWriteStore* store );
  bool createManifest( KoOdfWriteStore* store );

  int sheetFormatIndex;
  int columnFormatIndex;
  int rowFormatIndex;
  int cellFormatIndex;

  void processWorkbookForBody( Workbook* workbook, KoXmlWriter* xmlWriter );
  void processWorkbookForStyle( Workbook* workbook, KoXmlWriter* xmlWriter );
  void processSheetForBody( Sheet* sheet, KoXmlWriter* xmlWriter );
  void processSheetForStyle( Sheet* sheet, KoXmlWriter* xmlWriter );
  void processColumnForBody( Column* column, int repeat, KoXmlWriter* xmlWriter );
  void processColumnForStyle( Column* column, int repeat, KoXmlWriter* xmlWriter );
  void processRowForBody( Row* row, int repeat, KoXmlWriter* xmlWriter );
  void processRowForStyle( Row* row, int repeat, KoXmlWriter* xmlWriter );
  void processCellForBody( Cell* cell, KoXmlWriter* xmlWriter );
  void processCellForStyle( Cell* cell, KoXmlWriter* xmlWriter );
  void processFormat( Format* format, KoGenStyle& style );
  QString processValueFormat( const QString& valueFormat );
};


ExcelImport::ExcelImport ( QObject* parent, const QStringList& )
    : KoFilter( parent )
{
  d = new Private;
}

ExcelImport::~ExcelImport()
{
  delete d;
}

KoFilter::ConversionStatus ExcelImport::convert( const QByteArray& from, const QByteArray& to )
{
  if ( from != "application/vnd.ms-excel" )
    return KoFilter::NotImplemented;

  if ( to != "application/vnd.oasis.opendocument.spreadsheet" )
    return KoFilter::NotImplemented;

  d->inputFile = m_chain->inputFile();
  d->outputFile = m_chain->outputFile();

  // open inputFile
  d->workbook = new Swinder::Workbook;
  if( !d->workbook->load( d->inputFile.toLocal8Bit() ) )
  {
    delete d->workbook;
    d->workbook = 0;
    return KoFilter::StupidError;
  }

  if( d->workbook->isPasswordProtected() )
  {
    delete d->workbook;
    d->workbook = 0;
    return KoFilter::PasswordProtected;
  }

  d->styles = new KoGenStyles();

  // create output store
  KoStore* storeout;
  storeout = KoStore::createStore( d->outputFile, KoStore::Write,
    "application/vnd.oasis.opendocument.spreadsheet", KoStore::Zip );

  if ( !storeout )
  {
    kWarning() << "Couldn't open the requested file.";
    delete d->workbook;
    return KoFilter::FileNotFound;
  }

  // Tell KoStore not to touch the file names
  storeout->disallowNameExpansion();
  KoOdfWriteStore oasisStore( storeout );

  // store document styles
  if ( !d->createStyles( &oasisStore ) )
  {
    kWarning() << "Couldn't open the file 'styles.xml'.";
    delete d->workbook;
    delete storeout;
    return KoFilter::CreationError;
  }

  // store document content
  if ( !d->createContent( &oasisStore ) )
  {
    kWarning() << "Couldn't open the file 'content.xml'.";
    delete d->workbook;
    delete storeout;
    return KoFilter::CreationError;
  }

  // store document manifest
  if ( !d->createManifest( &oasisStore ) )
  {
    kWarning() << "Couldn't open the file 'META-INF/manifest.xml'.";
    delete d->workbook;
    delete storeout;
    return KoFilter::CreationError;
  }

  // we are done!
  delete d->workbook;
  delete d->styles;
  delete storeout;
  d->inputFile.clear();
  d->outputFile.clear();
  d->workbook = 0;
  d->styles = 0;
  d->cellStyles.clear();
  d->rowStyles.clear();
  d->colStyles.clear();
  d->sheetStyles.clear();

  return KoFilter::OK;
}

bool ExcelImport::Private::createContent( KoOdfWriteStore* store )
{
  KoXmlWriter* bodyWriter = store->bodyWriter();
  KoXmlWriter* contentWriter = store->contentWriter();
  if ( !bodyWriter || !contentWriter )
    return false;

  // FIXME this is dummy and hardcoded, replace with real font names
  contentWriter->startElement( "office:font-face-decls" );
  contentWriter->startElement( "style:font-face" );
  contentWriter->addAttribute( "style:name", "Arial" );
  contentWriter->addAttribute( "svg:font-family", "Arial" );
  contentWriter->endElement(); // style:font-face
  contentWriter->startElement( "style:font-face" );
  contentWriter->addAttribute( "style:name", "Times New Roman" );
  contentWriter->addAttribute( "svg:font-family", "&apos;Times New Roman&apos;" );
  contentWriter->endElement(); // style:font-face
  contentWriter->endElement(); // office:font-face-decls

  // office:automatic-styles
  processWorkbookForStyle( workbook, contentWriter );
  styles->saveOdfAutomaticStyles( contentWriter, false );

  // important: reset all indexes
  sheetFormatIndex = 0;
  columnFormatIndex = 0;
  rowFormatIndex = 0;
  cellFormatIndex = 0;

  // office:body
  bodyWriter->startElement( "office:body" );
  processWorkbookForBody( workbook, bodyWriter );
  bodyWriter->endElement();  // office:body

  return store->closeContentWriter();
}

bool ExcelImport::Private::createStyles( KoOdfWriteStore* store )
{
  if ( !store->store()->open( "styles.xml" ) )
    return false;
  KoStoreDevice dev( store->store() );
  KoXmlWriter* stylesWriter = new KoXmlWriter( &dev );

  // FIXME this is dummy default, replace if necessary
  stylesWriter->startDocument( "office:document-styles" );
  stylesWriter->startElement( "office:document-styles" );
  stylesWriter->addAttribute( "xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0" );
  stylesWriter->addAttribute( "xmlns:style", "urn:oasis:names:tc:opendocument:xmlns:style:1.0" );
  stylesWriter->addAttribute( "xmlns:text", "urn:oasis:names:tc:opendocument:xmlns:text:1.0" );
  stylesWriter->addAttribute( "xmlns:table", "urn:oasis:names:tc:opendocument:xmlns:table:1.0" );
  stylesWriter->addAttribute( "xmlns:draw", "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0" );
  stylesWriter->addAttribute( "xmlns:fo", "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0" );
  stylesWriter->addAttribute( "xmlns:svg","urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0" );
  stylesWriter->addAttribute( "office:version","1.0" );
  stylesWriter->startElement( "office:styles" );
  stylesWriter->startElement( "style:default-style" );
  stylesWriter->addAttribute( "style:family", "table-cell" );
  stylesWriter->startElement( "style:table-cell-properties" );
  stylesWriter->addAttribute( "style:decimal-places", "2" );
  stylesWriter->endElement(); // style:table-cell-properties
  stylesWriter->startElement( "style:paragraph-properties" );
  stylesWriter->addAttribute( "style:tab-stop-distance", "0.5in" );
  stylesWriter->endElement(); // style:paragraph-properties
  stylesWriter->startElement( "style:text-properties" );
  stylesWriter->addAttribute( "style:font-name", "Albany AMT" );
  stylesWriter->addAttribute( "fo:language", "en" );
  stylesWriter->addAttribute( "fo:country", "US" );
  stylesWriter->addAttribute( "style:font-name-asian", "Albany AMT1" );
  stylesWriter->addAttribute( "style:country-asian", "none" );
  stylesWriter->addAttribute( "style:font-name-complex", "Lucidasans" );
  stylesWriter->addAttribute( "style:language-complex", "none" );
  stylesWriter->addAttribute( "style:country-complex", "none" );
  stylesWriter->endElement(); // style:text-properties
  stylesWriter->endElement(); // style:default-style
  stylesWriter->startElement( "style:style" );
  stylesWriter->addAttribute( "style:name", "Default" );
  stylesWriter->addAttribute( "style:family", "table-cell" );
  stylesWriter->endElement(); // style:style
  stylesWriter->endElement(); // office:styles

  // office:automatic-styles
  stylesWriter->startElement( "office:automatic-styles" );
  stylesWriter->endElement(); // office:automatic-styles

  stylesWriter->endElement();  // office:document-styles
  stylesWriter->endDocument();

  delete stylesWriter;

  return store->store()->close();
}

bool ExcelImport::Private::createManifest( KoOdfWriteStore* store )
{
  KoXmlWriter* manifestWriter = store->manifestWriter( "application/vnd.oasis.opendocument.spreadsheet" );

  manifestWriter->addManifestEntry( "styles.xml", "text/xml" );
  manifestWriter->addManifestEntry( "content.xml", "text/xml" );

  return store->closeManifestWriter();
}

void ExcelImport::Private::processWorkbookForBody( Workbook* workbook, KoXmlWriter* xmlWriter )
{
  if( !workbook ) return;
  if( !xmlWriter ) return;

  xmlWriter->startElement( "office:spreadsheet" );

  for( unsigned i=0; i < workbook->sheetCount(); i++ )
  {
    Sheet* sheet = workbook->sheet( i );
    processSheetForBody( sheet, xmlWriter );
  }

  xmlWriter->endElement();  // office:spreadsheet
}

void ExcelImport::Private::processWorkbookForStyle( Workbook* workbook, KoXmlWriter* xmlWriter )
{
  if( !workbook ) return;
  if( !xmlWriter ) return;

  for( unsigned i=0; i < workbook->sheetCount(); i++ )
  {
    Sheet* sheet = workbook->sheet( i );
    processSheetForStyle( sheet, xmlWriter );
  }
}

void ExcelImport::Private::processSheetForBody( Sheet* sheet, KoXmlWriter* xmlWriter )
{
  if( !sheet ) return;
  if( !xmlWriter ) return;

  xmlWriter->startElement( "table:table" );

  xmlWriter->addAttribute( "table:name", string( sheet->name() ) );
  xmlWriter->addAttribute( "table:print", "false" );
  xmlWriter->addAttribute( "table:protected", "false" );
  xmlWriter->addAttribute( "table:style-name", sheetStyles[sheetFormatIndex]);
  sheetFormatIndex++;

  unsigned ci = 0;
  while( ci <= sheet->maxColumn() )
  {
    Column* column = sheet->column( ci, false );
    if( column )
    {
      // forward search for columns with same properties
      unsigned cj = ci + 1;
      while( cj <= sheet->maxColumn() )
      {
        const Column* nextColumn = sheet->column( cj, false );
        if( !nextColumn ) break;
        if( column->width() != nextColumn->width() ) break;
        if( column->visible() != nextColumn->visible() ) break;
        if( column->format() != nextColumn->format() ) break;
        cj++;
      }

      int repeated = cj - ci;
      processColumnForBody( column, repeated, xmlWriter );
      ci += repeated;
    }
    else {
      ci++;
      xmlWriter->startElement( "table:table-column" );
      xmlWriter->endElement();
    }
  }

  for( unsigned i = 0; i <= sheet->maxRow(); i++ )
  {
    // FIXME optimized this when operator== in Swinder::Format is implemented
    processRowForBody( sheet->row( i, false ), 1, xmlWriter );
  }

  xmlWriter->endElement();  // table:table
}

void ExcelImport::Private::processSheetForStyle( Sheet* sheet, KoXmlWriter* xmlWriter )
{
  if( !sheet ) return;
  if( !xmlWriter ) return;

  KoGenStyle style(KoGenStyle::StyleAutoTable, "table");
  style.addAttribute( "style:master-page-name", "Default" );

  style.addProperty( "table:display", sheet->visible() ? "true" : "false" );
  style.addProperty( "table:writing-mode", "lr-tb" );

  QString styleName = styles->lookup(style, "ta");
  sheetStyles.append(styleName);

  unsigned ci = 0;
  while( ci <= sheet->maxColumn() )
  {
    Column* column = sheet->column( ci, false );
    if( column )
    {
      // forward search for similar column
      unsigned cj = ci + 1;
      while( cj <= sheet->maxColumn() )
      {
        Column* nextColumn = sheet->column( cj, false );
        if( !nextColumn ) break;
        if( column->width() != nextColumn->width() ) break;
        if( column->visible() != nextColumn->visible() ) break;
        if( column->format() != nextColumn->format() ) break;
        cj++;
      }

      int repeated = cj - ci;
      processColumnForStyle( column, repeated, xmlWriter );
      ci += repeated;
    }
    else
      ci++;
  }

  for( unsigned i = 0; i <= sheet->maxRow(); i++ )
  {
    Row* row = sheet->row( i, false );
    // FIXME optimized this when operator== in Swinder::Format is implemented
    processRowForStyle( row, 1, xmlWriter );
  }
}

void ExcelImport::Private::processColumnForBody( Column* column, int repeat, KoXmlWriter* xmlWriter )
{
  if( !column ) return;
  if( !xmlWriter ) return;

  xmlWriter->startElement( "table:table-column" );
  xmlWriter->addAttribute( "table:default-style-name", "Default" );
  xmlWriter->addAttribute( "table:visibility", column->visible() ? "visible" : "collapse" );
  if(repeat > 1) xmlWriter->addAttribute( "table:number-columns-repeated", repeat );
  xmlWriter->addAttribute( "table:style-name", colStyles[columnFormatIndex] );
  columnFormatIndex++;

  xmlWriter->endElement();  // table:table-column
}

void ExcelImport::Private::processColumnForStyle( Column* column, int /*repeat*/, KoXmlWriter* xmlWriter )
{
  if( !column ) return;
  if( !xmlWriter ) return;

  KoGenStyle style(KoGenStyle::StyleAutoTableColumn, "table-column");
  style.addProperty( "fo:break-before", "auto" );
  style.addProperty( "style:column-width", QString("%1in").arg(column->width()/27) );

  QString styleName = styles->lookup(style, "co");
  colStyles.append(styleName);
}

void ExcelImport::Private::processRowForBody( Row* row, int /*repeat*/, KoXmlWriter* xmlWriter )
{
  if( !xmlWriter ) return;
  if( !row ) {
    xmlWriter->startElement( "table:table-row" );
    xmlWriter->endElement();
    return;
  }
  if( !row->sheet() ) return;

  // find the column of the rightmost cell (if any)
  int lastCol = -1;
  for( unsigned i = 0; i <= row->sheet()->maxColumn(); i++ )
    if( row->sheet()->cell( i, row->index(), false ) ) lastCol = i;

  xmlWriter->startElement( "table:table-row" );
  xmlWriter->addAttribute( "table:visibility", row->visible() ? "visible" : "collapse" );
  xmlWriter->addAttribute( "table:style-name", rowStyles[rowFormatIndex] );
  rowFormatIndex++;

  for( int i = 0; i <= lastCol; i++ )
  {
    Cell* cell = row->sheet()->cell( i, row->index(), false );
    if( cell )
      processCellForBody( cell, xmlWriter );
    else
    {
      // empty cell
      xmlWriter->startElement( "table:table-cell" );
      xmlWriter->endElement();
    }
  }

  xmlWriter->endElement();  // table:table-row
}

void ExcelImport::Private::processRowForStyle( Row* row, int repeat, KoXmlWriter* xmlWriter )
{
  if( !row ) return;
  if( !row->sheet() ) return;
  if( !xmlWriter ) return;

  // find the column of the rightmost cell (if any)
  int lastCol = -1;
  for( unsigned i = 0; i <= row->sheet()->maxColumn(); i++ )
    if( row->sheet()->cell( i, row->index(), false ) ) lastCol = i;

  KoGenStyle style(KoGenStyle::StyleAutoTableRow, "table-row");
  if(repeat > 1) style.addAttribute( "table:number-rows-repeated", repeat );

  style.addProperty( "fo:break-before", "auto" );
  style.addPropertyPt( "style:row-height", row->height() );

  QString styleName = styles->lookup( style, "ro" );
  rowStyles.append(styleName);

  for( int i = 0; i <= lastCol; i++ )
  {
    Cell* cell = row->sheet()->cell( i, row->index(), false );
    if( cell )
      processCellForStyle( cell, xmlWriter );
  }
}

static bool isPercentageFormat( const QString& valueFormat )
{
  if( valueFormat.isEmpty() ) return false;
  if( valueFormat.length() < 1 ) return false;
  return valueFormat[valueFormat.length()-1] == QChar('%');
}

static bool isDateFormat( const QString& valueFormat )
{
  QString vfu = valueFormat.toUpper();

  if( vfu == "M/D/YY" ) return true;
  if( vfu == "M/D/YYYY" ) return true;
  if( vfu == "MM/DD/YY" ) return true;
  if( vfu == "MM/DD/YYYY" ) return true;
  if( vfu == "D-MMM-YY" ) return true;
  if( vfu == "D\\-MMM\\-YY" ) return true;
  if( vfu == "D-MMM-YYYY" ) return true;
  if( vfu == "D\\-MMM\\-YYYY" ) return true;
  if( vfu == "D-MMM" ) return true;
  if( vfu == "D\\-MMM" ) return true;
  if( vfu == "D-MM" ) return true;
  if( vfu == "D\\-MM" ) return true;
  if( vfu == "MMM/DD" ) return true;
  if( vfu == "MMM/D" ) return true;
  if( vfu == "MM/DD" ) return true;
  if( vfu == "MM/D" ) return true;
  if( vfu == "MM/DD/YY" ) return true;
  if( vfu == "MM/DD/YYYY" ) return true;
  if( vfu == "YYYY/MM/D" ) return true;
  if( vfu == "YYYY/MM/DD" ) return true;
  if( vfu == "YYYY-MM-D" ) return true;
  if( vfu == "YYYY\\-MM\\-D" ) return true;
  if( vfu == "YYYY-MM-DD" ) return true;
  if( vfu == "YYYY\\-MM\\-DD" ) return true;

  return false;
}

static bool isTimeFormat( const QString& valueFormat )
{
  QString vf = valueFormat;

  if( vf == "h:mm AM/PM" ) return true;
  if( vf == "h:mm:ss AM/PM" ) return true;
  if( vf == "h:mm" ) return true;
  if( vf == "h:mm:ss" ) return true;
  if( vf == "[h]:mm:ss" ) return true;
  if( vf == "[h]:mm" ) return true;
  if( vf == "[mm]:ss" ) return true;
  if( vf == "M/D/YY h:mm" ) return true;
  if( vf == "[ss]" ) return true;
  if( vf == "mm:ss" ) return true;
  if( vf == "mm:ss.0" ) return true;
  if( vf == "[mm]:ss" ) return true;
  if( vf == "[ss]" ) return true;

  return false;
}

static QString convertDate( double serialNo )
{
  // reference is midnight 30 Dec 1899
  QDate dd( 1899, 12, 30 );
  dd = dd.addDays( (int) serialNo );
  return dd.toString( "yyyy-MM-dd" );
}

static QString convertTime( double serialNo )
{
  // reference is midnight 30 Dec 1899
  QTime tt;
  tt = tt.addMSecs( qRound( (serialNo-(int)serialNo) * 86400 * 1000 ) );
  return tt.toString( "PThhHmmMss,zzz0S" );
}

void ExcelImport::Private::processCellForBody( Cell* cell, KoXmlWriter* xmlWriter )
{
  if( !cell ) return;
  if( !xmlWriter ) return;

  if( cell->isCovered() )
    xmlWriter->startElement( "table:covered-table-cell" );
  else
    xmlWriter->startElement( "table:table-cell" );

  xmlWriter->addAttribute( "table:style-name", cellStyles[cellFormatIndex] );
  cellFormatIndex++;

  if( cell->columnSpan() > 1 )
    xmlWriter->addAttribute( "table:number-columns-spanned", cell->columnSpan() );
  if( cell->rowSpan() > 1 )
    xmlWriter->addAttribute( "table:number-rows-spanned", cell->rowSpan() );

  QString formula = string( cell->formula() );
  if( !formula.isEmpty() )
    xmlWriter->addAttribute( "table:formula", formula.prepend("=") );

  Value value = cell->value();

  if( value.isBoolean() )
  {
    xmlWriter->addAttribute( "office:value-type", "boolean" );
    xmlWriter->addAttribute( "office:boolean-value", value.asBoolean() ? "true" : "false" );
  }
  else if( value.isFloat() || value.isInteger() )
  {
    QString valueFormat = string( cell->format().valueFormat() );

    bool handled = false;

    if( isPercentageFormat( valueFormat ) )
    {
      handled = true;
      xmlWriter->addAttribute( "office:value-type", "percentage" );
      xmlWriter->addAttribute( "office:value", QString::number( value.asFloat(), 'g', 15 ) );
    }

    if( isDateFormat( valueFormat ) )
    {
      handled = true;
      QString dateValue = convertDate( value.asFloat() );
      xmlWriter->addAttribute( "office:value-type", "date" );
      xmlWriter->addAttribute( "office:date-value", dateValue );
    }

    if( isTimeFormat( valueFormat ) )
    {
      handled = true;
      QString timeValue = convertTime( value.asFloat() );
      xmlWriter->addAttribute( "office:value-type", "time" );
      xmlWriter->addAttribute( "office:time-value", timeValue );
    }

    // fallback
    if( !handled )
    {
      xmlWriter->addAttribute( "office:value-type", "float" );
      xmlWriter->addAttribute( "office:value", QString::number( value.asFloat(), 'g', 15 ) );
    }
  }
  else if( value.isString() )
  {
    QString str = string( value.asString() );
    xmlWriter->addAttribute( "office:value-type", "string" );
    xmlWriter->addAttribute( "office:string-value", str );
    xmlWriter->startElement( "text:p" );
    xmlWriter->addTextNode( str );
    xmlWriter->endElement(); //  text:p
  }

  // TODO: handle formula

  xmlWriter->endElement(); //  table:[covered-]table-cell
}

void ExcelImport::Private::processCellForStyle( Cell* cell, KoXmlWriter* xmlWriter )
{
  if( !cell ) return;
  if( !xmlWriter ) return;

  // TODO optimize with hash table
  Format format = cell->format();

  // handle data format, e.g. number style
  QString refName;
  QString valueFormat = string( format.valueFormat() );
  if( valueFormat != QString("General") )
  {
    refName = processValueFormat( valueFormat );
  }

  KoGenStyle style(KoGenStyle::StyleAutoTableCell, "table-cell");
  // now the real table-cell
  if( !refName.isEmpty() )
    style.addAttribute( "style:data-style-name", refName );

  processFormat( &format, style );
  QString styleName = styles->lookup( style, "ce" );
  cellStyles.append(styleName);
}

QString convertColor( const Color& color )
{
  char buf[8];
  sprintf( buf, "#%02x%02x%02x", color.red, color.green, color.blue );
  return QString( buf );
}

QString convertBorder( const Pen& pen )
{
  if( pen.style == Pen::NoLine || pen.width == 0 ) return "none";

  QString result = QString::number( pen.width );
  result += "pt ";

  switch( pen.style )
  {
    case Pen::SolidLine: result += "solid "; break;
    case Pen::DashLine: result += "dashed "; break;
    case Pen::DotLine: result += "dotted "; break;
    case Pen::DashDotLine: result += "dot-dash "; break;
    case Pen::DashDotDotLine: result += "dot-dot-dash "; break;
  }

  return result + convertColor( pen.color );
}

void ExcelImport::Private::processFormat( Format* format, KoGenStyle& style )
{
  if( !format ) return;

  FormatFont font = format->font();
  FormatAlignment align = format->alignment();
  FormatBackground back = format->background();
  FormatBorders borders = format->borders();

  if( !font.isNull() )
  {
    if( font.bold() )
      style.addProperty("fo:font-weight", "bold", KoGenStyle::TextType);

    if( font.italic() )
      style.addProperty("fo:font-style", "italic", KoGenStyle::TextType);

    if( font.underline() )
    {
      style.addProperty("style:text-underline-style", "solid", KoGenStyle::TextType);
      style.addProperty("style:text-underline-width", "auto", KoGenStyle::TextType);
      style.addProperty("style:text-underline-color", "font-color", KoGenStyle::TextType);
    }

    if( font.strikeout() )
      style.addProperty("style:text-line-through-style", "solid", KoGenStyle::TextType);

    if( font.subscript() )
      style.addProperty("style:text-position", "sub", KoGenStyle::TextType);

    if( font.superscript() )
      style.addProperty("style:text-position", "super", KoGenStyle::TextType);

    if( !font.fontFamily().isEmpty() )
      style.addProperty("style:font-name", QString::fromRawData(reinterpret_cast<const QChar*>(font.fontFamily().data()), font.fontFamily().length()), KoGenStyle::TextType);

    style.addPropertyPt("fo:font-size", font.fontSize(), KoGenStyle::TextType);

    style.addProperty("fo:color", convertColor( font.color() ), KoGenStyle::TextType);
  }

  if( !align.isNull() )
  {
    switch( align.alignY() ) {
      case Format::Top: style.addProperty( "style:vertical-align", "top" ); break;
      case Format::Middle: style.addProperty( "style:vertical-align", "middle" ); break;
      case Format::Bottom: style.addProperty( "style:vertical-align", "bottom" ); break;
    }

    style.addProperty( "fo:wrap-option", align.wrap() ? "wrap" : "no-wrap" );
    //TODO rotation
    //TODO stacked letters
  }

  if( !borders.isNull() )
  {
    style.addProperty( "fo:border-left", convertBorder( borders.leftBorder() ) );
    style.addProperty( "fo:border-right", convertBorder( borders.rightBorder() ) );
    style.addProperty( "fo:border-top", convertBorder( borders.topBorder() ) );
    style.addProperty( "fo:border-bottom", convertBorder( borders.bottomBorder() ) );
    //TODO diagonal 'borders'
  }

  if( !back.isNull() && back.pattern() != FormatBackground::EmptyPattern )
  {
    Color backColor = back.backgroundColor();
    if( back.pattern() == FormatBackground::SolidPattern )
      backColor = back.foregroundColor();

    style.addProperty( "fo:background-color", convertColor( backColor ) );

    //TODO patterns
  }

  if( !align.isNull() )
  {
    switch( align.alignX() ) {
      case Format::Left: style.addProperty( "fo:text-align", "start", KoGenStyle::ParagraphType ); break;
      case Format::Center: style.addProperty( "fo:text-align", "center", KoGenStyle::ParagraphType ); break;
      case Format::Right: style.addProperty( "fo:text-align", "end", KoGenStyle::ParagraphType ); break;
    }

    if( align.indentLevel() != 0 )
      style.addProperty( "fo:margin-left", QString::number( align.indentLevel() ) + "0pt", KoGenStyle::ParagraphType );
  }
}

static void processDateFormatComponent( KoXmlWriter* xmlWriter, const QString& component )
{
  if( component[0] == 'd' )
  {
    xmlWriter->startElement( "number:day" );
    xmlWriter->addAttribute( "number:style", component.length() == 1 ? "short" : "long" );
    xmlWriter->endElement();  // number:day
  }
  else if( component[0] == 'm' )
  {
    xmlWriter->startElement( "number:month" );
    xmlWriter->addAttribute( "number:textual", component.length() == 3 ? "true" : "false" );
    xmlWriter->addAttribute( "number:style", component.length() == 2 ? "long" : "short" );
    xmlWriter->endElement();  // number:month
  }
  else if( component[0] == 'y' )
  {
    xmlWriter->startElement( "number:year" );
    xmlWriter->addAttribute( "number:style", component.length() == 2 ? "short" : "long" );
    xmlWriter->endElement();  // number:year
  }
}

static void processDateFormatSeparator( KoXmlWriter* xmlWriter, const QString& separator )
{
  xmlWriter->startElement( "number:text");
  xmlWriter->addTextNode( separator[separator.length() - 1] );
  xmlWriter->endElement();  // number:text
}

QString ExcelImport::Private::processValueFormat( const QString& valueFormat )
{
  QRegExp numberRegEx("(0+)(\\.0+)?(E\\+0+)?");
  QRegExp percentageRegEx("(0+)(\\.0+)?%");
  QRegExp dateRegEx("(m{1,3}|d{1,2}|yy|yyyy)(/|-|\\\\-)(m{1,3}|d{1,2}|yy|yyyy)(?:(/|-|\\\\-)(m{1,3}|d{1,2}|yy|yyyy))?");

  if( numberRegEx.exactMatch(valueFormat) )
  {
    if( numberRegEx.cap(3).length() )
      return KoOdfNumberStyles::saveOdfScientificStyle(*styles, valueFormat, "", "");
    else
      return KoOdfNumberStyles::saveOdfNumberStyle(*styles, valueFormat, "", "");
  }
  else if( percentageRegEx.exactMatch(valueFormat) )
  {
    return KoOdfNumberStyles::saveOdfPercentageStyle(*styles, valueFormat, "", "");
  }
  else if( dateRegEx.exactMatch(valueFormat) )
  {
    KoGenStyle style(KoGenStyle::StyleNumericDate);
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter elementWriter(&buffer);    // TODO pass indentation level

    processDateFormatComponent( &elementWriter, dateRegEx.cap(1) );
    processDateFormatSeparator( &elementWriter, dateRegEx.cap(2) );
    processDateFormatComponent( &elementWriter, dateRegEx.cap(3) );
    if( dateRegEx.cap(4).length() )
    {
      processDateFormatSeparator( &elementWriter, dateRegEx.cap(4) );
      processDateFormatComponent( &elementWriter, dateRegEx.cap(5) );
    }

    QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
    style.addChildElement("number", elementContents);

    return styles->lookup(style, "N");
  }
  else if( valueFormat == "h:mm AM/PM" )
  {
    KoGenStyle style(KoGenStyle::StyleNumericTime);
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter xmlWriter(&buffer);    // TODO pass indentation level

    xmlWriter.startElement( "number:hours" );
    xmlWriter.addAttribute( "number:style", "short" );
    xmlWriter.endElement();  // number:hour

    xmlWriter.startElement( "number:text");
    xmlWriter.addTextNode( ":" );
    xmlWriter.endElement();  // number:text

    xmlWriter.startElement( "number:minutes" );
    xmlWriter.addAttribute( "number:style", "long" );
    xmlWriter.endElement();  // number:minutes

    xmlWriter.startElement( "number:text");
    xmlWriter.addTextNode( " " );
    xmlWriter.endElement();  // number:text

    xmlWriter.startElement( "number:am-pm" );
    xmlWriter.endElement();  // number:am-pm

    QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
    style.addChildElement("number", elementContents);

    return styles->lookup(style, "N");
  }
  else if( valueFormat == "h:mm:ss AM/PM" )
  {
    KoGenStyle style(KoGenStyle::StyleNumericTime);
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter xmlWriter(&buffer);    // TODO pass indentation level

    xmlWriter.startElement( "number:hours" );
    xmlWriter.addAttribute( "number:style", "short" );
    xmlWriter.endElement();  // number:hour

    xmlWriter.startElement( "number:text");
    xmlWriter.addTextNode( ":" );
    xmlWriter.endElement();  // number:text

    xmlWriter.startElement( "number:minutes" );
    xmlWriter.addAttribute( "number:style", "long" );
    xmlWriter.endElement();  // number:minutes

    xmlWriter.startElement( "number:text");
    xmlWriter.addTextNode( ":" );
    xmlWriter.endElement();  // number:text

    xmlWriter.startElement( "number:seconds" );
    xmlWriter.addAttribute( "number:style", "long" );
    xmlWriter.endElement();  // number:minutes

    xmlWriter.startElement( "number:text");
    xmlWriter.addTextNode( " " );
    xmlWriter.endElement();  // number:text

    xmlWriter.startElement( "number:am-pm" );
    xmlWriter.endElement();  // number:am-pm

    QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
    style.addChildElement("number", elementContents);

    return styles->lookup(style, "N");
  }
  else if( valueFormat == "h:mm" )
  {
    KoGenStyle style(KoGenStyle::StyleNumericTime);
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter xmlWriter(&buffer);    // TODO pass indentation level

    xmlWriter.startElement( "number:hours" );
    xmlWriter.addAttribute( "number:style", "short" );
    xmlWriter.endElement();  // number:hour

    xmlWriter.startElement( "number:text");
    xmlWriter.addTextNode( ":" );
    xmlWriter.endElement();  // number:text

    xmlWriter.startElement( "number:minutes" );
    xmlWriter.addAttribute( "number:style", "long" );
    xmlWriter.endElement();  // number:minutes

    QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
    style.addChildElement("number", elementContents);

    return styles->lookup(style, "N");
  }
  else if( valueFormat == "h:mm:ss" )
  {
    KoGenStyle style(KoGenStyle::StyleNumericTime);
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter xmlWriter(&buffer);    // TODO pass indentation level

    xmlWriter.startElement( "number:hours" );
    xmlWriter.addAttribute( "number:style", "short" );
    xmlWriter.endElement();  // number:hour

    xmlWriter.startElement( "number:text");
    xmlWriter.addTextNode( ":" );
    xmlWriter.endElement();  // number:text

    xmlWriter.startElement( "number:minutes" );
    xmlWriter.addAttribute( "number:style", "long" );
    xmlWriter.endElement();  // number:minutes

    xmlWriter.startElement( "number:text");
    xmlWriter.addTextNode( ":" );
    xmlWriter.endElement();  // number:text

    xmlWriter.startElement( "number:seconds" );
    xmlWriter.addAttribute( "number:style", "long" );
    xmlWriter.endElement();  // number:minutes

    QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
    style.addChildElement("number", elementContents);

    return styles->lookup(style, "N");
  }
  else if( valueFormat == "[h]:mm" )
  {
    KoGenStyle style(KoGenStyle::StyleNumericTime);
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter xmlWriter(&buffer);    // TODO pass indentation level

    style.addAttribute( "number:truncate-on-overflow", "false" );

    xmlWriter.startElement( "number:hours" );
    xmlWriter.addAttribute( "number:style", "short" );
    xmlWriter.endElement();  // number:hour

    xmlWriter.startElement( "number:text");
    xmlWriter.addTextNode( ":" );
    xmlWriter.endElement();  // number:text

    xmlWriter.startElement( "number:minutes" );
    xmlWriter.addAttribute( "number:style", "long" );
    xmlWriter.endElement();  // number:minutes

    QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
    style.addChildElement("number", elementContents);

    return styles->lookup(style, "N");
  }
  else if( valueFormat == "[h]:mm:ss" )
  {
    KoGenStyle style(KoGenStyle::StyleNumericTime);
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter xmlWriter(&buffer);    // TODO pass indentation level

    style.addAttribute( "number:truncate-on-overflow", "false" );

    xmlWriter.startElement( "number:hours" );
    xmlWriter.addAttribute( "number:style", "short" );
    xmlWriter.endElement();  // number:hour

    xmlWriter.startElement( "number:text");
    xmlWriter.addTextNode( ":" );
    xmlWriter.endElement();  // number:text

    xmlWriter.startElement( "number:minutes" );
    xmlWriter.addAttribute( "number:style", "long" );
    xmlWriter.endElement();  // number:minutes

    xmlWriter.startElement( "number:text");
    xmlWriter.addTextNode( ":" );
    xmlWriter.endElement();  // number:text

    xmlWriter.startElement( "number:seconds" );
    xmlWriter.addAttribute( "number:style", "long" );
    xmlWriter.endElement();  // number:minutes

    QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
    style.addChildElement("number", elementContents);

    return styles->lookup(style, "N");
  }
  else if( valueFormat == "mm:ss" )
  {
    KoGenStyle style(KoGenStyle::StyleNumericTime);
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter xmlWriter(&buffer);    // TODO pass indentation level

    xmlWriter.startElement( "number:minutes" );
    xmlWriter.addAttribute( "number:style", "long" );
    xmlWriter.endElement();  // number:minutes

    xmlWriter.startElement( "number:text");
    xmlWriter.addTextNode( ":" );
    xmlWriter.endElement();  // number:text

    xmlWriter.startElement( "number:seconds" );
    xmlWriter.addAttribute( "number:style", "long" );
    xmlWriter.endElement();  // number:minutes

    QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
    style.addChildElement("number", elementContents);

    return styles->lookup(style, "N");
  }
  else if( valueFormat == "mm:ss.0" )
  {
    KoGenStyle style(KoGenStyle::StyleNumericTime);
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter xmlWriter(&buffer);    // TODO pass indentation level

    xmlWriter.startElement( "number:minutes" );
    xmlWriter.addAttribute( "number:style", "long" );
    xmlWriter.endElement();  // number:minutes

    xmlWriter.startElement( "number:text");
    xmlWriter.addTextNode( ":" );
    xmlWriter.endElement();  // number:text

    xmlWriter.startElement( "number:seconds" );
    xmlWriter.addAttribute( "number:style", "long" );

    xmlWriter.endElement();  // number:minutes
    xmlWriter.startElement( "number:text");
    xmlWriter.addTextNode( ".0" );
    xmlWriter.endElement();  // number:text


    QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
    style.addChildElement("number", elementContents);

    return styles->lookup(style, "N");
  }
  else if( valueFormat == "[mm]:ss" )
  {
    KoGenStyle style(KoGenStyle::StyleNumericTime);
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter xmlWriter(&buffer);    // TODO pass indentation level

    style.addAttribute( "number:truncate-on-overflow", "false" );

    xmlWriter.startElement( "number:minutes" );
    xmlWriter.addAttribute( "number:style", "long" );
    xmlWriter.endElement();  // number:minutes

    xmlWriter.startElement( "number:text");
    xmlWriter.addTextNode( ":" );
    xmlWriter.endElement();  // number:textexactMatch

    xmlWriter.startElement( "number:seconds" );
    xmlWriter.addAttribute( "number:style", "long" );
    xmlWriter.endElement();  // number:minutes

    QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
    style.addChildElement("number", elementContents);

    return styles->lookup(style, "N");
  }
  else if( valueFormat == "[ss]" )
  {
    KoGenStyle style(KoGenStyle::StyleNumericTime);
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter xmlWriter(&buffer);    // TODO pass indentation level

    style.addAttribute( "number:truncate-on-overflow", "false" );

    xmlWriter.startElement( "number:seconds" );
    xmlWriter.addAttribute( "number:style", "long" );
    xmlWriter.endElement();  // number:minutes

    QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
    style.addChildElement("number", elementContents);

    return styles->lookup(style, "N");
  }
  else
  {
    return "";
  }
}
