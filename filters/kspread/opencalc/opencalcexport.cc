/* This file is part of the KDE project
   Copyright (C) 2000 David Faure <faure@kde.org>
   Copyright (C) 2000 Norbert Andres <nandres@web.de>

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

#include <opencalcexport.h>

#include <qdom.h>
#include <qfile.h>

#include <kdebug.h>
#include <kmessagebox.h>
#include <kgenericfactory.h>
#include <koDocumentInfo.h>
#include <koFilterChain.h>

#include <kspread_aboutdata.h>
#include <kspread_cell.h>
#include <kspread_doc.h>
#include <kspread_format.h>
#include <kspread_map.h>
#include <kspread_sheet.h>

typedef KGenericFactory<OpenCalcExport, KoFilter> OpenCalcExportFactory;
K_EXPORT_COMPONENT_FACTORY( libopencalcexport, 
                            OpenCalcExportFactory( "opencalcfilter" ) );

#define STOPEXPORT \
  do \
  { \
    delete store; \
    return false; \
  } while(0)

OpenCalcExport::OpenCalcExport(KoFilter *, const char *, const QStringList&) :
                     KoFilter() 
{
}

KoFilter::ConversionStatus OpenCalcExport::convert( const QCString & from, 
                                                    const QCString & to )
{
  /* later...
     KSpreadLeader  * leader = new KSpreadLeader( m_chain );
     OpenCalcWorker * worker = new OpenCalcWorker();
     leader->setWorker( worker );
     
     KoFilter::ConversionStatus status = leader->convert();
     
     delete worker;
     delete leader;
     
     return status;
  */

  KoDocument * document = m_chain->inputDocument();

  if ( !document )
    return KoFilter::StupidError;

  if ( strcmp(document->className(), "KSpreadDoc") != 0)
  {
    kdWarning(30501) << "document isn't a KSpreadDoc but a " 
                     << document->className() << endl;
    return KoFilter::NotImplemented;
  }

  if ( ( to != "application/x-opencalc") || (from != "application/x-kspread" ) )
  {
    kdWarning(30501) << "Invalid mimetypes " << to << " " << from << endl;
    return KoFilter::NotImplemented;
  }

  KSpreadDoc const * const ksdoc = static_cast<const KSpreadDoc *>(document);

  if ( ksdoc->mimeType() != "application/x-kspread" )
  {
    kdWarning(30501) << "Invalid document mimetype " << ksdoc->mimeType() << endl;
    return KoFilter::NotImplemented;
  }

  if ( !writeFile( ksdoc ) )
    return KoFilter::CreationError;

  emit sigProgress( 100 );
  
  return KoFilter::OK;
}

bool OpenCalcExport::writeFile( KSpreadDoc const * const ksdoc )
{
  kdDebug() << "writeFile()" << endl;
  KoStore * store = KoStore::createStore( m_chain->outputFile(), KoStore::Write, "", KoStore::Zip );

  if ( !store )
    return false;

  uint filesWritten = 0;

  if ( !exportContent( store, ksdoc ) )
    STOPEXPORT;
  else
    filesWritten |= contentXML;    

  kdDebug() << "Content exported" << endl;

  // TODO: pass sheet number and cell number
  if ( !exportDocInfo( store, ksdoc ) )
    STOPEXPORT;
  else
    filesWritten |= metaXML;

  kdDebug() << "Document Info exported" << endl;
  /*
  if ( !exportStyles( store, ksdoc ) )
    STOPEXPORT;
  else
    filesWritten |= stylesXML;

  kdDebug() << "Styles exported" << endl;
  */

  if ( !writeMetaFile( store, filesWritten ) )
    STOPEXPORT;

  // writes zip file to disc
  delete store;
  store = 0;

  return true;
}

bool OpenCalcExport::exportDocInfo( KoStore * store, KSpreadDoc const * const ksdoc )
{
  kdDebug() << "exportDocInfo()" << endl;
  if ( !store->open( "meta.xml" ) )
    return false;

  KoDocumentInfo       * docInfo    = ksdoc->documentInfo();
  KoDocumentInfoAbout  * aboutPage  = static_cast<KoDocumentInfoAbout *>( docInfo->page( "about" ) );
  KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor*>( docInfo->page( "author" ) );

  QDomDocument meta;
  meta.appendChild( meta.createProcessingInstruction( "xml","version=\"1.0\" encoding=\"UTF-8\"" ) );

  QDomElement content = meta.createElement( "office:document-meta" );
  content.setAttribute( "xmlns:office", "http://openoffice.org/2000/office");
  content.setAttribute( "xmlns:xlink", "http://www.w3.org/1999/xlink" );
  content.setAttribute( "xmlns:dc", "http://purl.org/dc/elements/1.1/" );
  content.setAttribute( "xmlns:meta", "http://openoffice.org/2000/meta" );
  content.setAttribute( "office:version", "1.0" );

  QDomNode officeMeta = meta.createElement( "office:meta" );

  QDomElement data = meta.createElement( "meta:generator" );
  QString app( "KSpread " );
  app += version;
  data.appendChild( meta.createTextNode( app ) );
  officeMeta.appendChild( data );

  data = meta.createElement( "meta:initial-creator" );
  data.appendChild( meta.createTextNode( authorPage->fullName() ) );
  officeMeta.appendChild( data );
  
  data = meta.createElement( "meta:creator" );
  data.appendChild( meta.createTextNode( authorPage->fullName() ) );
  officeMeta.appendChild( data );

  data = meta.createElement( "meta:user-defined" );
  data.setAttribute( "meta:name", "Info 1" );
  data.appendChild( meta.createTextNode( aboutPage->title() ) );
  officeMeta.appendChild( data );

  data = meta.createElement( "meta:user-defined" );
  data.setAttribute( "meta:name", "Info 2" );
  data.appendChild( meta.createTextNode( aboutPage->abstract() ) );
  officeMeta.appendChild( data );

  /* TODO:
    <meta:creation-date>2003-01-08T23:57:31</meta:creation-date>
    <dc:date>2003-01-08T23:58:05</dc:date>
    <dc:language>en-US</dc:language>
    <meta:editing-cycles>2</meta:editing-cycles>
    <meta:editing-duration>PT38S</meta:editing-duration>
    <meta:user-defined meta:name="Info 3"/>
    <meta:user-defined meta:name="Info 4"/>
  */

  data = meta.createElement( "meta:document-statistic" );
  data.setAttribute( "meta:table-count", QString::number( ksdoc->map()->count() ) );
  //  TODO: data.setAttribute( "meta:cell-count",  );
  officeMeta.appendChild( data );

  content.appendChild( officeMeta );
  meta.appendChild( content );

  QCString doc( meta.toCString() );
  kdDebug() << "Meta: " << doc << endl;

  store->write( doc, doc.length() );

  if ( !store->close() )
    return false;

  return true;
}

bool OpenCalcExport::exportContent( KoStore * store, KSpreadDoc const * const ksdoc )
{
  kdDebug() << "exportContent()" << endl;
  if ( !store->open( "content.xml" ) )
    return false;

  QDomDocument doc;
  doc.appendChild( doc.createProcessingInstruction( "xml","version=\"1.0\" encoding=\"UTF-8\"" ) );

  QDomElement content = doc.createElement( "office:document-content" );
  content.setAttribute( "xmlns:office", "http://openoffice.org/2000/office");
  content.setAttribute( "xmlns:style", "http://openoffice.org/2000/style" );
  content.setAttribute( "xmlns:text", "http://openoffice.org/2000/text" );
  content.setAttribute( "xmlns:table", "http://openoffice.org/2000/table" );
  content.setAttribute( "xmlns:draw", "http://openoffice.org/2000/drawing" );
  content.setAttribute( "xmlns:fo", "http://www.w3.org/1999/XSL/Format" );
  content.setAttribute( "xmlns:xlink", "http://www.w3.org/1999/xlink" );
  content.setAttribute( "xmlns:number", "http://openoffice.org/2000/datastyle" );
  content.setAttribute( "xmlns:svg", "http://www.w3.org/2000/svg" );
  content.setAttribute( "xmlns:chart", "http://openoffice.org/2000/chart" );
  content.setAttribute( "xmlns:dr3d", "http://openoffice.org/2000/dr3d" );
  content.setAttribute( "xmlns:math", "http://www.w3.org/1998/Math/MathML" );
  content.setAttribute( "xmlns:form", "http://openoffice.org/2000/form" );
  content.setAttribute( "xmlns:script", "http://openoffice.org/2000/script" );
  content.setAttribute( "office:class", "spreadsheet" );
  content.setAttribute( "office:version", "1.0" );

  QDomElement data = doc.createElement( "office:script" );
  content.appendChild( data );

  if ( !exportBody( doc, content, ksdoc ) )
    return false;

  doc.appendChild( content );

  QCString f( doc.toCString() );
  kdDebug() << "Content: " << (char const * ) f << endl;

  store->write( f, f.length() );

  if ( !store->close() )
    return false;

  kdDebug() << "exit exportContent()" << endl;

  return true;
}

bool OpenCalcExport::exportBody( QDomDocument & doc, QDomElement & content, KSpreadDoc const * const ksdoc )
{
  kdDebug() << "exportBody" << endl;
  QDomElement fontDecls  = doc.createElement( "office:font-decls" );
  QDomElement autoStyles = doc.createElement( "office:automatic-styles" );
  QDomElement body       = doc.createElement( "office:body" );

  QPtrListIterator<KSpreadSheet> it( ksdoc->map()->tableList() );

  for( ; it.current(); ++it )
  {
    TableStyle ts;
    int maxCols         = 0;
    int maxRows         = 0;
    KSpreadSheet * sheet = it.current();

    ts.visible = sheet->isHidden();

    QDomElement tabElem = doc.createElement( "table:table" );
    tabElem.setAttribute( "table:style-name", m_styles.tableStyle( ts ) );
    tabElem.setAttribute( "table:name", sheet->tableName() );

    maxRowCols( sheet, maxCols, maxRows );

    exportSheet( doc, tabElem, sheet, maxCols, maxRows );
    
    body.appendChild( tabElem );
  }

  m_styles.writeStyles( doc, autoStyles );

  content.appendChild( fontDecls );
  content.appendChild( autoStyles );
  content.appendChild( body );

  return true;
}

void OpenCalcExport::exportSheet( QDomDocument & doc, QDomElement & tabElem, 
                                  KSpreadSheet const * const sheet, int maxCols, int maxRows )
{
  kdDebug() << "exportSheet: " << sheet->tableName() << endl;
  int i = 1;

  // TODO: handle empty sheets
  for ( ; i <= maxCols; ++i )
  {
    ColumnFormat const * const column = sheet->columnFormat( i );
    ColumnStyle cs;
    cs.breakB = Style::automatic;
    cs.size   = column->mmWidth() / 10;

    QDomElement colElem = doc.createElement( "table:table-column" );
    colElem.setAttribute( "table:style-name", m_styles.columnStyle( cs ) );
    colElem.setAttribute( "table:default-cell-style-name", "Default" );
    // TODO: colElem.setAttribute( "table:number-columns-repeated", x );

    tabElem.appendChild( colElem );
  }

  for ( i = 1; i <= maxRows; ++i )
  {
    RowFormat const * const row = sheet->rowFormat( i );
    RowStyle rs;
    rs.breakB = Style::automatic;
    rs.size   = row->mmHeight() / 10;

    QDomElement rowElem = doc.createElement( "table:table-row" );
    rowElem.setAttribute( "table:style-name", m_styles.rowStyle( rs ) );

    exportCells( doc, rowElem, sheet, i, maxCols );

    tabElem.appendChild( rowElem );
  }
}

void OpenCalcExport::exportCells( QDomDocument & doc, QDomElement & rowElem, 
                                  KSpreadSheet const * const sheet, int row, int maxCols )
{
  kdDebug() << "exportCells" << endl;
  int i = 1;
  for ( ; i <= maxCols; ++i )
  {
    KSpreadCell const * const cell = sheet->cellAt( i, row );
    QDomElement cellElem = doc.createElement( "table:table-cell" );

    KSpreadValue const value( cell->value() );

    if ( value.isBoolean() )
    {
      cellElem.setAttribute( "table:value-type", "boolean" );
      cellElem.setAttribute( "table:boolean-value", ( value.asBoolean() ? "true" : "false" ) );
    }
    else if ( value.isNumber() )
    {
      KSpreadFormat::FormatType type = cell->getFormatType( i, row );

      if ( ( type == KSpreadFormat::Percentage ) 
           || ( type == KSpreadFormat::CustomPercentage ) )
        cellElem.setAttribute( "table:value-type", "percentage" );
      else
        cellElem.setAttribute( "table:value-type", "float" );

      cellElem.setAttribute( "table:value", QString::number( value.asFloat() ) );
    }      
    else
    {
      kdDebug() << "Type: " << value.type() << endl;
    }

    if ( cell->isFormula() )
    {
      // TODO:
      // QString formula( convertFormula( cell->text() ) );
      // cellElem.setAttribute( "table:formula", formula );
    }

    if ( !cell->isEmpty() )
    {
      kdDebug() << "Writing cell content" << endl;
      QDomElement textElem = doc.createElement( "text:p" );
      textElem.appendChild( doc.createTextNode( cell->strOutText() ) );

      cellElem.appendChild( textElem );
    }
    kdDebug() << "Cell: " << cell->strOutText() << endl;

    rowElem.appendChild( cellElem );
  }
}

void OpenCalcExport::maxRowCols( KSpreadSheet const * const sheet, 
                                 int & maxCols, int & maxRows )
{
  KSpreadCell const * cell = sheet->firstCell();

  while ( cell )
  {
    if ( cell->column() > maxCols )
      maxCols = cell->column();

    if ( cell->row() > maxRows )
      maxRows = cell->row();

    cell = cell->nextCell();
  }
}

bool OpenCalcExport::exportStyles( KoStore * store, KSpreadDoc const * const ksdoc )
{
  return false;

  if ( !store->open( "styles.xml" ) )
    return false;

  // TODO

  if ( !store->close() )
    return false;

  return false;
}

bool OpenCalcExport::writeMetaFile( KoStore * store, uint filesWritten )
{
  if ( !store->open( "manifest.xml" ) )
    return false;

  QDomImplementation impl;
  QDomDocumentType type( impl.createDocumentType( "manifest:manifest", "-//OpenOffice.org//DTD Manifest 1.0//EN", "Manifest.dtd" ) );

  QDomDocument meta( type );
  meta.appendChild( meta.createProcessingInstruction( "xml","version=\"1.0\" encoding=\"UTF-8\"" ) );
  
  QDomElement content = meta.createElement( "manifest:manifest" );
  content.setAttribute( "xmlns:manifest", "http://openoffice.org/2001/manifest" );

  QDomElement entry = meta.createElement( "manifest:file-entry" );
  entry.setAttribute( "manifest:media-type", "application/vnd.sun.xml.calc" );
  entry.setAttribute( "manifest:full-path", "/" );
  content.appendChild( entry );

  entry = meta.createElement( "manifest:file-entry" );
  entry.setAttribute( "manifest:media-type", "" );
  entry.setAttribute( "manifest:full-path", "Pictures/" );
  content.appendChild( entry );

  if ( filesWritten & contentXML )
  {
    entry = meta.createElement( "manifest:file-entry" );
    entry.setAttribute( "manifest:media-type", "text/xml" );
    entry.setAttribute( "manifest:full-path", "content.xml" );
    content.appendChild( entry );
  }

  if ( filesWritten & stylesXML )
  {
    entry = meta.createElement( "manifest:file-entry" );
    entry.setAttribute( "manifest:media-type", "text/xml" );
    entry.setAttribute( "manifest:full-path", "styles.xml" );
    content.appendChild( entry );
  }

  if ( filesWritten & metaXML )
  {
    entry = meta.createElement( "manifest:file-entry" );
    entry.setAttribute( "manifest:media-type", "text/xml" );
    entry.setAttribute( "manifest:full-path", "meta.xml" );
    content.appendChild( entry );
  }

  if ( filesWritten & settingsXML )
  {
    entry = meta.createElement( "manifest:file-entry" );
    entry.setAttribute( "manifest:media-type", "text/xml" );
    entry.setAttribute( "manifest:full-path", "settings.xml" );
    content.appendChild( entry );
  }

  meta.appendChild( content );

  QCString doc( meta.toCString() );
  kdDebug() << "Manifest: " << doc << endl;

  store->write( doc, doc.length() );

  if ( !store->close() )
    return false;

  return true;
}

#include <opencalcexport.moc>
