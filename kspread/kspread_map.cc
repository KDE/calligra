/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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

#include <stdlib.h>
#include <time.h>

#include <QFile>

#include <kcodecs.h>
#include <ktempfile.h>

#include <KoDom.h>
#include <KoGenStyles.h>
#include <KoOasisSettings.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>

#include "kspread_canvas.h"
#include "kspread_doc.h"
#include "kspread_genvalidationstyle.h"
#include "kspread_locale.h"
#include "kspread_sheet.h"
#include "kspread_style_manager.h"
#include "kspread_view.h"
#include "KSpreadMapIface.h"

#include "kspread_map.h"

using namespace KSpread;

bool Map::respectCase = true;

Map::Map ( Doc* doc, const char* name)
  : QObject( doc ),
    m_doc( doc ),
    m_initialActiveSheet( 0 ),
    m_initialMarkerColumn( 0 ),
    m_initialMarkerRow( 0 ),
    m_initialXOffset(0.0),
    m_initialYOffset(0.0),
    tableId (1),
    m_dcop( 0 )
{
  setObjectName( name ); // ### necessary for DCOP/Scripting?
}

Map::~Map()
{
  qDeleteAll( m_lstSheets );
  delete m_dcop;
}

Doc* Map::doc() const
{
  return m_doc;
}

void Map::setProtected( QByteArray const & passwd )
{
  m_strPassword = passwd;
}

Sheet* Map::createSheet()
{
  QString s( i18n("Sheet%1", tableId++ ) );
  Sheet *t = new Sheet ( this, s , s.toUtf8());
  t->setSheetName( s, true ); // huh? (Werner)
  return t;
}

void Map::addSheet( Sheet *_sheet )
{
  m_lstSheets.append( _sheet );

  m_doc->setModified( true );

  emit sig_addSheet( _sheet );
}

Sheet *Map::addNewSheet ()
{
  Sheet *t = createSheet ();
  addSheet (t);
  return t;
}

void Map::moveSheet( const QString & _from, const QString & _to, bool _before )
{
  Sheet* sheetfrom = findSheet( _from );
  Sheet* sheetto = findSheet( _to );

  int from = m_lstSheets.indexOf( sheetfrom ) ;
  int to = m_lstSheets.indexOf( sheetto ) ;
  if ( !_before )
  ++to;

  if ( to > (int)m_lstSheets.count() )
  {
    m_lstSheets.append( sheetfrom );
    m_lstSheets.removeAt( from );
  }
  else if ( from < to )
  {
    m_lstSheets.insert( to, sheetfrom );
    m_lstSheets.removeAt( from );
  }
  else
  {
    m_lstSheets.removeAt( from );
    m_lstSheets.insert( to, sheetfrom );
  }
}

void Map::loadOasisSettings( KoOasisSettings &settings )
{
    KoOasisSettings::Items viewSettings = settings.itemSet( "view-settings" );
    KoOasisSettings::IndexedMap viewMap = viewSettings.indexedMap( "Views" );
    KoOasisSettings::Items firstView = viewMap.entry( 0 );

    KoOasisSettings::NamedMap sheetsMap = firstView.namedMap( "Tables" );
    kDebug()<<" loadOasisSettings( KoOasisSettings &settings ) exist : "<< !sheetsMap.isNull() <<endl;
    if ( !sheetsMap.isNull() )
    {
      foreach ( Sheet* sheet, m_lstSheets )
      {
        sheet->loadOasisSettings( sheetsMap );
      }
    }

    QString activeSheet = firstView.parseConfigItemString( "ActiveTable" );
    kDebug()<<" loadOasisSettings( KoOasisSettings &settings ) activeSheet :"<<activeSheet<<endl;

    if (!activeSheet.isEmpty())
    {
        // Used by View's constructor
        m_initialActiveSheet = findSheet( activeSheet );
    }

}

void Map::saveOasisSettings( KoXmlWriter &settingsWriter )
{
    settingsWriter.addConfigItem( "ViewId", QString::fromLatin1( "View1" ) );
    // Save visual info for the first view, such as active sheet and active cell
    // It looks like a hack, but reopening a document creates only one view anyway (David)
    View * view = static_cast<View*>( m_doc->views().first());
    if ( view ) // no view if embedded document
    {
        // save current sheet selection before to save marker, otherwise current pos is not saved
        view->saveCurrentSheetSelection();
        //<config:config-item config:name="ActiveTable" config:type="string">Feuille1</config:config-item>
        settingsWriter.addConfigItem( "ActiveTable",  view->activeSheet()->sheetName() );
    }

    //<config:config-item-map-named config:name="Tables">
    settingsWriter.startElement("config:config-item-map-named" );
    settingsWriter.addAttribute("config:name","Tables" );
    foreach ( Sheet* sheet, m_lstSheets )
    {
      settingsWriter.startElement( "config:config-item-map-entry" );
      settingsWriter.addAttribute( "config:name", sheet->sheetName() );
      if ( view )
      {
        QPoint marker = view->markerFromSheet( sheet );
        KoPoint offset = view->offsetFromSheet( sheet );
        settingsWriter.addConfigItem( "CursorPositionX", marker.x() );
        settingsWriter.addConfigItem( "CursorPositionY", marker.y() );
        settingsWriter.addConfigItem( "xOffset", offset.x() );
        settingsWriter.addConfigItem( "yOffset", offset.y() );
      }
      sheet->saveOasisSettings( settingsWriter );
      settingsWriter.endElement();
    }
    settingsWriter.endElement();
}


bool Map::saveOasis( KoXmlWriter & xmlWriter, KoGenStyles & mainStyles, KoStore *store, KoXmlWriter* manifestWriter, int &_indexObj, int &_partIndexObj )
{
    if ( !m_strPassword.isEmpty() )
    {
        xmlWriter.addAttribute("table:structure-protected", "true" );
        QByteArray str = KCodecs::base64Encode( m_strPassword );
        // FIXME Stefan: see OpenDocument spec, ch. 17.3 Encryption
        xmlWriter.addAttribute("table:protection-key", QString( str.data() ) );
    }

    GenValidationStyles valStyle;

    KTempFile bodyTmpFile;
    //Check that creation of temp file was successful
    if (bodyTmpFile.status() != 0)
    {
	    qWarning("Creation of temporary file to store document body failed.");
	    return false;
    }

    bodyTmpFile.setAutoDelete( true );
    QFile* tmpFile = bodyTmpFile.file();
    KoXmlWriter bodyTmpWriter( tmpFile );


    foreach ( Sheet* sheet, m_lstSheets )
    {
        sheet->saveOasis( bodyTmpWriter, mainStyles, valStyle, store,
                          manifestWriter, _indexObj, _partIndexObj );
    }

    valStyle.writeStyle( xmlWriter );


    tmpFile->close();
    xmlWriter.addCompleteElement( tmpFile );
    bodyTmpFile.close();

    return true;
}

QDomElement Map::save( QDomDocument& doc )
{
    QDomElement mymap = doc.createElement( "map" );
  // Save visual info for the first view, such as active sheet and active cell
  // It looks like a hack, but reopening a document creates only one view anyway (David)
  View * view = static_cast<View*>(m_doc->views().first());
  if ( view ) // no view if embedded document
  {
    Canvas * canvas = view->canvasWidget();
    mymap.setAttribute( "activeTable",  canvas->activeSheet()->sheetName() );
    mymap.setAttribute( "markerColumn", canvas->markerColumn() );
    mymap.setAttribute( "markerRow",    canvas->markerRow() );
    mymap.setAttribute( "xOffset",      canvas->xOffset() );
    mymap.setAttribute( "yOffset",      canvas->yOffset() );
  }

  if ( !m_strPassword.isNull() )
  {
    if ( m_strPassword.size() > 0 )
    {
      QByteArray str = KCodecs::base64Encode( m_strPassword );
      mymap.setAttribute( "protected", QString( str.data() ) );
    }
    else
      mymap.setAttribute( "protected", "" );
  }

  foreach ( Sheet* sheet, m_lstSheets )
  {
    QDomElement e = sheet->saveXML( doc );
    if ( e.isNull() )
      return e;
    mymap.appendChild( e );
  }
  return mymap;
}

bool Map::loadOasis( const QDomElement& body, KoOasisLoadingContext& oasisContext )
{
    if ( body.hasAttributeNS( KoXmlNS::table, "structure-protected" ) )
    {
        QByteArray passwd( "" );
        if ( body.hasAttributeNS( KoXmlNS::table, "protection-key" ) )
        {
            QString p = body.attributeNS( KoXmlNS::table, "protection-key", QString::null );
            QByteArray str( p.toLatin1() );
            passwd = KCodecs::base64Decode( str );
        }
        m_strPassword = passwd;
    }

    QDomNode sheetNode = KoDom::namedItemNS( body, KoXmlNS::table, "table" );
    // sanity check
    if ( sheetNode.isNull() ) return false;

    while ( !sheetNode.isNull() )
    {
        QDomElement sheetElement = sheetNode.toElement();
        if( !sheetElement.isNull() )
        {
            //kDebug()<<"  Map::loadOasis tableElement is not null \n";
            //kDebug()<<"tableElement.nodeName() :"<<sheetElement.nodeName()<<endl;
            if( sheetElement.nodeName() == "table:table" )
            {
                if( !sheetElement.attributeNS( KoXmlNS::table, "name", QString::null ).isEmpty() )
                {
                    Sheet* sheet = addNewSheet();
                    sheet->setSheetName( sheetElement.attributeNS( KoXmlNS::table, "name", QString::null ), true, false );
                }
            }
        }
        sheetNode = sheetNode.nextSibling();
    }

    //pre-load auto styles
    Styles autoStyles = StyleManager::loadOasisAutoStyles( oasisContext.oasisStyles() );

    // load the sheet
    sheetNode = body.firstChild();
    while ( !sheetNode.isNull() )
    {
        QDomElement sheetElement = sheetNode.toElement();
        if( !sheetElement.isNull() )
        {
            kDebug()<<"tableElement.nodeName() bis :"<<sheetElement.nodeName()<<endl;
            if( sheetElement.nodeName() == "table:table" )
            {
                if( !sheetElement.attributeNS( KoXmlNS::table, "name", QString::null ).isEmpty() )
                {
                    QString name = sheetElement.attributeNS( KoXmlNS::table, "name", QString::null );
                    Sheet* sheet = findSheet( name );
                    if( sheet )
                        sheet->loadOasis( sheetElement, oasisContext, autoStyles );
                }
            }
        }
        sheetNode = sheetNode.nextSibling();
    }

    //delete any styles which were not used
    StyleManager::releaseUnusedAutoStyles( autoStyles );

    return true;
}


bool Map::loadXML( const QDomElement& mymap )
{
  QString activeSheet   = mymap.attribute( "activeTable" );
  m_initialMarkerColumn = mymap.attribute( "markerColumn" ).toInt();
  m_initialMarkerRow    = mymap.attribute( "markerRow" ).toInt();
  m_initialXOffset      = mymap.attribute( "xOffset" ).toDouble();
  m_initialYOffset      = mymap.attribute( "yOffset" ).toDouble();

  QDomNode n = mymap.firstChild();
  if ( n.isNull() )
  {
      // We need at least one sheet !
      doc()->setErrorMessage( i18n("This document has no sheets (tables).") );
      return false;
  }
  while( !n.isNull() )
  {
    QDomElement e = n.toElement();
    if ( !e.isNull() && e.tagName() == "table" )
    {
      Sheet *t = addNewSheet();
      if ( !t->loadXML( e ) )
        return false;
    }
    n = n.nextSibling();
  }

  if ( mymap.hasAttribute( "protected" ) )
  {
    QString passwd = mymap.attribute( "protected" );

    if ( passwd.length() > 0 )
    {
      QByteArray str( passwd.toLatin1() );
      m_strPassword = KCodecs::base64Decode( str );
    }
    else
      m_strPassword = QByteArray( "" );
  }

  if (!activeSheet.isEmpty())
  {
    // Used by View's constructor
    m_initialActiveSheet = findSheet( activeSheet );
  }

  return true;
}

void Map::update()
{
  foreach ( Sheet* sheet, m_lstSheets )
  {
    sheet->recalc();
  }
}

Sheet* Map::findSheet( const QString & _name )
{
  foreach ( Sheet* sheet, m_lstSheets )
  {
    if ( _name.toLower() == sheet->sheetName().toLower() )
      return sheet;
  }
  return 0;
}

Sheet * Map::nextSheet( Sheet * currentSheet )
{
  if( currentSheet == m_lstSheets.last())
    return currentSheet;
  int index = 0;
  foreach ( Sheet* sheet, m_lstSheets )
  {
    if ( sheet == currentSheet )
      return m_lstSheets.value( ++index );
    ++index;
  }
  return 0;
}

Sheet * Map::previousSheet( Sheet * currentSheet )
{
  if( currentSheet == m_lstSheets.first())
    return currentSheet;
  int index = 0;
  foreach ( Sheet* sheet, m_lstSheets )
  {
    if ( sheet  == currentSheet )
      return m_lstSheets.value( --index );
    ++index;
  }
  return 0;
}

bool Map::saveChildren( KoStore * _store )
{
  foreach ( Sheet* sheet, m_lstSheets )
  {
    // set the child document's url to an internal url (ex: "tar:/0/1")
    if ( !sheet->saveChildren( _store, sheet->sheetName() ) )
      return false;
  }
  return true;
}

bool Map::loadChildren( KoStore * _store )
{
  foreach ( Sheet* sheet, m_lstSheets )
  {
    if ( !sheet->loadChildren( _store ) )
      return false;
  }
  return true;
}

DCOPObject * Map::dcopObject()
{
    if ( !m_dcop )
        m_dcop = new MapIface( this );

    return m_dcop;
}

void Map::takeSheet( Sheet * sheet )
{
  m_lstSheets.removeAll( sheet );
  m_lstDeletedSheets.append( sheet );
}

void Map::insertSheet( Sheet * sheet )
{
    m_lstDeletedSheets.removeAll( sheet );
    m_lstSheets.append(sheet);
}

// FIXME cache this for faster operation
QStringList Map::visibleSheets() const
{
  QStringList result;
  foreach ( Sheet* sheet, m_lstSheets )
  {
    if( !sheet->isHidden() )
      result.append( sheet->sheetName() );
  }
  return result;
}

// FIXME cache this for faster operation
QStringList Map::hiddenSheets() const
{
  QStringList result;
  foreach ( Sheet* sheet, m_lstSheets )
  {
    if( sheet->isHidden() )
      result.append( sheet->sheetName() );
  }
  return result;
}

#include "kspread_map.moc"

