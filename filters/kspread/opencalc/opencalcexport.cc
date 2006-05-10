/* This file is part of the KDE project
   Copyright (C) 2000 David Faure <faure@kde.org>
   Copyright (C) 2000 Norbert Andres <nandres@web.de>
   Copyright (C) 2005 Laurent Montel <montel@kde.org>

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

#include <float.h>
#include <math.h>

#include <opencalcexport.h>

#include <qdatetime.h>
#include <qdom.h>
#include <QFile>
#include <QRegExp>
#include <q3valuelist.h>
//Added by qt3to4:
#include <Q3CString>

#include <kdebug.h>
#include <kmessagebox.h>
#include <kcodecs.h>
#include <kgenericfactory.h>
#include <klocale.h>

#include <KoDocumentInfo.h>
#include <KoFilterChain.h>
#include <KoGlobal.h>

#include <kspread_aboutdata.h>
#include <kspread_cell.h>
#include <kspread_doc.h>
//#include <kspread_format.h>
#include <kspread_map.h>
#include <kspread_view.h>
#include <kspread_canvas.h>
#include <kspread_sheet.h>
#include <kspread_sheetprint.h>
#include <kspread_style.h>
#include <kspread_style_manager.h>
#include <kspread_util.h>

using namespace KSpread;

typedef Q3ValueList<Reference> AreaList;

class OpenCalcExportFactory : KGenericFactory<OpenCalcExport>
{
public:
    OpenCalcExportFactory(void) : KGenericFactory<OpenCalcExport> ("kspreadopencalcexport")
    {}
protected:
    virtual void setupTranslations( void )
    {
        KGlobal::locale()->insertCatalog( "kofficefilters" );
    }
};

K_EXPORT_COMPONENT_FACTORY( libopencalcexport, OpenCalcExportFactory() )

#define STOPEXPORT \
  do \
  { \
    delete store; \
    return false; \
  } while(0)

OpenCalcExport::OpenCalcExport( QObject* parent, const QStringList & )
  : KoFilter(parent), m_locale( 0 )
{
}

KoFilter::ConversionStatus OpenCalcExport::convert( const QByteArray & from,
                                                    const QByteArray & to )
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

  if (  !qobject_cast<const KSpread::Doc *>( document ) )
  {
    kWarning(30518) << "document isn't a KSpread::Doc but a "
        << document->metaObject()->className() << endl;
    return KoFilter::NotImplemented;
  }

  if ( ( to != "application/vnd.sun.xml.calc") || (from != "application/x-kspread" ) )
  {
    kWarning(30518) << "Invalid mimetypes " << to << " " << from << endl;
    return KoFilter::NotImplemented;
  }

  const Doc * ksdoc = static_cast<const Doc *>(document);

  if ( ksdoc->mimeType() != "application/x-kspread" )
  {
    kWarning(30518) << "Invalid document mimetype " << ksdoc->mimeType() << endl;
    return KoFilter::NotImplemented;
  }

  m_locale = static_cast<Doc*>(document)->locale();
  if ( !writeFile( ksdoc ) )
    return KoFilter::CreationError;

  emit sigProgress( 100 );

  return KoFilter::OK;
}

bool OpenCalcExport::writeFile( const Doc * ksdoc )
{
  KoStore * store = KoStore::createStore( m_chain->outputFile(), KoStore::Write, "", KoStore::Zip );

  if ( !store )
    return false;

  uint filesWritten = 0;

  if ( !exportContent( store, ksdoc ) )
    STOPEXPORT;
  else
    filesWritten |= contentXML;

  // TODO: pass sheet number and cell number
  if ( !exportDocInfo( store, ksdoc ) )
    STOPEXPORT;
  else
    filesWritten |= metaXML;

  if ( !exportStyles( store, ksdoc ) )
    STOPEXPORT;
  else
    filesWritten |= stylesXML;

  if ( !exportSettings( store, ksdoc ) )
    STOPEXPORT;
  else
    filesWritten |= settingsXML;

  if ( !writeMetaFile( store, filesWritten ) )
    STOPEXPORT;

  // writes zip file to disc
  delete store;
  store = 0;

  return true;
}

bool OpenCalcExport::exportDocInfo( KoStore * store, const Doc* ksdoc )
{
  if ( !store->open( "meta.xml" ) )
    return false;

  KoDocumentInfo       * docInfo    = ksdoc->documentInfo();

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
  data.appendChild( meta.createTextNode( docInfo->aboutInfo("initial-creator") ) );
  officeMeta.appendChild( data );

  data = meta.createElement( "meta:creator" );
  data.appendChild( meta.createTextNode( docInfo->authorInfo("creator") ) );
  officeMeta.appendChild( data );

  data = meta.createElement( "dc:description" );
  data.appendChild( meta.createTextNode( docInfo->aboutInfo("description") ) );
  officeMeta.appendChild( data );

  data = meta.createElement( "meta:keywords" );
  QDomElement dataItem = meta.createElement( "meta:keyword" );
  dataItem.appendChild( meta.createTextNode( docInfo->aboutInfo("keyword") ) );
  data.appendChild( dataItem );
  officeMeta.appendChild( data );

  data = meta.createElement( "dc:title" );
  data.appendChild( meta.createTextNode( docInfo->aboutInfo("title") ) );
  officeMeta.appendChild( data );

  data = meta.createElement( "dc:subject" );
  data.appendChild( meta.createTextNode( docInfo->aboutInfo("subject") ) );
  officeMeta.appendChild( data );

  const QDateTime dt ( QDateTime::currentDateTime() );
  if ( dt.isValid() )
  {
    data = meta.createElement( "dc:date" );
    data.appendChild( meta.createTextNode( dt.toString( Qt::ISODate ) ) );
    officeMeta.appendChild( data );
  }

  /* TODO:
    <meta:creation-date>2003-01-08T23:57:31</meta:creation-date>
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

  QByteArray doc( meta.toByteArray() );
  kDebug(30518) << "Meta: " << doc << endl;

  store->write( doc, doc.length() );

  if ( !store->close() )
    return false;

  return true;
}

bool OpenCalcExport::exportSettings( KoStore * store, const Doc * ksdoc )
{
  if ( !store->open( "settings.xml" ) )
    return false;

  QDomDocument doc;
  doc.appendChild( doc.createProcessingInstruction( "xml","version=\"1.0\" encoding=\"UTF-8\"" ) );

  QDomElement settings = doc.createElement( "office:document-settings" );
  settings.setAttribute( "xmlns:office", "http://openoffice.org/2000/office");
  settings.setAttribute( "xmlns:xlink", "http://www.w3.org/1999/xlink" );
  settings.setAttribute( "xmlns:config", "http://openoffice.org/2001/config" );
  settings.setAttribute( "office:version", "1.0" );

  QDomElement begin = doc.createElement( "office:settings" );

  QDomElement configItem = doc.createElement("config:config-item-set" );
  configItem.setAttribute( "config:name", "view-settings" );

  QDomElement mapIndexed = doc.createElement( "config:config-item-map-indexed" );
  mapIndexed.setAttribute("config:name", "Views" );
  configItem.appendChild( mapIndexed );

  QDomElement mapItem = doc.createElement("config:config-item-map-entry" );

  QDomElement attribute =  doc.createElement("config:config-item" );
  attribute.setAttribute( "config:name", "ActiveTable" );
  attribute.setAttribute( "config:type", "string" );

  View * view = static_cast<View*>( ksdoc->views().first());
  QString activeTable;
  if ( view ) // no view if embedded document
  {
      Canvas * canvas = view->canvasWidget();
      activeTable = canvas->activeSheet()->sheetName();
      // save current sheet selection before to save marker, otherwise current pos is not saved
      view->saveCurrentSheetSelection();
  }
  attribute.appendChild( doc.createTextNode( activeTable ) );
  mapItem.appendChild( attribute );

  QDomElement configmaped = doc.createElement( "config:config-item-map-named" );
  configmaped.setAttribute( "config:name","Tables" );

  foreach(Sheet* sheet, ksdoc->map()->sheetList())
  {
      QPoint marker;
      if ( view )
      {
          marker = view->markerFromSheet( sheet );
      }
      QDomElement tmpItemMapNamed = doc.createElement( "config:config-item-map-entry" );
      tmpItemMapNamed.setAttribute( "config:name", sheet->sheetName() );

      QDomElement sheetAttribute = doc.createElement( "config:config-item" );
      sheetAttribute.setAttribute( "config:name", "CursorPositionX" );
      sheetAttribute.setAttribute( "config:type", "int" );
      sheetAttribute.appendChild( doc.createTextNode( QString::number(marker.x() )  ) );
      tmpItemMapNamed.appendChild( sheetAttribute );

      sheetAttribute = doc.createElement( "config:config-item" );
      sheetAttribute.setAttribute( "config:name", "CursorPositionY" );
      sheetAttribute.setAttribute( "config:type", "int" );
      sheetAttribute.appendChild( doc.createTextNode( QString::number(marker.y() )  ) );
      tmpItemMapNamed.appendChild( sheetAttribute );

      configmaped.appendChild( tmpItemMapNamed );
  }
  mapItem.appendChild( configmaped );



  mapIndexed.appendChild( mapItem );

  begin.appendChild( configItem );

  settings.appendChild( begin );

  doc.appendChild( settings );

  QByteArray f( doc.toByteArray() );
  kDebug(30518) << "Settings: " << (char const * ) f << endl;

  store->write( f, f.length() );

  if ( !store->close() )
    return false;

  return true;
}

bool OpenCalcExport::exportContent( KoStore * store, const Doc * ksdoc )
{
  if ( !store->open( "content.xml" ) )
    return false;

  createDefaultStyles();

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

  QByteArray f( doc.toByteArray() );
  kDebug(30518) << "Content: " << (char const * ) f << endl;

  store->write( f, f.length() );

  if ( !store->close() )
    return false;

  return true;
}

void exportNamedExpr( QDomDocument & doc, QDomElement & parent,
                      AreaList const & namedAreas )
{
  AreaList::const_iterator it  = namedAreas.begin();
  AreaList::const_iterator end = namedAreas.end();

  while ( it != end )
  {
    QDomElement namedRange = doc.createElement( "table:named-range" );

    Reference ref = *it;

    namedRange.setAttribute( "table:name", ref.ref_name );
    namedRange.setAttribute( "table:base-cell-address", convertRefToBase( ref.sheet_name, ref.rect ) );
    namedRange.setAttribute( "table:cell-range-address", convertRefToRange( ref.sheet_name, ref.rect ) );

    parent.appendChild( namedRange );

    ++it;
  }
}

bool OpenCalcExport::exportBody( QDomDocument & doc, QDomElement & content, const Doc * ksdoc )
{
  QDomElement fontDecls  = doc.createElement( "office:font-decls" );
  QDomElement autoStyles = doc.createElement( "office:automatic-styles" );
  QDomElement body       = doc.createElement( "office:body" );

  if ( ksdoc->map()->isProtected() )
  {
    body.setAttribute( "table:structure-protected", "true" );

    Q3CString passwd;
    ksdoc->map()->password( passwd );
    if ( passwd.length() > 0 )
    {
      Q3CString str( KCodecs::base64Encode( passwd ) );
      body.setAttribute( "table:protection-key", QString( str.data() ) );
    }
  }

  foreach(Sheet* sheet, ksdoc->map()->sheetList())
  {
    SheetStyle ts;
    int maxCols         = 1;
    int maxRows         = 1;

    ts.visible = !sheet->isHidden();

    QDomElement tabElem = doc.createElement( "table:table" );
    tabElem.setAttribute( "table:style-name", m_styles.sheetStyle( ts ) );

    if ( sheet->isProtected() )
    {
      tabElem.setAttribute( "table:protected", "true" );

      Q3CString passwd;
      sheet->password( passwd );
      if ( passwd.length() > 0 )
      {
        Q3CString str( KCodecs::base64Encode( passwd ) );
        tabElem.setAttribute( "table:protection-key", QString( str.data() ) );
      }
    }

    QString name( sheet->sheetName() );

    int n = name.indexOf( ' ' );
    if ( n != -1 )
    {
      kDebug(30518) << "Sheet name converting: " << name << endl;
      name[n] == '_';
      kDebug(30518) << "Sheet name converted: " << name << endl;
    }
    name = name.replace( ' ', "_" );

    QRect _printRange = sheet->print()->printRange();
    if ( _printRange != ( QRect( QPoint( 1, 1 ), QPoint( KS_colMax, KS_rowMax ) ) ) )
    {
        QString range= convertRangeToRef( name, _printRange );
        //kDebug(30518)<<" range : "<<range<<endl;
        tabElem.setAttribute( "table:print-ranges", range );
    }


    tabElem.setAttribute( "table:name", name );

    maxRowCols( sheet, maxCols, maxRows );

    exportSheet( doc, tabElem, sheet, maxCols, maxRows );

    body.appendChild( tabElem );
  }

  KoDocument * document   = m_chain->inputDocument();
  Doc * kspreadDoc = static_cast<Doc *>( document );

  AreaList namedAreas = kspreadDoc->listArea();
  if ( namedAreas.count() > 0 )
  {
    QDomElement namedExpr = doc.createElement( "table:named-expressions" );
    exportNamedExpr( doc, namedExpr, namedAreas );

    body.appendChild( namedExpr );
  }

  m_styles.writeStyles( doc, autoStyles );
  m_styles.writeFontDecl( doc, fontDecls );

  content.appendChild( fontDecls );
  content.appendChild( autoStyles );
  content.appendChild( body );

  return true;
}

void OpenCalcExport::exportSheet( QDomDocument & doc, QDomElement & tabElem,
                                  const Sheet * sheet, int maxCols, int maxRows )
{
  kDebug(30518) << "exportSheet: " << sheet->sheetName() << endl;
  int i = 1;

  while ( i <= maxCols )
  {
    const ColumnFormat * column = sheet->columnFormat( i );
    ColumnStyle cs;
    cs.breakB = ::Style::automatic;
    cs.size   = column->mmWidth() / 10;
    bool hide = column->isHide();

    int j        = i + 1;
    int repeated = 1;
    while ( j <= maxCols )
    {
      const ColumnFormat *c = sheet->columnFormat( j );
      ColumnStyle cs1;
      cs1.breakB = ::Style::automatic;
      cs1.size   = c->mmWidth() / 10;

      if ( ColumnStyle::isEqual( &cs, cs1 ) && ( hide == c->isHide() ) )
        ++repeated;
      else
        break;
      ++j;
    }

    QDomElement colElem = doc.createElement( "table:table-column" );
    colElem.setAttribute( "table:style-name", m_styles.columnStyle( cs ) );
    colElem.setAttribute( "table:default-cell-style-name", "Default" );//todo fixme create style from cell
    if ( hide )
      colElem.setAttribute( "table:visibility", "collapse" );

    if ( repeated > 1 )
      colElem.setAttribute( "table:number-columns-repeated", QString::number( repeated ) );

    tabElem.appendChild( colElem );
    i += repeated;
  }

  for ( i = 1; i <= maxRows; ++i )
  {
    const RowFormat * row = sheet->rowFormat( i );
    RowStyle rs;
    rs.breakB = ::Style::automatic;
    rs.size   = row->mmHeight() / 10;

    QDomElement rowElem = doc.createElement( "table:table-row" );
    rowElem.setAttribute( "table:style-name", m_styles.rowStyle( rs ) );
    if ( row->isHide() )
      rowElem.setAttribute( "table:visibility", "collapse" );

    exportCells( doc, rowElem, sheet, i, maxCols );

    tabElem.appendChild( rowElem );
  }
}

void OpenCalcExport::exportCells( QDomDocument & doc, QDomElement & rowElem,
                                  const Sheet *sheet, int row, int maxCols )
{
  int i = 1;
  while ( i <= maxCols )
  {
    int  repeated = 1;
    bool hasComment = false;
    const Cell* cell = sheet->cellAt( i, row );
    QDomElement cellElem;

    if ( !cell->isPartOfMerged() )
      cellElem = doc.createElement( "table:table-cell" );
    else
      cellElem = doc.createElement( "table:covered-table-cell" );

    QFont font;
    Value const value( cell->value() );
    if ( !cell->isDefault() )
    {
      font = cell->format()->textFont( i, row );
      m_styles.addFont( font );

      if ( cell->format()->hasProperty( KSpread::Style::SComment ) )
        hasComment = true;
    }

    CellStyle c;
    CellStyle::loadData( c, cell ); // TODO: number style

    cellElem.setAttribute( "table:style-name", m_styles.cellStyle( c ) );

    // group empty cells with the same style
    if ( cell->isEmpty() && !hasComment && !cell->isPartOfMerged() && !cell->doesMergeCells() )
    {
      int j = i + 1;
      while ( j <= maxCols )
      {
        const Cell *cell1 = sheet->cellAt( j, row );

        CellStyle c1;
        CellStyle::loadData( c1, cell1 ); // TODO: number style

        if ( cell1->isEmpty() && !cell->format()->hasProperty( KSpread::Style::SComment )
             && CellStyle::isEqual( &c, c1 ) && !cell->isPartOfMerged() && !cell->doesMergeCells() )
          ++repeated;
        else
          break;
        ++j;
      }
      if ( repeated > 1 )
        cellElem.setAttribute( "table:number-columns-repeated", QString::number( repeated ) );
    }

    if ( value.isBoolean() )
    {
      kDebug(30518) << "Type: Boolean" << endl;
      cellElem.setAttribute( "table:value-type", "boolean" );
      cellElem.setAttribute( "table:boolean-value", ( value.asBoolean() ? "true" : "false" ) );
    }
    else if ( value.isNumber() )
    {
      kDebug(30518) << "Type: Number" << endl;
      FormatType type = cell->format()->getFormatType( i, row );

      if ( type == Percentage_format )
        cellElem.setAttribute( "table:value-type", "percentage" );
      else
        cellElem.setAttribute( "table:value-type", "float" );

      cellElem.setAttribute( "table:value", QString::number( value.asFloat() ) );
    }
    else
    {
      kDebug(30518) << "Type: " << value.type() << endl;
    }

    if ( cell->isFormula() )
    {
      kDebug(30518) << "Formula found" << endl;

      QString formula( convertFormula( cell->text() ) );
      cellElem.setAttribute( "table:formula", formula );
    }
    else if ( !cell->link().isEmpty() )
    {
      QDomElement link = doc.createElement( "text:p" );
      QDomElement linkref = doc.createElement( "text:a" );

      QString tmp = cell->link();
       if ( localReferenceAnchor( tmp ) )
           linkref.setAttribute( "xlink:href", ( "#"+tmp ) );
       else
           linkref.setAttribute( "xlink:href", tmp  );

       linkref.appendChild( doc.createTextNode( cell->text() ) );

       link.appendChild( linkref );
       cellElem.appendChild( link );
    }
    else if ( !cell->isEmpty() )
    {
      QDomElement textElem = doc.createElement( "text:p" );
      textElem.appendChild( doc.createTextNode( cell->strOutText() ) );

      cellElem.appendChild( textElem );
      kDebug(30518) << "Cell StrOut: " << cell->strOutText() << endl;
    }

    if ( cell->doesMergeCells() )
    {
      int colSpan = cell->mergedXCells() + 1;
      int rowSpan = cell->mergedYCells() + 1;

      if ( colSpan > 1 )
        cellElem.setAttribute( "table:number-columns-spanned", QString::number( colSpan ) );

      if ( rowSpan > 1 )
        cellElem.setAttribute( "table:number-rows-spanned", QString::number( rowSpan ) );
    }

    if ( hasComment )
    {
      QString comment( cell->format()->comment( i, row ) );
      QDomElement annotation = doc.createElement( "office:annotation" );
      QDomElement text = doc.createElement( "text:p" );
      text.appendChild( doc.createTextNode( comment ) );

      annotation.appendChild( text );
      cellElem.appendChild( annotation );
    }

    rowElem.appendChild( cellElem );

    i += repeated;
  }
}

void OpenCalcExport::maxRowCols( const Sheet *sheet,
                                 int & maxCols, int & maxRows )
{
  Cell const * cell = sheet->firstCell();

  while ( cell )
  {
    if ( cell->column() > maxCols )
      maxCols = cell->column();

    if ( cell->row() > maxRows )
      maxRows = cell->row();

    cell = cell->nextCell();
  }

  RowFormat const * row = sheet->firstRow();

  while ( row )
  {
    if ( row->row() > maxRows )
      maxRows = row->row();

    row = row->next();
  }

  ColumnFormat const * col = sheet->firstCol();
  while ( col )
  {
    if ( col->column() > maxCols )
      maxCols = col->column();

    col = col->next();
  }

}

bool OpenCalcExport::exportStyles( KoStore * store, const Doc *ksdoc )
{
  if ( !store->open( "styles.xml" ) )
    return false;

  QDomDocument doc;
  doc.appendChild( doc.createProcessingInstruction( "xml","version=\"1.0\" encoding=\"UTF-8\"" ) );

  QDomElement content = doc.createElement( "office:document-styles" );
  content.setAttribute( "xmlns:office", "http://openoffice.org/2000/office" );
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
  content.setAttribute( "office:version", "1.0" );

  // order important here!
  QDomElement officeStyles = doc.createElement( "office:styles" );
  exportDefaultCellStyle( doc, officeStyles );

  QDomElement fontDecls = doc.createElement( "office:font-decls" );
  m_styles.writeFontDecl( doc, fontDecls );

  // TODO: needs in new number/date/time parser...
  //  exportDefaultNumberStyles( doc, officeStyles );

  QDomElement defaultStyle = doc.createElement( "style:style" );
  defaultStyle.setAttribute( "style:name", "Default" );
  defaultStyle.setAttribute( "style:family", "table-cell" );
  officeStyles.appendChild( defaultStyle );

  QDomElement autoStyles = doc.createElement( "office:automatic-styles" );
  exportPageAutoStyles( doc, autoStyles, ksdoc );

  QDomElement masterStyles = doc.createElement( "office:master-styles" );
  exportMasterStyles( doc, masterStyles, ksdoc );

  content.appendChild( fontDecls );
  content.appendChild( officeStyles );
  content.appendChild( autoStyles );
  content.appendChild( masterStyles );

  doc.appendChild( content );

  QByteArray f( doc.toByteArray() );
  kDebug(30518) << "Content: " << (char const * ) f << endl;

  store->write( f, f.length() );

  if ( !store->close() )
    return false;

  return true;
}

void OpenCalcExport::exportDefaultCellStyle( QDomDocument & doc, QDomElement & officeStyles )
{
  QDomElement defStyle = doc.createElement( "style:default-style" );
  defStyle.setAttribute( "style:family", "table-cell" );

  KoDocument * document = m_chain->inputDocument();
  Doc * ksdoc    = static_cast<Doc *>(document);

  Format * format = new Format( 0, ksdoc->styleManager()->defaultStyle() );
  const KLocale *locale = ksdoc->locale();
  QString language;
  QString country;
  QString charSet;

  QString l( locale->language() );
  KLocale::splitLocale( l, language, country, charSet );
  QFont font( format->font() );
  m_styles.addFont( font, true );

  QDomElement style = doc.createElement( "style:properties" );
  style.setAttribute( "style:font-name", font.family() );
  style.setAttribute( "fo:font-size", QString( "%1pt" ).arg( font.pointSize() ) );
  style.setAttribute( "style:decimal-places", QString::number( locale->fracDigits() ) );
  style.setAttribute( "fo:language", language );
  style.setAttribute( "fo:country", country );
  style.setAttribute( "style:font-name-asian", "HG Mincho Light J" );
  style.setAttribute( "style:language-asian", "none" );
  style.setAttribute( "style:country-asian", "none" );
  style.setAttribute( "style:font-name-complex", "Arial Unicode MS" );
  style.setAttribute( "style:language-complex", "none" );
  style.setAttribute( "style:country-complex", "none" );
  style.setAttribute( "style:tab-stop-distance", "1.25cm" );

  defStyle.appendChild( style );
  officeStyles.appendChild( defStyle );
  delete format;
}

void OpenCalcExport::createDefaultStyles()
{
  // TODO: default number styles, currency styles,...
}

void OpenCalcExport::exportPageAutoStyles( QDomDocument & doc, QDomElement & autoStyles,
                                           const Doc *ksdoc )
{
  const Sheet * sheet = ksdoc->map()->sheetList().first();

  float width  = 20.999;
  float height = 29.699;

  if ( sheet )
  {
    width  = sheet->print()->paperWidth() / 10;
    height = sheet->print()->paperHeight() / 10;
  }

  QString sWidth  = QString( "%1cm" ).arg( width  );
  QString sHeight = QString( "%1cm" ).arg( height );

  QDomElement pageMaster = doc.createElement( "style:page-master" );
  pageMaster.setAttribute( "style:name", "pm1" );

  QDomElement properties = doc.createElement( "style:properties" );
  properties.setAttribute( "fo:page-width",  sWidth  );
  properties.setAttribute( "fo:page-height", sHeight );
  properties.setAttribute( "fo:border", "0.002cm solid #000000" );
  properties.setAttribute( "fo:padding", "0cm" );
  properties.setAttribute( "fo:background-color", "transparent" );

  pageMaster.appendChild( properties );

  QDomElement header = doc.createElement( "style:header-style" );
  properties = doc.createElement( "style:properties" );
  properties.setAttribute( "fo:min-height", "0.75cm" );
  properties.setAttribute( "fo:margin-left", "0cm" );
  properties.setAttribute( "fo:margin-right", "0cm" );
  properties.setAttribute( "fo:margin-bottom", "0.25cm" );

  header.appendChild( properties );

  QDomElement footer = doc.createElement( "style:header-style" );
  properties = doc.createElement( "style:properties" );
  properties.setAttribute( "fo:min-height", "0.75cm" );
  properties.setAttribute( "fo:margin-left", "0cm" );
  properties.setAttribute( "fo:margin-right", "0cm" );
  properties.setAttribute( "fo:margin-bottom", "0.25cm" );

  footer.appendChild( properties );

  pageMaster.appendChild( header );
  pageMaster.appendChild( footer );

  autoStyles.appendChild( pageMaster );
}

void OpenCalcExport::exportMasterStyles( QDomDocument & doc, QDomElement & masterStyles,
                                         const Doc * ksdoc )
{
  QDomElement masterPage = doc.createElement( "style:master-page" );
  masterPage.setAttribute( "style:name", "Default" );
  masterPage.setAttribute( "style:page-master-name", "pm1" );

  const Sheet * sheet = ksdoc->map()->sheetList().first();

  QString headerLeft;
  QString headerCenter;
  QString headerRight;
  QString footerLeft;
  QString footerCenter;
  QString footerRight;

  if ( sheet )
  {
    headerLeft   = sheet->print()->headLeft();
    headerCenter = sheet->print()->headMid();
    headerRight  = sheet->print()->headRight();
    footerLeft   = sheet->print()->footLeft();
    footerCenter = sheet->print()->footMid();
    footerRight  = sheet->print()->footRight();
  }

  if ( ( headerLeft.length() > 0 ) || ( headerCenter.length() > 0 )
       || ( headerRight.length() > 0 ) )
  {
    QDomElement header = doc.createElement( "style:header" );
    QDomElement left   = doc.createElement( "style:region-left" );
    QDomElement text   = doc.createElement( "text:p" );
    convertPart( headerLeft, doc, text, ksdoc );
    left.appendChild( text );

    QDomElement center = doc.createElement( "style:region-center" );
    QDomElement text1  = doc.createElement( "text:p" );
    convertPart( headerCenter, doc, text1, ksdoc );
    center.appendChild( text1 );

    QDomElement right = doc.createElement( "style:region-right" );
    QDomElement text2 = doc.createElement( "text:p" );
    convertPart( headerRight, doc, text2, ksdoc );
    right.appendChild( text2 );

    header.appendChild( left   );
    header.appendChild( center );
    header.appendChild( right  );

    masterPage.appendChild( header );
  }
  else
  {
    QDomElement header = doc.createElement( "style:header" );
    QDomElement text   = doc.createElement( "text:p" );
    QDomElement name   = doc.createElement( "text:sheet-name" );
    name.appendChild( doc.createTextNode( "???" ) );
    text.appendChild( name );
    header.appendChild( text );

    masterPage.appendChild( header );
  }

  if ( ( footerLeft.length() > 0 ) || ( footerCenter.length() > 0 )
       || ( footerRight.length() > 0 ) )
  {
    QDomElement footer = doc.createElement( "style:footer" );
    QDomElement left   = doc.createElement( "style:region-left" );
    QDomElement text   = doc.createElement( "text:p" );
    convertPart( footerLeft, doc, text, ksdoc );
    left.appendChild( text );

    QDomElement center = doc.createElement( "style:region-center" );
    QDomElement text1  = doc.createElement( "text:p" );
    convertPart( footerCenter, doc, text1, ksdoc );
    center.appendChild( text1 );

    QDomElement right = doc.createElement( "style:region-right" );
    QDomElement text2  = doc.createElement( "text:p" );
    convertPart( footerRight, doc, text2, ksdoc );
    right.appendChild( text2 );

    footer.appendChild( left   );
    footer.appendChild( center );
    footer.appendChild( right  );

    masterPage.appendChild( footer );
  }
  else
  {
    QDomElement footer = doc.createElement( "style:footer" );
    QDomElement text   = doc.createElement( "text:p" );
    text.appendChild( doc.createTextNode( i18n( "Page " ) ) );
    QDomElement number = doc.createElement( "text:page-number" );
    number.appendChild( doc.createTextNode( "1" ) );
    text.appendChild( number );
    footer.appendChild( text );

    masterPage.appendChild( footer );
  }

  masterStyles.appendChild( masterPage );
}

void OpenCalcExport::addText( QString const & text, QDomDocument & doc,
                              QDomElement & parent )
{
  if (text.length() > 0 )
    parent.appendChild( doc.createTextNode( text ) );
}

void OpenCalcExport::convertPart( QString const & part, QDomDocument & doc,
                                  QDomElement & parent, const Doc * ksdoc )
{
  QString text;
  QString var;

  bool inVar = false;
  uint i = 0;
  uint l = part.length();
  while ( i < l )
  {
    if ( inVar || part[i] == '<' )
    {
      inVar = true;
      var += part[i];
      if ( part[i] == '>' )
      {
        inVar = false;
        if ( var == "<page>" )
        {
          addText( text, doc, parent );

          QDomElement page = doc.createElement( "text:page-number" );
          page.appendChild( doc.createTextNode( "1" ) );
          parent.appendChild( page );
        }
        else if ( var == "<pages>" )
        {
          addText( text, doc, parent );

          QDomElement page = doc.createElement( "text:page-count" );
          page.appendChild( doc.createTextNode( "99" ) );
          parent.appendChild( page );
        }
        else if ( var == "<date>" )
        {
          addText( text, doc, parent );

          QDomElement t = doc.createElement( "text:date" );
          t.setAttribute( "text:date-value", "0-00-00" );
          // todo: "style:data-style-name", "N2"
          t.appendChild( doc.createTextNode( QDate::currentDate().toString() ) );
          parent.appendChild( t );
        }
        else if ( var == "<time>" )
        {
          addText( text, doc, parent );

          QDomElement t = doc.createElement( "text:time" );
          t.appendChild( doc.createTextNode( QTime::currentTime().toString() ) );
          parent.appendChild( t );
        }
        else if ( var == "<file>" ) // filepath + name
        {
          addText( text, doc, parent );

          QDomElement t = doc.createElement( "text:file-name" );
          t.setAttribute( "text:display", "full" );
          t.appendChild( doc.createTextNode( "???" ) );
          parent.appendChild( t );
        }
        else if ( var == "<name>" ) // filename
        {
          addText( text, doc, parent );

          QDomElement t = doc.createElement( "text:title" );
          t.appendChild( doc.createTextNode( "???" ) );
          parent.appendChild( t );
        }
        else if ( var == "<author>" )
        {
          KoDocumentInfo       * docInfo    = ksdoc->documentInfo();

          text += docInfo->authorInfo("creator");

          addText( text, doc, parent );
        }
        else if ( var == "<email>" )
        {
          KoDocumentInfo       * docInfo    = ksdoc->documentInfo();

          text += docInfo->authorInfo("email");

          addText( text, doc, parent );
        }
        else if ( var == "<org>" )
        {
          KoDocumentInfo       * docInfo    = ksdoc->documentInfo();

          text += docInfo->authorInfo("company");

          addText( text, doc, parent );
        }
        else if ( var == "<sheet>" )
        {
          addText( text, doc, parent );

          QDomElement s = doc.createElement( "text:sheet-name" );
          s.appendChild( doc.createTextNode( "???" ) );
          parent.appendChild( s );
        }
        else
        {
          // no known variable:
          text += var;
          addText( text, doc, parent );
        }

        text = "";
        var  = "";
      }
    }
    else
    {
      text += part[i];
    }
    ++i;
  }
  if ( !text.isEmpty() || !var.isEmpty() )
  {
      //we don't have var at the end =>store it
      addText( text+var, doc, parent );
  }
}

QString OpenCalcExport::convertFormula( QString const & formula ) const
{
  // TODO Stefan: Check if Oasis::encodeFormula could be used instead
  QChar decimalSymbol( '.' );
  if ( m_locale )
  {
    const QString decimal ( m_locale->decimalSymbol() );
    if ( !decimal.isEmpty() )
    {
        decimalSymbol = decimal.at( 0 );
    }
  }

  QString s;
  QRegExp exp("(\\$?)([a-zA-Z]+)(\\$?)([0-9]+)");
  int n = exp.search( formula, 0 );
  kDebug(30518) << "Exp: " << formula << ", n: " << n << ", Length: " << formula.length()
            << ", Matched length: " << exp.matchedLength() << endl;

  bool inQuote1 = false;
  bool inQuote2 = false;
  int i = 0;
  int l = (int) formula.length();
  if ( l <= 0 )
    return formula;
  while ( i < l )
  {
    if ( ( n != -1 ) && ( n < i ) )
    {
      n = exp.search( formula, i );
      kDebug(30518) << "Exp: " << formula.right( l - i ) << ", n: " << n << endl;
    }
    if ( formula[i] == '"' )
    {
      inQuote1 = !inQuote1;
      s += formula[i];
      ++i;
      continue;
    }
    if ( formula[i] == '\'' )
    {
      // named area
      inQuote2 = !inQuote2;
      ++i;
      continue;
    }
    if ( inQuote1 || inQuote2 )
    {
      s += formula[i];
      ++i;
      continue;
    }
    if ( ( formula[i] == '=' ) && ( formula[i + 1] == '=' ) )
    {
      s += '=';
      ++i;++i;
      continue;
    }
    if ( formula[i] == '!' )
    {
      QChar c;
      int j = (int) s.length() - 1;

      while ( j >= 0 )
      {
        c = s[j];
        if ( c == ' ' )
          s[j] = '_';
        if ( !(c.isLetterOrNumber() || c == ' ' || c == '.'
               || c == '_') )
        {
          s.insert( j + 1, '[' );
          break;
        }
        --j;
      }
      s += '.';
      ++i;
      continue;
    }
    else if ( formula[i] == decimalSymbol )
    {
      s += '.'; // decimal point
      ++i;
      continue;
    }
    if ( n == i )
    {
      int ml = exp.matchedLength();
      if ( formula[ i + ml ] == '!' )
      {
        kDebug(30518) << "No cell ref but sheet name" << endl;
        s += formula[i];
        ++i;
        continue;
      }
      if ( ( i > 0 ) && ( formula[i - 1] != '!' ) )
        s += "[.";
      for ( int j = 0; j < ml; ++j )
      {
        s += formula[i];
        ++i;
      }
      s += ']';
      continue;
    }

    s += formula[i];
    ++i;
  }

  return s;
}

bool OpenCalcExport::writeMetaFile( KoStore * store, uint filesWritten )
{
    store->enterDirectory( "META-INF" );
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

  QByteArray doc( meta.toByteArray() );
  kDebug(30518) << "Manifest: " << doc << endl;

  store->write( doc, doc.length() );

  if ( !store->close() )
    return false;

  return true;
}

#include <opencalcexport.moc>
