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

#include <config.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <excelimport.h>
#include <excelimport.moc>

#include <QBuffer>
#include <q3cstring.h>
#include <QDateTime>
#include <QFile>
#include <QString>
#include <QTextStream>

#include <kdebug.h>
#include <KoFilterChain.h>
#include <KoGlobal.h>
#include <KoUnit.h>
#include <kgenericfactory.h>

#include <KoXmlWriter.h>
#include <KoOasisStore.h>

#include "swinder.h"
#include <iostream>

typedef KGenericFactory<ExcelImport> ExcelImportFactory;
K_EXPORT_COMPONENT_FACTORY( libexcelimport, ExcelImportFactory( "kofficefilters" ) )


// UString -> QConstString conversion. Use .string() to get the QString.
// Always store the QConstString into a variable first, to avoid a deep copy.
inline QConstString string( const Swinder::UString& str ) {
   // Let's hope there's no copying of the QConstString happening...
   return QConstString( reinterpret_cast<const QChar*>( str.data() ), str.length() );
}

using namespace Swinder;

class ExcelImport::Private
{
public:
  QString inputFile;
  QString outputFile;

  Workbook *workbook;

  bool createStyles( KoOasisStore* store );
  bool createContent( KoOasisStore* store );
  bool createManifest( KoOasisStore* store );

  int sheetFormatIndex;
  int columnFormatIndex;
  int rowFormatIndex;
  int cellFormatIndex;
  int valueFormatIndex;

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
  void processFormat( Format* format, KoXmlWriter* xmlWriter );
  void processValueFormat( QString valueFormat, QString refName, KoXmlWriter* xmlWriter );
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
  if ( from != "application/msexcel" )
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

  // create output store
  KoStore* storeout;
  storeout = KoStore::createStore( d->outputFile, KoStore::Write,
    "application/vnd.oasis.opendocument.spreadsheet", KoStore::Zip );

  if ( !storeout )
  {
    kWarning() << "Couldn't open the requested file." << endl;
    delete d->workbook;
    return KoFilter::FileNotFound;
  }

  // Tell KoStore not to touch the file names
  storeout->disallowNameExpansion();
  KoOasisStore oasisStore( storeout );

  // store document styles
  d->sheetFormatIndex = 1;
  d->columnFormatIndex = 1;
  d->rowFormatIndex = 1;
  d->cellFormatIndex = 1;
  d->valueFormatIndex = 1;
  if ( !d->createStyles( &oasisStore ) )
  {
    kWarning() << "Couldn't open the file 'styles.xml'." << endl;
    delete d->workbook;
    delete storeout;
    return KoFilter::CreationError;
  }

  // store document content
  d->sheetFormatIndex = 1;
  d->columnFormatIndex = 1;
  d->rowFormatIndex = 1;
  d->cellFormatIndex = 1;
  d->valueFormatIndex = 1;
  if ( !d->createContent( &oasisStore ) )
  {
    kWarning() << "Couldn't open the file 'content.xml'." << endl;
    delete d->workbook;
    delete storeout;
    return KoFilter::CreationError;
  }

  // store document manifest
  if ( !d->createManifest( &oasisStore ) )
  {
    kWarning() << "Couldn't open the file 'META-INF/manifest.xml'." << endl;
    delete d->workbook;
    delete storeout;
    return KoFilter::CreationError;
  }

  // we are done!
  delete d->workbook;
  delete storeout;
  d->inputFile = QString::null;
  d->outputFile = QString::null;
  d->workbook = 0;

  return KoFilter::OK;
}

bool ExcelImport::Private::createContent( KoOasisStore* store )
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

  // important: reset all indexes
  sheetFormatIndex = 1;
  columnFormatIndex = 1;
  rowFormatIndex = 1;
  cellFormatIndex = 1;
  valueFormatIndex = 1;

  // office:automatic-styles
  contentWriter->startElement( "office:automatic-styles" );
  processWorkbookForStyle( workbook, contentWriter );
  contentWriter->endElement(); // office:automatic-style

  // important: reset all indexes
  sheetFormatIndex = 1;
  columnFormatIndex = 1;
  rowFormatIndex = 1;
  cellFormatIndex = 1;
  valueFormatIndex = 1;

  // office:body
  bodyWriter->startElement( "office:body" );
  processWorkbookForBody( workbook, bodyWriter );
  bodyWriter->endElement();  // office:body

  return store->closeContentWriter();
}

bool ExcelImport::Private::createStyles( KoOasisStore* store )
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

bool ExcelImport::Private::createManifest( KoOasisStore* store )
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

  xmlWriter->addAttribute( "table:name", string( sheet->name() ).string() );
  xmlWriter->addAttribute( "table:print", "false" );
  xmlWriter->addAttribute( "table:protected", "false" );
  xmlWriter->addAttribute( "table:style-name", QString("ta%1").arg(sheetFormatIndex));
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

  xmlWriter->startElement( "style:style" );
  xmlWriter->addAttribute( "style:family", "table" );
  xmlWriter->addAttribute( "style:master-page-name", "Default" );
  xmlWriter->addAttribute( "style:name", QString("ta%1").arg(sheetFormatIndex) );
  sheetFormatIndex++;

  xmlWriter->startElement( "style:table-properties" );
  xmlWriter->addAttribute( "table:display", sheet->visible() ? "true" : "false" );
  xmlWriter->addAttribute( "table:writing-mode", "lr-tb" );
  xmlWriter->endElement();  // style:table-properties

  xmlWriter->endElement();  // style:style

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
  xmlWriter->addAttribute( "table:style-name", QString("co%1").arg(columnFormatIndex) );
  columnFormatIndex++;

  xmlWriter->endElement();  // table:table-column
}

void ExcelImport::Private::processColumnForStyle( Column* column, int /*repeat*/, KoXmlWriter* xmlWriter )
{
  if( !column ) return;
  if( !xmlWriter ) return;

  xmlWriter->startElement( "style:style" );
  xmlWriter->addAttribute( "style:family", "table-column" );
  xmlWriter->addAttribute( "style:name", QString("co%1").arg(columnFormatIndex) );
  columnFormatIndex++;

  xmlWriter->startElement( "style:table-column-properties" );
  xmlWriter->addAttribute( "fo:break-before", "auto" );
  xmlWriter->addAttribute( "style:column-width", QString("%1in").arg(column->width()/27) );
  xmlWriter->endElement();  // style:table-column-properties

  xmlWriter->endElement();  // style:style
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
  xmlWriter->addAttribute( "table:style-name", QString("ro%1").arg(rowFormatIndex) );
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

  xmlWriter->startElement( "style:style" );
  xmlWriter->addAttribute( "style:family", "table-row" );
  if(repeat > 1) xmlWriter->addAttribute( "table:number-rows-repeated", repeat );
  xmlWriter->addAttribute( "style:name", QString("ro%1").arg(rowFormatIndex) );
  rowFormatIndex++;

  xmlWriter->startElement( "style:table-row-properties" );
  xmlWriter->addAttribute( "fo:break-before", "auto" );
  xmlWriter->addAttribute( "style:row-height", QString("%1pt").arg(row->height()) );
  xmlWriter->endElement();  // style:table-row-properties

  xmlWriter->endElement();  // style:style

  for( int i = 0; i <= lastCol; i++ )
  {
    Cell* cell = row->sheet()->cell( i, row->index(), false );
    if( cell )
      processCellForStyle( cell, xmlWriter );
  }
}

static bool isPercentageFormat( QString valueFormat )
{
  if( valueFormat.isEmpty() ) return false;
  if( valueFormat.length() < 1 ) return false;
  return valueFormat[valueFormat.length()-1] == QChar('%');
}

static bool isDateFormat( QString valueFormat )
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

static bool isTimeFormat( QString valueFormat )
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

  xmlWriter->startElement( "table:table-cell" );
  xmlWriter->addAttribute( "table:style-name", QString("ce%1").arg(cellFormatIndex) );
  cellFormatIndex++;

  QString formula = string( cell->formula() ).string();
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
    QString valueFormat = string( cell->format().valueFormat() ).string();

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
    QString str = string( value.asString() ).string();
    xmlWriter->addAttribute( "office:value-type", "string" );
    xmlWriter->addAttribute( "office:string-value", str );
    xmlWriter->startElement( "text:p" );
    xmlWriter->addTextNode( str );
    xmlWriter->endElement(); //  text:p
  }

  // TODO: handle formula

  xmlWriter->endElement(); //  table:table-cell
}

void ExcelImport::Private::processCellForStyle( Cell* cell, KoXmlWriter* xmlWriter )
{
  if( !cell ) return;
  if( !xmlWriter ) return;

  // TODO optimize with hash table
  Format format = cell->format();

  // handle data format, e.g. number style
  QString refName;
  QString valueFormat = string( format.valueFormat() ).string();
  if( valueFormat != QString("General") )
  {
    refName = QString("N%1").arg(valueFormatIndex);
    valueFormatIndex++;
    processValueFormat( valueFormat, refName, xmlWriter );
  }

  // now the real table-cell
  xmlWriter->startElement( "style:style" );
  xmlWriter->addAttribute( "style:family", "table-cell" );
  xmlWriter->addAttribute( "style:name", QString("ce%1").arg(cellFormatIndex) );
  cellFormatIndex++;
  if( !refName.isEmpty() )
    xmlWriter->addAttribute( "style:data-style-name", refName );

  processFormat( &format, xmlWriter );

  xmlWriter->endElement();  // style:style

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

void ExcelImport::Private::processFormat( Format* format, KoXmlWriter* xmlWriter )
{
  if( !format ) return;
  if( !xmlWriter ) return;

  FormatFont font = format->font();
  FormatAlignment align = format->alignment();
  FormatBackground back = format->background();
  FormatBorders borders = format->borders();

  if( !font.isNull() )
  {
    xmlWriter->startElement( "style:text-properties" );

    if( font.bold() )
      xmlWriter->addAttribute( "fo:font-weight", "bold" );

    if( font.italic() )
      xmlWriter->addAttribute( "fo:font-style", "italic" );

    if( font.underline() )
    {
      xmlWriter->addAttribute( "style:text-underline-style", "solid" );
      xmlWriter->addAttribute( "style:text-underline-width", "auto" );
      xmlWriter->addAttribute( "style:text-underline-color", "font-color" );
    }

    if( font.strikeout() )
      xmlWriter->addAttribute( "style:text-line-through-style", "solid" );

    if( font.subscript() )
      xmlWriter->addAttribute( "style:text-position", "sub" );

    if( font.superscript() )
      xmlWriter->addAttribute( "style:text-position", "super" );

    if( !font.fontFamily().isEmpty() )
      xmlWriter->addAttribute( "style:font-name", string(font.fontFamily()).string() );

    xmlWriter->addAttribute( "fo:font-size", QString("%1pt").arg(font.fontSize()) );

    xmlWriter->addAttribute( "fo:color", convertColor( font.color() ) );

    xmlWriter->endElement();  // style:text-properties
  }

  xmlWriter->startElement( "style:table-cell-properties" );
  if( !align.isNull() )
  {
    switch( align.alignY() ) {
      case Format::Top: xmlWriter->addAttribute( "style:vertical-align", "top" ); break;
      case Format::Middle: xmlWriter->addAttribute( "style:vertical-align", "middle" ); break;
      case Format::Bottom: xmlWriter->addAttribute( "style:vertical-align", "bottom" ); break;
    }

    xmlWriter->addAttribute( "fo:wrap-option", align.wrap() ? "wrap" : "no-wrap" );
    //TODO rotation
    //TODO stacked letters
  }

  if( !borders.isNull() )
  {
    xmlWriter->addAttribute( "fo:border-left", convertBorder( borders.leftBorder() ) );
    xmlWriter->addAttribute( "fo:border-right", convertBorder( borders.rightBorder() ) );
    xmlWriter->addAttribute( "fo:border-top", convertBorder( borders.topBorder() ) );
    xmlWriter->addAttribute( "fo:border-bottom", convertBorder( borders.bottomBorder() ) );
    //TODO diagonal 'borders'
  }

  if( !back.isNull() && back.pattern() != FormatBackground::EmptyPattern )
  {
    Color backColor = back.backgroundColor();
    if( back.pattern() == FormatBackground::SolidPattern )
      backColor = back.foregroundColor();

    xmlWriter->addAttribute( "fo:background-color", convertColor( backColor ) );

    //TODO patterns
  }
  xmlWriter->endElement(); // style:table-cell-properties

  xmlWriter->startElement( "style:paragraph-properties" );
  if( !align.isNull() )
  {
    switch( align.alignX() ) {
      case Format::Left: xmlWriter->addAttribute( "fo:text-align", "start" ); break;
      case Format::Center: xmlWriter->addAttribute( "fo:text-align", "center" ); break;
      case Format::Right: xmlWriter->addAttribute( "fo:text-align", "end" ); break;
    }

    if( align.indentLevel() != 0 )
      xmlWriter->addAttribute( "fo:margin-left", QString::number( align.indentLevel() ) + "0pt" );
  }
  xmlWriter->endElement(); // style:paragraph-properties
}

void ExcelImport::Private::processValueFormat( QString valueFormat, QString refName,
KoXmlWriter* xmlWriter )
{
  /*int decimalPlaces = 2;
  int leadingZeroes = 1;
  int exponentDigits = -1;
  bool percentage = false;*/

  // TODO: someday we need a real MS Excel to OpenDocument format paraser
  // this just catches the most common format, not covers all possible cases

  if( valueFormat == "0")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 0 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0.0")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 1 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0.00")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 2 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0.000")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 3 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0.0000")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 4 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0.00000")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 5 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0.000000")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 6 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0.0000000")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 7 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0.00000000")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 8 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0.000000000")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 9 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0.0000000000")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 10 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0.00000000000")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 11 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0.000000000000")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 12 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0.0000000000000")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 13 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0.00000000000000")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 14 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0.000000000000000")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 15 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0.0000000000000000")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 16 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0E+00")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:scientific-number" );
    xmlWriter->addAttribute( "number:decimal-places", 0 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->addAttribute( "number:min-exponent-digits", 2 );
    xmlWriter->endElement();  // number:scientific-number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0.0E+00")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:scientific-number" );
    xmlWriter->addAttribute( "number:decimal-places", 1 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->addAttribute( "number:min-exponent-digits", 2 );
    xmlWriter->endElement();  // number:scientific-number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0.00E+00")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:scientific-number" );
    xmlWriter->addAttribute( "number:decimal-places", 2 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->addAttribute( "number:min-exponent-digits", 2 );
    xmlWriter->endElement();  // number:scientific-number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0.000E+00")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:scientific-number" );
    xmlWriter->addAttribute( "number:decimal-places", 3 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->addAttribute( "number:min-exponent-digits", 2 );
    xmlWriter->endElement();  // number:scientific-number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0.0000E+00")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:scientific-number" );
    xmlWriter->addAttribute( "number:decimal-places", 4 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->addAttribute( "number:min-exponent-digits", 2 );
    xmlWriter->endElement();  // number:scientific-number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0.00000E+00")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:scientific-number" );
    xmlWriter->addAttribute( "number:decimal-places", 5 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->addAttribute( "number:min-exponent-digits", 2 );
    xmlWriter->endElement();  // number:scientific-number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0.000000E+00")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:scientific-number" );
    xmlWriter->addAttribute( "number:decimal-places", 6 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->addAttribute( "number:min-exponent-digits", 2 );
    xmlWriter->endElement();  // number:scientific-number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0.0000000E+00")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:scientific-number" );
    xmlWriter->addAttribute( "number:decimal-places", 7 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->addAttribute( "number:min-exponent-digits", 2 );
    xmlWriter->endElement();  // number:scientific-number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0.00000000E+00")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:scientific-number" );
    xmlWriter->addAttribute( "number:decimal-places", 8 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->addAttribute( "number:min-exponent-digits", 2 );
    xmlWriter->endElement();  // number:scientific-number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0.000000000E+00")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:scientific-number" );
    xmlWriter->addAttribute( "number:decimal-places", 9 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->addAttribute( "number:min-exponent-digits", 2 );
    xmlWriter->endElement();  // number:scientific-number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0.0000000000E+00")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:scientific-number" );
    xmlWriter->addAttribute( "number:decimal-places", 10 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->addAttribute( "number:min-exponent-digits", 2 );
    xmlWriter->endElement();  // number:scientific-number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0.00000000000E+00")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:scientific-number" );
    xmlWriter->addAttribute( "number:decimal-places", 11 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->addAttribute( "number:min-exponent-digits", 2 );
    xmlWriter->endElement();  // number:scientific-number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0.000000000000E+00")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:scientific-number" );
    xmlWriter->addAttribute( "number:decimal-places", 12 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->addAttribute( "number:min-exponent-digits", 2 );
    xmlWriter->endElement();  // number:scientific-number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0.0000000000000E+00")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:scientific-number" );
    xmlWriter->addAttribute( "number:decimal-places", 13 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->addAttribute( "number:min-exponent-digits", 2 );
    xmlWriter->endElement();  // number:scientific-number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0.00000000000000E+00")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:scientific-number" );
    xmlWriter->addAttribute( "number:decimal-places", 14 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->addAttribute( "number:min-exponent-digits", 2 );
    xmlWriter->endElement();  // number:scientific-number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0.000000000000000E+00")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:scientific-number" );
    xmlWriter->addAttribute( "number:decimal-places", 16 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->addAttribute( "number:min-exponent-digits", 2 );
    xmlWriter->endElement();  // number:scientific-number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0.0000000000000000E+00")
  {
    xmlWriter->startElement( "number:number-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->startElement( "number:scientific-number" );
    xmlWriter->addAttribute( "number:decimal-places", 17 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->addAttribute( "number:min-exponent-digits", 2 );
    xmlWriter->endElement();  // number:scientific-number
    xmlWriter->endElement();  // number:number-style
  }
  else if( valueFormat == "0%")
  {
    xmlWriter->startElement( "number:percentage-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 0 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->startElement( "number:text" );
    xmlWriter->addTextNode( "%" );
    xmlWriter->endElement();  // number:text
    xmlWriter->endElement();  // number:percentage-style
  }
  else if( valueFormat == "0.0%")
  {
    xmlWriter->startElement( "number:percentage-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 1 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->startElement( "number:text" );
    xmlWriter->addTextNode( "%" );
    xmlWriter->endElement();  // number:text
    xmlWriter->endElement();  // number:percentage-style
  }
  else if( valueFormat == "0.00%")
  {
    xmlWriter->startElement( "number:percentage-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 2 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->startElement( "number:text" );
    xmlWriter->addTextNode( "%" );
    xmlWriter->endElement();  // number:text
    xmlWriter->endElement();  // number:percentage-style
  }
  else if( valueFormat == "0.000%")
  {
    xmlWriter->startElement( "number:percentage-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 3 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->startElement( "number:text" );
    xmlWriter->addTextNode( "%" );
    xmlWriter->endElement();  // number:text
    xmlWriter->endElement();  // number:percentage-style
  }
  else if( valueFormat == "0.0000%")
  {
    xmlWriter->startElement( "number:percentage-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 4 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->startElement( "number:text" );
    xmlWriter->addTextNode( "%" );
    xmlWriter->endElement();  // number:text
    xmlWriter->endElement();  // number:percentage-style
  }
  else if( valueFormat == "0.00000%")
  {
    xmlWriter->startElement( "number:percentage-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 5 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->startElement( "number:text" );
    xmlWriter->addTextNode( "%" );
    xmlWriter->endElement();  // number:text
    xmlWriter->endElement();  // number:percentage-style
  }
  else if( valueFormat == "0.000000%")
  {
    xmlWriter->startElement( "number:percentage-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 6 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->startElement( "number:text" );
    xmlWriter->addTextNode( "%" );
    xmlWriter->endElement();  // number:text
    xmlWriter->endElement();  // number:percentage-style
  }
  else if( valueFormat == "0.0000000%")
  {
    xmlWriter->startElement( "number:percentage-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 7 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->startElement( "number:text" );
    xmlWriter->addTextNode( "%" );
    xmlWriter->endElement();  // number:text
    xmlWriter->endElement();  // number:percentage-style
  }
  else if( valueFormat == "0.00000000%")
  {
    xmlWriter->startElement( "number:percentage-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 8 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->startElement( "number:text" );
    xmlWriter->addTextNode( "%" );
    xmlWriter->endElement();  // number:text
    xmlWriter->endElement();  // number:percentage-style
  }
  else if( valueFormat == "0.000000000%")
  {
    xmlWriter->startElement( "number:percentage-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 9 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->startElement( "number:text" );
    xmlWriter->addTextNode( "%" );
    xmlWriter->endElement();  // number:text
    xmlWriter->endElement();  // number:percentage-style
  }
  else if( valueFormat == "0.0000000000%")
  {
    xmlWriter->startElement( "number:percentage-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 10 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->startElement( "number:text" );
    xmlWriter->addTextNode( "%" );
    xmlWriter->endElement();  // number:text
    xmlWriter->endElement();  // number:percentage-style
  }
  else if( valueFormat == "0.00000000000%")
  {
    xmlWriter->startElement( "number:percentage-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 11 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->startElement( "number:text" );
    xmlWriter->addTextNode( "%" );
    xmlWriter->endElement();  // number:text
    xmlWriter->endElement();  // number:percentage-style
  }
  else if( valueFormat == "0.000000000000%")
  {
    xmlWriter->startElement( "number:percentage-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 12 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->startElement( "number:text" );
    xmlWriter->addTextNode( "%" );
    xmlWriter->endElement();  // number:text
    xmlWriter->endElement();  // number:percentage-style
  }
  else if( valueFormat == "0.0000000000000%")
  {
    xmlWriter->startElement( "number:percentage-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 13 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->startElement( "number:text" );
    xmlWriter->addTextNode( "%" );
    xmlWriter->endElement();  // number:text
    xmlWriter->endElement();  // number:percentage-style
  }
  else if( valueFormat == "0.00000000000000%")
  {
    xmlWriter->startElement( "number:percentage-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 14 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->startElement( "number:text" );
    xmlWriter->addTextNode( "%" );
    xmlWriter->endElement();  // number:text
    xmlWriter->endElement();  // number:percentage-style
  }
  else if( valueFormat == "0.000000000000000%")
  {
    xmlWriter->startElement( "number:percentage-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 15 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->startElement( "number:text" );
    xmlWriter->addTextNode( "%" );
    xmlWriter->endElement();  // number:text
    xmlWriter->endElement();  // number:percentage-style
  }
  else if( valueFormat == "0.0000000000000000%")
  {
    xmlWriter->startElement( "number:percentage-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->startElement( "number:number" );
    xmlWriter->addAttribute( "number:decimal-places", 16 );
    xmlWriter->addAttribute( "number:min-integer-digits", 1 );
    xmlWriter->endElement();  // number:number
    xmlWriter->startElement( "number:text" );
    xmlWriter->addTextNode( "%" );
    xmlWriter->endElement();  // number:text
    xmlWriter->endElement();  // number:percentage-style
  }
  else if( valueFormat.toLower() == "m/d/yy")
  {
    xmlWriter->startElement( "number:date-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );

    xmlWriter->startElement( "number:month" );
    xmlWriter->addAttribute( "number:textual", "false" );
    xmlWriter->addAttribute( "number:style", "short" );
    xmlWriter->endElement();  // number:month

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( "/" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:day" );
    xmlWriter->addAttribute( "number:style", "short" );
    xmlWriter->endElement();  // number:day

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( "/" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:year" );
    xmlWriter->addAttribute( "number:style", "short" );
    xmlWriter->endElement();  // number:year

    xmlWriter->endElement();  // number:date-style
  }
  else if( valueFormat.toLower() == "m/d/yyyy")
  {
    xmlWriter->startElement( "number:date-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );

    xmlWriter->startElement( "number:month" );
    xmlWriter->addAttribute( "number:textual", "false" );
    xmlWriter->addAttribute( "number:style", "short" );
    xmlWriter->endElement();  // number:month

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( "/" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:day" );
    xmlWriter->addAttribute( "number:style", "short" );
    xmlWriter->endElement();  // number:day

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( "/" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:year" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:year

    xmlWriter->endElement();  // number:date-style
  }
  else if( (valueFormat.toLower() == "d-mmm-yy") || (valueFormat.toLower() == "d\\-mmm\\-yy") )
  {
    xmlWriter->startElement( "number:date-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );

    xmlWriter->startElement( "number:day" );
    xmlWriter->addAttribute( "number:style", "short" );
    xmlWriter->endElement();  // number:day

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( "-" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:month" );
    xmlWriter->addAttribute( "number:textual", "true" );
    xmlWriter->addAttribute( "number:style", "short" );
    xmlWriter->endElement();  // number:month

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( "-" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:year" );
    xmlWriter->addAttribute( "number:style", "short" );
    xmlWriter->endElement();  // number:year

    xmlWriter->endElement();  // number:date-style
  }
  else if( (valueFormat.toLower() == "d-mmm-yyyy") || (valueFormat.toLower() == "d\\-mmm\\-yyyy") )
  {
    xmlWriter->startElement( "number:date-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );

    xmlWriter->startElement( "number:day" );
    xmlWriter->addAttribute( "number:style", "short" );
    xmlWriter->endElement();  // number:day

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( "-" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:month" );
    xmlWriter->addAttribute( "number:textual", "true" );
    xmlWriter->addAttribute( "number:style", "short" );
    xmlWriter->endElement();  // number:month

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( "-" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:year" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:year

    xmlWriter->endElement();  // number:date-style
  }
  else if( (valueFormat.toLower() == "d-mmm") || (valueFormat.toLower() == "d\\-mmm") )
  {
    xmlWriter->startElement( "number:date-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );

    xmlWriter->startElement( "number:day" );
    xmlWriter->addAttribute( "number:style", "short" );
    xmlWriter->endElement();  // number:day

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( "-" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:month" );
    xmlWriter->addAttribute( "number:textual", "true" );
    xmlWriter->addAttribute( "number:style", "short" );
    xmlWriter->endElement();  // number:month

    xmlWriter->endElement();  // number:date-style
  }
  else if( (valueFormat.toLower() == "d-mm") || (valueFormat.toLower() == "d\\-mm") )
  {
    xmlWriter->startElement( "number:date-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );

    xmlWriter->startElement( "number:day" );
    xmlWriter->addAttribute( "number:style", "short" );
    xmlWriter->endElement();  // number:day

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( "-" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:month" );
    xmlWriter->addAttribute( "number:textual", "false" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:month

    xmlWriter->endElement();  // number:date-style
  }
  else if( valueFormat.toLower() == "mmm/d" )
  {
    xmlWriter->startElement( "number:date-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );

    xmlWriter->startElement( "number:month" );
    xmlWriter->addAttribute( "number:textual", "true" );
    xmlWriter->addAttribute( "number:style", "short" );
    xmlWriter->endElement();  // number:month

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( "/" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:day" );
    xmlWriter->addAttribute( "number:style", "short" );
    xmlWriter->endElement();  // number:day

    xmlWriter->endElement();  // number:date-style
  }
  else if( valueFormat.toLower() == "mmm/dd" )
  {
    xmlWriter->startElement( "number:date-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );

    xmlWriter->startElement( "number:month" );
    xmlWriter->addAttribute( "number:textual", "true" );
    xmlWriter->addAttribute( "number:style", "short" );
    xmlWriter->endElement();  // number:month

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( "/" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:day" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:day

    xmlWriter->endElement();  // number:date-style
  }
  else if( valueFormat.toLower() == "mm/d" )
  {
    xmlWriter->startElement( "number:date-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );

    xmlWriter->startElement( "number:month" );
    xmlWriter->addAttribute( "number:textual", "false" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:month

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( "/" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:day" );
    xmlWriter->addAttribute( "number:style", "short" );
    xmlWriter->endElement();  // number:day

    xmlWriter->endElement();  // number:date-style
  }
  else if( valueFormat.toLower() == "mm/dd" )
  {
    xmlWriter->startElement( "number:date-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );

    xmlWriter->startElement( "number:month" );
    xmlWriter->addAttribute( "number:textual", "false" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:month

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( "/" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:day" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:day

    xmlWriter->endElement();  // number:date-style
  }
  else if( valueFormat.toLower() == "mm/dd/yy" )
  {
    xmlWriter->startElement( "number:date-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );

    xmlWriter->startElement( "number:month" );
    xmlWriter->addAttribute( "number:textual", "false" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:month

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( "/" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:day" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:day

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( "/" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:year" );
    xmlWriter->addAttribute( "number:style", "short" );
    xmlWriter->endElement();  // number:year

    xmlWriter->endElement();  // number:date-style
  }
  else if( valueFormat.toLower() == "mm/dd/yyyy" )
  {
    xmlWriter->startElement( "number:date-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );

    xmlWriter->startElement( "number:month" );
    xmlWriter->addAttribute( "number:textual", "false" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:month

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( "/" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:day" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:day

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( "/" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:year" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:year

    xmlWriter->endElement();  // number:date-style
  }
  else if( valueFormat.toLower() == "yyyy/mm/dd" )
  {
    xmlWriter->startElement( "number:date-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );

    xmlWriter->startElement( "number:year" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:year

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( "/" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:month" );
    xmlWriter->addAttribute( "number:textual", "false" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:month

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( "/" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:day" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:day

    xmlWriter->endElement();  // number:date-style
  }
  else if( valueFormat.toLower() == "yyyy/mm/d" )
  {
    xmlWriter->startElement( "number:date-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );

    xmlWriter->startElement( "number:year" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:year

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( "/" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:month" );
    xmlWriter->addAttribute( "number:textual", "false" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:month

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( "/" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:day" );
    xmlWriter->addAttribute( "number:style", "short" );
    xmlWriter->endElement();  // number:day

    xmlWriter->endElement();  // number:date-style
  }
  else if( (valueFormat.toLower() == "yyyy-mm-dd") || (valueFormat.toLower() == "yyyy\\-mm\\-dd") )
  {
    xmlWriter->startElement( "number:date-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );

    xmlWriter->startElement( "number:year" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:year

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( "-" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:month" );
    xmlWriter->addAttribute( "number:textual", "false" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:month

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( "-" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:day" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:day

    xmlWriter->endElement();  // number:date-style
  }
  else if( (valueFormat.toLower() == "yyyy-mm-d") || (valueFormat.toLower() == "yyyy\\-mm\\-d") )
  {
    xmlWriter->startElement( "number:date-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );

    xmlWriter->startElement( "number:year" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:year

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( "-" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:month" );
    xmlWriter->addAttribute( "number:textual", "false" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:month

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( "-" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:day" );
    xmlWriter->addAttribute( "number:style", "short" );
    xmlWriter->endElement();  // number:day

    xmlWriter->endElement();  // number:date-style
  }

  else if( valueFormat == "h:mm AM/PM" )
  {
    xmlWriter->startElement( "number:time-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );

    xmlWriter->startElement( "number:hours" );
    xmlWriter->addAttribute( "number:style", "short" );
    xmlWriter->endElement();  // number:hour

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( ":" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:minutes" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:minutes

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( " " );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:am-pm" );
    xmlWriter->endElement();  // number:am-pm

    xmlWriter->endElement();  // number:time-style
  }
  else if( valueFormat == "h:mm:ss AM/PM" )
  {
    xmlWriter->startElement( "number:time-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );

    xmlWriter->startElement( "number:hours" );
    xmlWriter->addAttribute( "number:style", "short" );
    xmlWriter->endElement();  // number:hour

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( ":" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:minutes" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:minutes

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( ":" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:seconds" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:minutes

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( " " );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:am-pm" );
    xmlWriter->endElement();  // number:am-pm

    xmlWriter->endElement();  // number:time-style
  }
  else if( valueFormat == "h:mm" )
  {
    xmlWriter->startElement( "number:time-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );

    xmlWriter->startElement( "number:hours" );
    xmlWriter->addAttribute( "number:style", "short" );
    xmlWriter->endElement();  // number:hour

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( ":" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:minutes" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:minutes

    xmlWriter->endElement();  // number:time-style
  }
  else if( valueFormat == "h:mm:ss" )
  {
    xmlWriter->startElement( "number:time-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );

    xmlWriter->startElement( "number:hours" );
    xmlWriter->addAttribute( "number:style", "short" );
    xmlWriter->endElement();  // number:hour

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( ":" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:minutes" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:minutes

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( ":" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:seconds" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:minutes

    xmlWriter->endElement();  // number:time-style
  }
  else if( valueFormat == "[h]:mm" )
  {
    xmlWriter->startElement( "number:time-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->addAttribute( "number:truncate-on-overflow", "false" );

    xmlWriter->startElement( "number:hours" );
    xmlWriter->addAttribute( "number:style", "short" );
    xmlWriter->endElement();  // number:hour

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( ":" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:minutes" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:minutes

    xmlWriter->endElement();  // number:time-style
  }
  else if( valueFormat == "[h]:mm:ss" )
  {
    xmlWriter->startElement( "number:time-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->addAttribute( "number:truncate-on-overflow", "false" );

    xmlWriter->startElement( "number:hours" );
    xmlWriter->addAttribute( "number:style", "short" );
    xmlWriter->endElement();  // number:hour

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( ":" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:minutes" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:minutes

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( ":" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:seconds" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:minutes

    xmlWriter->endElement();  // number:time-style
  }
  else if( valueFormat == "mm:ss" )
  {
    xmlWriter->startElement( "number:time-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );

    xmlWriter->startElement( "number:minutes" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:minutes

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( ":" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:seconds" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:minutes

    xmlWriter->endElement();  // number:time-style
  }
  else if( valueFormat == "mm:ss.0" )
  {
    xmlWriter->startElement( "number:time-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );

    xmlWriter->startElement( "number:minutes" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:minutes

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( ":" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:seconds" );
    xmlWriter->addAttribute( "number:style", "long" );

    xmlWriter->endElement();  // number:minutes
    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( ".0" );
    xmlWriter->endElement();  // number:text


    xmlWriter->endElement();  // number:time-style
  }
  else if( valueFormat == "[mm]:ss" )
  {
    xmlWriter->startElement( "number:time-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->addAttribute( "number:truncate-on-overflow", "false" );

    xmlWriter->startElement( "number:minutes" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:minutes

    xmlWriter->startElement( "number:text");
    xmlWriter->addTextNode( ":" );
    xmlWriter->endElement();  // number:text

    xmlWriter->startElement( "number:seconds" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:minutes

    xmlWriter->endElement();  // number:time-style
  }
  else if( valueFormat == "[ss]" )
  {
    xmlWriter->startElement( "number:time-style" );
    xmlWriter->addAttribute( "style:name", refName );
    xmlWriter->addAttribute( "style:family", "data-style" );
    xmlWriter->addAttribute( "number:truncate-on-overflow", "false" );

    xmlWriter->startElement( "number:seconds" );
    xmlWriter->addAttribute( "number:style", "long" );
    xmlWriter->endElement();  // number:minutes

    xmlWriter->endElement();  // number:time-style
  }

}
