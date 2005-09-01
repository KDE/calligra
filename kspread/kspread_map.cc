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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/


#include "kspread_map.h"
#include "kspread_doc.h"
#include "kspread_canvas.h"
#include "kspread_view.h"
#include "kspread_locale.h"
#include "kspread_genvalidationstyle.h"
#include "KSpreadMapIface.h"

#include <koOasisSettings.h>
#include <kmdcodec.h>
#include <koGenStyles.h>
#include <koxmlns.h>
#include <kodom.h>
#include <time.h>
#include <stdlib.h>
#include <ktempfile.h>
#include <qfile.h>

bool KSpreadMap::respectCase = true;

KSpreadMap::KSpreadMap ( KSpreadDoc* doc, const char* name)
  : QObject( doc, name ),
    m_initialActiveSheet( 0 ),
    m_initialMarkerColumn( 0 ),
    m_initialMarkerRow( 0 ),
    tableId (1),
    m_doc( doc ),
    m_dcop( 0 )
{
  m_lstSheets.setAutoDelete( true );
}

KSpreadMap::~KSpreadMap()
{
    delete m_dcop;
}

KSpreadDoc* KSpreadMap::doc()
{
  return m_doc;
}

void KSpreadMap::setProtected( QCString const & passwd )
{
  m_strPassword = passwd;
}

KSpreadSheet* KSpreadMap::createSheet()
{
  QString s( i18n("Sheet%1") );
  s = s.arg( tableId++ );
  KSpreadSheet *t = new KSpreadSheet ( this, s , s.utf8());
  t->setSheetName( s, true ); // huh? (Werner)
  return t;
}

void KSpreadMap::addSheet( KSpreadSheet *_sheet )
{
  m_lstSheets.append( _sheet );

  m_doc->setModified( true );

  emit sig_addSheet( _sheet );
}

KSpreadSheet *KSpreadMap::addNewSheet ()
{
  KSpreadSheet *t = createSheet ();
  addSheet (t);
  return t;
}

void KSpreadMap::moveSheet( const QString & _from, const QString & _to, bool _before )
{
  KSpreadSheet* sheetfrom = findSheet( _from );
  KSpreadSheet* sheetto = findSheet( _to );

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

void KSpreadMap::loadOasisSettings( KoOasisSettings &settings )
{
    KoOasisSettings::Items viewSettings = settings.itemSet( "view-settings" );
    KoOasisSettings::IndexedMap viewMap = viewSettings.indexedMap( "Views" );
    KoOasisSettings::Items firstView = viewMap.entry( 0 );

    KoOasisSettings::NamedMap sheetsMap = firstView.namedMap( "Tables" );
    kdDebug()<<" loadOasisSettings( KoOasisSettings &settings ) exist : "<< !sheetsMap.isNull() <<endl;
    if ( !sheetsMap.isNull() )
    {
        QPtrListIterator<KSpreadSheet> it( m_lstSheets );
        for( ; it.current(); ++it )
        {
            it.current()->loadOasisSettings( sheetsMap );
        }
    }

    QString activeSheet = firstView.parseConfigItemString( "ActiveTable" );
    kdDebug()<<" loadOasisSettings( KoOasisSettings &settings ) activeSheet :"<<activeSheet<<endl;

    if (!activeSheet.isEmpty())
    {
        // Used by KSpreadView's constructor
        m_initialActiveSheet = findSheet( activeSheet );
    }

}

void KSpreadMap::saveOasisSettings( KoXmlWriter &settingsWriter )
{
    settingsWriter.addConfigItem( "ViewId", QString::fromLatin1( "View1" ) );
    // Save visual info for the first view, such as active sheet and active cell
    // It looks like a hack, but reopening a document creates only one view anyway (David)
    KSpreadView * view = static_cast<KSpreadView*>( m_doc->views().getFirst());
    if ( view ) // no view if embedded document
    {
        // save current sheet selection before to save marker, otherwise current pos is not saved
        view->saveCurrentSheetSelection();
        KSpreadCanvas * canvas = view->canvasWidget();
        //<config:config-item config:name="ActiveTable" config:type="string">Feuille1</config:config-item>
        settingsWriter.addConfigItem( "ActiveTable",  canvas->activeSheet()->sheetName() );
    }

    //<config:config-item-map-named config:name="Tables">
    settingsWriter.startElement("config:config-item-map-named" );
    settingsWriter.addAttribute("config:name","Tables" );
    QPtrListIterator<KSpreadSheet> it( m_lstSheets );
    for( ; it.current(); ++it )
    {
        QPoint marker;
        if ( view )
        {
            marker = view->markerFromSheet( *it );
        }
        settingsWriter.startElement( "config:config-item-map-entry" );
        settingsWriter.addAttribute( "config:name", ( *it )->sheetName() );
        it.current()->saveOasisSettings( settingsWriter, marker);
        settingsWriter.endElement();
    }
    settingsWriter.endElement();
}


bool KSpreadMap::saveOasis( KoXmlWriter & xmlWriter, KoGenStyles & mainStyles )
{
    if ( !m_strPassword.isEmpty() )
    {
        xmlWriter.addAttribute("table:structure-protected", "true" );
        QCString str = KCodecs::base64Encode( m_strPassword );
        xmlWriter.addAttribute("table:protection-key", QString( str.data() ) );/* FIXME !!!!*/
    }

    KSpreadGenValidationStyles valStyle;

    KTempFile bodyTmpFile;
    bodyTmpFile.setAutoDelete( true );
    QFile* tmpFile = bodyTmpFile.file();
    KoXmlWriter bodyTmpWriter( tmpFile );


    QPtrListIterator<KSpreadSheet> it( m_lstSheets );
    for( ; it.current(); ++it )
    {
        it.current()->saveOasis( bodyTmpWriter, mainStyles, valStyle );
    }

    valStyle.writeStyle( xmlWriter );


    tmpFile->close();
    xmlWriter.addCompleteElement( tmpFile );
    bodyTmpFile.close();

    return true;
}

QDomElement KSpreadMap::save( QDomDocument& doc )
{
  QDomElement mymap = doc.createElement( "map" );
  // Save visual info for the first view, such as active sheet and active cell
  // It looks like a hack, but reopening a document creates only one view anyway (David)
  KSpreadView * view = static_cast<KSpreadView*>(m_doc->views().getFirst());
  if ( view ) // no view if embedded document
  {
    KSpreadCanvas * canvas = view->canvasWidget();
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

  QPtrListIterator<KSpreadSheet> it( m_lstSheets );
  for( ; it.current(); ++it )
  {
    QDomElement e = it.current()->saveXML( doc );
    if ( e.isNull() )
      return e;
    mymap.appendChild( e );
  }

  return mymap;
}

bool KSpreadMap::loadOasis( const QDomElement& body, KoOasisStyles& oasisStyles )
{
    if ( body.hasAttributeNS( KoXmlNS::table, "structure-protected" ) )
    {
        QCString passwd( "" );
        if ( body.hasAttributeNS( KoXmlNS::table, "protection-key" ) )
        {
            QString p = body.attributeNS( KoXmlNS::table, "protection-key", QString::null );
            QCString str( p.latin1() );
            kdDebug(30518) << "Decoding password: " << str << endl;
            passwd = KCodecs::base64Decode( str );
        }
        //todo remove me !!!!!!!! FIXME
        kdDebug(30518) << "Password hash: '" << passwd << "'" << endl;
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
            kdDebug()<<"  KSpreadMap::loadOasis tableElement is not null \n";
            kdDebug()<<"tableElement.nodeName() :"<<sheetElement.nodeName()<<endl;
            if( sheetElement.nodeName() == "table:table" )
            {
                if( !sheetElement.attributeNS( KoXmlNS::table, "name", QString::null ).isEmpty() )
                {
                    KSpreadSheet* sheet = addNewSheet();
                    sheet->setSheetName( sheetElement.attributeNS( KoXmlNS::table, "name", QString::null ), true, false );
                }
            }
        }
        sheetNode = sheetNode.nextSibling();
    }

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
                    KSpreadSheet* sheet = findSheet( name );
                    if( sheet )
                        sheet->loadOasis( sheetElement , oasisStyles );
                }
            }
        }
        sheetNode = sheetNode.nextSibling();
    }

    return true;
}


bool KSpreadMap::loadXML( const QDomElement& mymap )
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
      KSpreadSheet *t = addNewSheet();
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
    // Used by KSpreadView's constructor
    m_initialActiveSheet = findSheet( activeSheet );
  }

  return true;
}

void KSpreadMap::update()
{
  QPtrListIterator<KSpreadSheet> it( m_lstSheets );
  for( ; it.current(); ++it )
    it.current()->recalc();
}

KSpreadSheet* KSpreadMap::findSheet( const QString & _name )
{
    KSpreadSheet * t;

    for ( t = m_lstSheets.first(); t != 0L; t = m_lstSheets.next() )
    {
        if ( _name == t->sheetName() )
            return t;
    }

    return 0L;
}

KSpreadSheet * KSpreadMap::nextSheet( KSpreadSheet * currentSheet )
{
    KSpreadSheet * t;

    if( currentSheet == m_lstSheets.last())
      return currentSheet;

    for ( t = m_lstSheets.first(); t != 0L; t = m_lstSheets.next() )
    {
        if ( t  == currentSheet )
            return m_lstSheets.next();
    }

    return 0L;
}

KSpreadSheet * KSpreadMap::previousSheet( KSpreadSheet * currentSheet )
{
    KSpreadSheet * t;

    if( currentSheet == m_lstSheets.first())
      return currentSheet;

    for ( t = m_lstSheets.first(); t != 0L; t = m_lstSheets.next() )
    {
        if ( t  == currentSheet )
            return m_lstSheets.prev();
    }

    return 0L;
}

bool KSpreadMap::saveChildren( KoStore * _store )
{
  QPtrListIterator<KSpreadSheet> it( m_lstSheets );
  for( ; it.current(); ++it )
  {
    // set the child document's url to an internal url (ex: "tar:/0/1")
    if ( !it.current()->saveChildren( _store, it.current()->sheetName() ) )
      return false;
  }
  return true;
}

bool KSpreadMap::loadChildren( KoStore * _store )
{
  QPtrListIterator<KSpreadSheet> it( m_lstSheets );
  for( ; it.current(); ++it )
    if ( !it.current()->loadChildren( _store ) )
      return false;

  return true;
}

DCOPObject * KSpreadMap::dcopObject()
{
    if ( !m_dcop )
        m_dcop = new KSpreadMapIface( this );

    return m_dcop;
}

void KSpreadMap::takeSheet( KSpreadSheet * sheet )
{
    int pos = m_lstSheets.findRef( sheet );
    m_lstSheets.take( pos );
    m_lstDeletedSheets.append( sheet );
}

void KSpreadMap::insertSheet( KSpreadSheet * sheet )
{
    int pos = m_lstDeletedSheets.findRef( sheet );
    if ( pos != -1 )
        m_lstDeletedSheets.take( pos );
    m_lstSheets.append(sheet);
}

// FIXME cache this for faster operation
QStringList KSpreadMap::visibleSheets() const
{
    QStringList result;

    QPtrListIterator<KSpreadSheet> it( m_lstSheets );
    for( ; it; ++it )
    {
        KSpreadSheet* sheet = it.current();
        if( !sheet->isHidden() )
            result.append( sheet->sheetName() );
    }

    return result;
}

// FIXME cache this for faster operation
QStringList KSpreadMap::hiddenSheets() const
{
    QStringList result;

    QPtrListIterator<KSpreadSheet> it( m_lstSheets );
    for( ; it; ++it )
    {
        KSpreadSheet* sheet = it.current();
        if( sheet->isHidden() )
            result.append( sheet->sheetName() );
    }

    return result;
}

#include "kspread_map.moc"

