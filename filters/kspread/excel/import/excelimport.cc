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

#include <qbuffer.h>
#include <qcstring.h>
#include <qfile.h>
#include <qstring.h>
#include <qtextstream.h>

#include <kdebug.h>
#include <koFilterChain.h>
#include <koGlobal.h>
#include <koUnit.h>
#include <kgenericfactory.h>

#include <KoXmlWriter.h>

#include "swinder.h"
#include <iostream>

typedef KGenericFactory<ExcelImport, KoFilter> ExcelImportFactory;
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

  QByteArray createStyles();
  QByteArray createContent();
  QByteArray createManifest();
  
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
  void processFormat( Format* format, KoXmlWriter* xmlWriter );
};


ExcelImport::ExcelImport ( QObject*, const char*, const QStringList& )
    : KoFilter()
{
  d = new Private;
}

ExcelImport::~ExcelImport()
{
  delete d;
}

KoFilter::ConversionStatus ExcelImport::convert( const QCString& from, const QCString& to )
{
  if ( from != "application/msexcel" )
    return KoFilter::NotImplemented; 

  if ( to != "application/vnd.oasis.opendocument.spreadsheet" )     
    return KoFilter::NotImplemented;

  d->inputFile = m_chain->inputFile();
  d->outputFile = m_chain->outputFile();

  // open inputFile
  d->workbook = new Swinder::Workbook;
  if( d->workbook->load( d->inputFile.local8Bit() ) )
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
    kdWarning() << "Couldn't open the requested file." << endl;
    return KoFilter::FileNotFound;
  }

  // store document styles
  d->sheetFormatIndex = 1;
  d->columnFormatIndex = 1;
  d->rowFormatIndex = 1;
  d->cellFormatIndex = 1;
  if ( !storeout->open( "styles.xml" ) )
  {
    kdWarning() << "Couldn't open the file 'styles.xml'." << endl;
    return KoFilter::CreationError;
  }
  storeout->write( d->createStyles() );
  storeout->close();

  // store document content
  d->sheetFormatIndex = 1;
  d->columnFormatIndex = 1;
  d->rowFormatIndex = 1;
  d->cellFormatIndex = 1;
  if ( !storeout->open( "content.xml" ) )
  {
    kdWarning() << "Couldn't open the file 'content.xml'." << endl;
    return KoFilter::CreationError;
  }
  storeout->write( d->createContent() );
  storeout->close();

  // store document manifest
  storeout->enterDirectory( "META-INF" );
  if ( !storeout->open( "manifest.xml" ) )
  {
     kdWarning() << "Couldn't open the file 'META-INF/manifest.xml'." << endl;
     return KoFilter::CreationError;
  }
  storeout->write( d->createManifest() );
  storeout->close();

  // we are done!
  delete d->workbook;
  delete storeout;
  d->inputFile = QString::null;
  d->outputFile = QString::null;
  d->workbook = 0;

  return KoFilter::OK;
}

QByteArray ExcelImport::Private::createContent()
{
  KoXmlWriter* contentWriter;
  QByteArray contentData;
  QBuffer contentBuffer( contentData );

  contentBuffer.open( IO_WriteOnly );
  contentWriter = new KoXmlWriter( &contentBuffer );

  contentWriter->startDocument( "office:document-content" );
  contentWriter->startElement( "office:document-content" );
  
  contentWriter->addAttribute( "xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0" );
  contentWriter->addAttribute( "xmlns:style", "urn:oasis:names:tc:opendocument:xmlns:style:1.0" );
  contentWriter->addAttribute( "xmlns:text", "urn:oasis:names:tc:opendocument:xmlns:text:1.0" );
  contentWriter->addAttribute( "xmlns:table", "urn:oasis:names:tc:opendocument:xmlns:table:1.0" );
  contentWriter->addAttribute( "xmlns:draw", "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0" );
  contentWriter->addAttribute( "xmlns:fo", "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0" );
  contentWriter->addAttribute( "xmlns:svg","urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0" );
  contentWriter->addAttribute( "office:version","1.0" );

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
  sheetFormatIndex = 1;
  columnFormatIndex = 1;
  rowFormatIndex = 1;
  cellFormatIndex = 1;
  contentWriter->startElement( "office:automatic-styles" );
  processWorkbookForStyle( workbook, contentWriter );
  contentWriter->endElement(); // office:automatic-style

  // office:body
  sheetFormatIndex = 1;
  columnFormatIndex = 1;
  rowFormatIndex = 1;
  cellFormatIndex = 1;
  contentWriter->startElement( "office:body" );
  processWorkbookForBody( workbook, contentWriter );
  contentWriter->endElement();  // office:body
  
  contentWriter->endElement();  // office:document-content
  contentWriter->endDocument();
  
  delete contentWriter;

  // for troubleshooting only !!
  QString dbg;
  for( unsigned i=0; i<contentData.size(); i++ )
    dbg.append( contentData[i] );
  printf("\ncontent.xml:\n%s\n", dbg.latin1() );

  return contentData;
}

QByteArray ExcelImport::Private::createStyles()
{
  KoXmlWriter* stylesWriter;
  QByteArray stylesData;
  QBuffer stylesBuffer( stylesData );

  stylesBuffer.open( IO_WriteOnly );
  stylesWriter = new KoXmlWriter( &stylesBuffer );

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
  stylesWriter->endElement(); // office:styles
  stylesWriter->startElement( "style:style" );
  stylesWriter->addAttribute( "style:name", "Default" );
  stylesWriter->addAttribute( "style:family", "table-cell" );
  stylesWriter->endElement(); // style:style
  
  // office:automatic-styles
  stylesWriter->startElement( "office:automatic-styles" );
  stylesWriter->endElement(); // office:automatic-styles

  stylesWriter->endElement();  // office:document-styles
  stylesWriter->endDocument();
  
  delete stylesWriter;

  // for troubleshooting only !!
  QString dbg;
  for( unsigned i=0; i<stylesData.size(); i++ )
    dbg.append( stylesData[i] );
  printf("\nstyles.xml:\n%s\n", dbg.latin1() );

  return stylesData;
}

QByteArray ExcelImport::Private::createManifest()
{
  KoXmlWriter* manifestWriter;
  QByteArray manifestData;
  QBuffer manifestBuffer( manifestData );

  manifestBuffer.open( IO_WriteOnly );
  manifestWriter = new KoXmlWriter( &manifestBuffer );
  
  manifestWriter->startDocument( "manifest:manifest" );
  manifestWriter->startElement( "manifest:manifest" );
  manifestWriter->addAttribute( "xmlns:manifest", "urn:oasis:names:tc:openoffice:xmlns:manifest:1.0" );
  manifestWriter->addManifestEntry( "/", "application/vnd.oasis.opendocument.spreadsheet" );
  manifestWriter->addManifestEntry( "styles.xml", "text/xml" );
  manifestWriter->addManifestEntry( "content.xml", "text/xml" );
  manifestWriter->endElement();
  manifestWriter->endDocument();
  delete manifestWriter;

  // for troubleshooting only !!
  QString dbg;
  for( unsigned i=0; i<manifestData.size(); i++ )
    dbg.append( manifestData[i] );
  printf("\nmanifest.xml:\n%s\n", dbg.latin1() );

  return manifestData;
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
  
  int ci = 0;
  while( ci <= sheet->maxColumn() )
  {
    Column* column = sheet->column( ci, false );
    if( column )
    {
      // forward search for columns with same properties
      int cj = ci + 1;
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
    else
      ci++;
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

  int ci = 0;
  while( ci <= sheet->maxColumn() )
  {
    Column* column = sheet->column( ci, false );
    if( column )
    {
      // forward search for similar column
      int cj = ci + 1;
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
  xmlWriter->addAttribute( "table:style-name", QString("co%1").arg(columnFormatIndex) );  
  columnFormatIndex++;

  xmlWriter->endElement();  // table:table-column
}

void ExcelImport::Private::processColumnForStyle( Column* column, int repeat, KoXmlWriter* xmlWriter )
{
  if( !column ) return;
  if( !xmlWriter ) return;
  
  xmlWriter->startElement( "style:style" );
  xmlWriter->addAttribute( "style:family", "table-column" );  
  if(repeat > 1) xmlWriter->addAttribute( "table:number-columns-repeated", repeat );  
  xmlWriter->addAttribute( "style:name", QString("co%1").arg(columnFormatIndex) );  
  columnFormatIndex++;
  
  xmlWriter->startElement( "style:table-column-properties" );
  xmlWriter->addAttribute( "fo:break-before", "auto" );
  xmlWriter->addAttribute( "style:column-width", QString("%1in").arg(column->width()/27) );
  xmlWriter->endElement();  // style:table-column-properties
  
  xmlWriter->endElement();  // style:style
}

void ExcelImport::Private::processRowForBody( Row* row, int repeat, KoXmlWriter* xmlWriter )
{
  if( !row ) return;
  if( !row->sheet() ) return;
  if( !xmlWriter ) return;
  
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

void ExcelImport::Private::processCellForBody( Cell* cell, KoXmlWriter* xmlWriter )
{
  if( !cell ) return;
  if( !xmlWriter ) return;
  
  xmlWriter->startElement( "table:table-cell" );
  xmlWriter->addAttribute( "table:style-name", QString("ce%1").arg(cellFormatIndex) );  
  cellFormatIndex++;
  
  Value value = cell->value();
  
  if( value.isBoolean() )
  {
    xmlWriter->addAttribute( "office:value-type", "boolean" );
    xmlWriter->addAttribute( "office:boolean-value", value.asBoolean() ? "true" : "false" );
  }
  else if( value.isFloat() )
  {
    xmlWriter->addAttribute( "office:value-type", "float" );
    xmlWriter->addAttribute( "office:value", QString::number( value.asFloat() ) );
  }
  else if( value.isInteger() )
  {
    xmlWriter->addAttribute( "office:value-type", "float" );
    xmlWriter->addAttribute( "office:value", QString::number( value.asInteger() ) );
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
  
  xmlWriter->startElement( "style:style" );
  xmlWriter->addAttribute( "style:family", "table-cell" );  
  xmlWriter->addAttribute( "style:name", QString("ce%1").arg(cellFormatIndex) );  
  cellFormatIndex++;
  
  Format format = cell->format();
  // TODO optimize with hash table
  processFormat( &format, xmlWriter );
  
  xmlWriter->endElement();  // style:style
}

void ExcelImport::Private::processFormat( Format* format, KoXmlWriter* xmlWriter )
{
  if( !format ) return;
  if( !xmlWriter ) return;
  
  FormatFont font = format->font();
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

  if( !font.fontFamily().isEmpty() )
    xmlWriter->addAttribute( "style:font-name", string(font.fontFamily()).string() );
    
  xmlWriter->addAttribute( "fo:font-size", QString("%1pt").arg(font.fontSize()) );  
  
  xmlWriter->endElement();  // style:text-properties
}
