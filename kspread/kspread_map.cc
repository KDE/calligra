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

#include <qfile.h>

#include <kmdcodec.h>
#include <ktempfile.h>

#include <KoDom.h>
#include <KoGenStyles.h>
#include <KoOasisSettings.h>
#include <KoOasisStyles.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>

#include "kspread_canvas.h"
#include "kspread_doc.h"
#include "kspread_genvalidationstyle.h"
#include "kspread_locale.h"
#include "kspread_sheet.h"
#include "kspread_style.h"
#include "kspread_style_manager.h"
#include "kspread_view.h"
#include "KSpreadMapIface.h"

#include "kspread_map.h"

using namespace KSpread;

bool Map::respectCase = true;

Map::Map ( Doc* doc, const char* name)
  : QObject( doc, name ),
    m_doc( doc ),
    m_initialActiveSheet( 0 ),
    m_initialMarkerColumn( 0 ),
    m_initialMarkerRow( 0 ),
    m_initialXOffset(0.0),
    m_initialYOffset(0.0),
    tableId (1),
    m_dcop( 0 )
{
  m_lstSheets.setAutoDelete( true );
}

Map::~Map()
{
    delete m_dcop;
}

Doc* Map::doc() const
{
  return m_doc;
}

void Map::setProtected( QCString const & passwd )
{
  m_strPassword = passwd;
}

Sheet* Map::createSheet()
{
  QString s( i18n("Sheet%1") );
  s = s.arg( tableId++ );
  Sheet *t = new Sheet ( this, s , s.utf8());
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

  int from = m_lstSheets.find( sheetfrom ) ;
  int to = m_lstSheets.find( sheetto ) ;
  if ( !_before )
  ++to;

  if ( to > (int)m_lstSheets.count() )
  {
    m_lstSheets.append( sheetfrom );
    m_lstSheets.take( from );
  }
  else if ( from < to )
  {
    m_lstSheets.insert( to, sheetfrom );
    m_lstSheets.take( from );
  }
  else
  {
    m_lstSheets.take( from );
    m_lstSheets.insert( to, sheetfrom );
  }
}

void Map::loadOasisSettings( KoOasisSettings &settings )
{
    KoOasisSettings::Items viewSettings = settings.itemSet( "view-settings" );
    KoOasisSettings::IndexedMap viewMap = viewSettings.indexedMap( "Views" );
    KoOasisSettings::Items firstView = viewMap.entry( 0 );

    KoOasisSettings::NamedMap sheetsMap = firstView.namedMap( "Tables" );
    kdDebug()<<" loadOasisSettings( KoOasisSettings &settings ) exist : "<< !sheetsMap.isNull() <<endl;
    if ( !sheetsMap.isNull() )
    {
        QPtrListIterator<Sheet> it( m_lstSheets );
        for( ; it.current(); ++it )
        {
            it.current()->loadOasisSettings( sheetsMap );
        }
    }

    QString activeSheet = firstView.parseConfigItemString( "ActiveTable" );
    kdDebug()<<" loadOasisSettings( KoOasisSettings &settings ) activeSheet :"<<activeSheet<<endl;

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
    View * view = static_cast<View*>( m_doc->views().getFirst());
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
    QPtrListIterator<Sheet> it( m_lstSheets );
    for( ; it.current(); ++it )
    {
        settingsWriter.startElement( "config:config-item-map-entry" );
        settingsWriter.addAttribute( "config:name", ( *it )->sheetName() );
        if ( view )
        {
          QPoint marker = view->markerFromSheet( *it );
          KoPoint offset = view->offsetFromSheet( *it );
          settingsWriter.addConfigItem( "CursorPositionX", marker.x() );
          settingsWriter.addConfigItem( "CursorPositionY", marker.y() );
          settingsWriter.addConfigItem( "xOffset", offset.x() );
          settingsWriter.addConfigItem( "yOffset", offset.y() );
        }
        it.current()->saveOasisSettings( settingsWriter );
        settingsWriter.endElement();
    }
    settingsWriter.endElement();
}


bool Map::saveOasis( KoXmlWriter & xmlWriter, KoGenStyles & mainStyles, KoStore *store, KoXmlWriter* manifestWriter, int &_indexObj, int &_partIndexObj )
{
    if ( !m_strPassword.isEmpty() )
    {
        xmlWriter.addAttribute("table:structure-protected", "true" );
        QCString str = KCodecs::base64Encode( m_strPassword );
        xmlWriter.addAttribute("table:protection-key", QString( str.data() ) );/* FIXME !!!!*/
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


    QPtrListIterator<Sheet> it( m_lstSheets );
    for( ; it.current(); ++it )
    {
        it.current()->saveOasis( bodyTmpWriter, mainStyles, valStyle, store, manifestWriter, _indexObj, _partIndexObj );
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
  View * view = static_cast<View*>(m_doc->views().getFirst());
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
      QCString str = KCodecs::base64Encode( m_strPassword );
      mymap.setAttribute( "protected", QString( str.data() ) );
    }
    else
      mymap.setAttribute( "protected", "" );
  }

  QPtrListIterator<Sheet> it( m_lstSheets );
  for( ; it.current(); ++it )
  {
    QDomElement e = it.current()->saveXML( doc );
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
        QCString passwd( "" );
        if ( body.hasAttributeNS( KoXmlNS::table, "protection-key" ) )
        {
            QString p = body.attributeNS( KoXmlNS::table, "protection-key", QString::null );
            QCString str( p.latin1() );
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
            //kdDebug()<<"  Map::loadOasis tableElement is not null \n";
            //kdDebug()<<"tableElement.nodeName() :"<<sheetElement.nodeName()<<endl;
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
    QDict<Style> autoStyles = doc()->styleManager()->loadOasisAutoStyles( oasisContext.oasisStyles() );

    // load the sheet
    sheetNode = body.firstChild();
    while ( !sheetNode.isNull() )
    {
        QDomElement sheetElement = sheetNode.toElement();
        if( !sheetElement.isNull() )
        {
            kdDebug()<<"tableElement.nodeName() bis :"<<sheetElement.nodeName()<<endl;
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
    doc()->styleManager()->releaseUnusedAutoStyles( autoStyles );

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
      QCString str( passwd.latin1() );
      m_strPassword = KCodecs::base64Decode( str );
    }
    else
      m_strPassword = QCString( "" );
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
  QPtrListIterator<Sheet> it( m_lstSheets );
  for( ; it.current(); ++it )
    it.current()->recalc();
}

Sheet* Map::findSheet( const QString & _name )
{
    Sheet * t;

    for ( t = m_lstSheets.first(); t != 0L; t = m_lstSheets.next() )
    {
	    if ( _name.lower() == t->sheetName().lower() )
            return t;
    }

    return 0L;
}

Sheet * Map::nextSheet( Sheet * currentSheet )
{
    Sheet * t;

    if( currentSheet == m_lstSheets.last())
      return currentSheet;

    for ( t = m_lstSheets.first(); t != 0L; t = m_lstSheets.next() )
    {
        if ( t  == currentSheet )
            return m_lstSheets.next();
    }

    return 0L;
}

Sheet * Map::previousSheet( Sheet * currentSheet )
{
    Sheet * t;

    if( currentSheet == m_lstSheets.first())
      return currentSheet;

    for ( t = m_lstSheets.first(); t != 0L; t = m_lstSheets.next() )
    {
        if ( t  == currentSheet )
            return m_lstSheets.prev();
    }

    return 0L;
}

bool Map::saveChildren( KoStore * _store )
{
  QPtrListIterator<Sheet> it( m_lstSheets );
  for( ; it.current(); ++it )
  {
    // set the child document's url to an internal url (ex: "tar:/0/1")
    if ( !it.current()->saveChildren( _store, it.current()->sheetName() ) )
      return false;
  }
  return true;
}

bool Map::loadChildren( KoStore * _store )
{
  QPtrListIterator<Sheet> it( m_lstSheets );
  for( ; it.current(); ++it )
    if ( !it.current()->loadChildren( _store ) )
      return false;

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
    int pos = m_lstSheets.findRef( sheet );
    m_lstSheets.take( pos );
    m_lstDeletedSheets.append( sheet );
}

void Map::insertSheet( Sheet * sheet )
{
    int pos = m_lstDeletedSheets.findRef( sheet );
    if ( pos != -1 )
        m_lstDeletedSheets.take( pos );
    m_lstSheets.append(sheet);
}

// FIXME cache this for faster operation
QStringList Map::visibleSheets() const
{
    QStringList result;

    QPtrListIterator<Sheet> it( m_lstSheets );
    for( ; it; ++it )
    {
        Sheet* sheet = it.current();
        if( !sheet->isHidden() )
            result.append( sheet->sheetName() );
    }

    return result;
}

// FIXME cache this for faster operation
QStringList Map::hiddenSheets() const
{
    QStringList result;

    QPtrListIterator<Sheet> it( m_lstSheets );
    for( ; it; ++it )
    {
        Sheet* sheet = it.current();
        if( sheet->isHidden() )
            result.append( sheet->sheetName() );
    }

    return result;
}

#include "kspread_map.moc"

