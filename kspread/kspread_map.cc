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
#include "kspread_genvalidationstyle.h"
#include "KSpreadMapIface.h"

#include <kmdcodec.h>
#include <koGenStyles.h>
#include <time.h>
#include <stdlib.h>
#include <ktempfile.h>
#include <qfile.h>

bool KSpreadMap::respectCase = true;

KSpreadMap::KSpreadMap( KSpreadDoc *_doc, const char* name )
  : QObject( _doc, name ),
    m_pDoc( _doc ),
    m_initialActiveTable( 0 ),
    m_initialMarkerColumn( 0 ),
    m_initialMarkerRow( 0 ),
    m_dcop( 0 )
{
  m_lstTables.setAutoDelete( true );
}

KSpreadMap::~KSpreadMap()
{
    delete m_dcop;
}

void KSpreadMap::setProtected( QCString const & passwd )
{
  m_strPassword = passwd;
}

void KSpreadMap::addTable( KSpreadSheet *_table )
{
  m_lstTables.append( _table );
}

void KSpreadMap::moveTable( const QString & _from, const QString & _to, bool _before )
{
  KSpreadSheet* tablefrom = findTable( _from );
  KSpreadSheet* tableto = findTable( _to );

  int from = m_lstTables.find( tablefrom ) ;
  int to = m_lstTables.find( tableto ) ;
  if ( !_before )
  ++to;

  if ( to > (int)m_lstTables.count() )
  {
    m_lstTables.append( tablefrom );
    m_lstTables.take( from );
  }
  else if ( from < to )
  {
    m_lstTables.insert( to, tablefrom );
    m_lstTables.take( from );
  }
  else
  {
    m_lstTables.take( from );
    m_lstTables.insert( to, tablefrom );
  }
}

void KSpreadMap::loadOasisSettings( const QDomElement& setting )
{

}

void KSpreadMap::saveOasisSettings( KoXmlWriter &settingsWriter )
{

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


    QPtrListIterator<KSpreadSheet> it( m_lstTables );
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
  // Save visual info for the first view, such as active table and active cell
  // It looks like a hack, but reopening a document creates only one view anyway (David)
  KSpreadView * view = static_cast<KSpreadView*>(this->doc()->views().getFirst());
  if ( view ) // no view if embedded document
  {
    KSpreadCanvas * canvas = view->canvasWidget();
    mymap.setAttribute( "activeTable", canvas->activeTable()->tableName() );
    mymap.setAttribute( "markerColumn", canvas->markerColumn() );
    mymap.setAttribute( "markerRow", canvas->markerRow() );
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

  QPtrListIterator<KSpreadSheet> it( m_lstTables );
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
    if ( body.hasAttribute( "table:structure-protected" ) )
    {
        kdDebug()<<" table:structure-protected !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
        QCString passwd( "" );
        if ( body.hasAttribute( "table:protection-key" ) )
        {
            QString p = body.attribute( "table:protection-key" );
            QCString str( p.latin1() );
            kdDebug(30518) << "Decoding password: " << str << endl;
            passwd = KCodecs::base64Decode( str );
        }
        //todo remove me !!!!!!!! FIXME
        kdDebug(30518) << "Password hash: '" << passwd << "'" << endl;
        m_strPassword = passwd;
    }
    QDomNode tableNode = body.namedItem( "table:table" );

    // sanity check
    if ( tableNode.isNull() ) return false;

    while ( !tableNode.isNull() )
    {
        QDomElement tableElement = tableNode.toElement();
        if( !tableElement.isNull() )
        if( tableElement.nodeName() == "table:table" )
        if( !tableElement.attribute( "table:name" ).isEmpty() )
            {
                KSpreadSheet* sheet = m_pDoc->createTable();
                m_pDoc->addTable( sheet );
                sheet->setTableName( tableElement.attribute( "table:name" ), true, false );
            }

        tableNode = tableNode.nextSibling();
    }

    // load the table
    tableNode = body.firstChild();
    while ( !tableNode.isNull() )
    {
        QDomElement tableElement = tableNode.toElement();
        if( !tableElement.isNull() )
        if( tableElement.nodeName() == "table:table" )
        if( !tableElement.attribute( "table:name" ).isEmpty() )
        {
            QString name = tableElement.attribute( "table:name" );
            KSpreadSheet* sheet = m_pDoc->map()->findTable( name );
            if( sheet )
                sheet->loadOasis( tableElement , oasisStyles );
        }
        tableNode = tableNode.nextSibling();
    }

    return true;
}


bool KSpreadMap::loadXML( const QDomElement& mymap )
{
  QString activeTable = mymap.attribute( "activeTable" );
  m_initialMarkerColumn = mymap.attribute( "markerColumn" ).toInt();
  m_initialMarkerRow = mymap.attribute( "markerRow" ).toInt();

  QDomNode n = mymap.firstChild();
  if ( n.isNull() )
  {
      // We need at least one table !
      m_pDoc->setErrorMessage( i18n("This document has no table.") );
      return false;
  }
  while( !n.isNull() )
  {
    QDomElement e = n.toElement();
    if ( !e.isNull() && e.tagName() == "table" )
    {
      KSpreadSheet *t = m_pDoc->createTable();
      m_pDoc->addTable( t );
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

  if (!activeTable.isEmpty())
  {
    // Used by KSpreadView's constructor
    m_initialActiveTable = findTable( activeTable );
  }

  return true;
}

void KSpreadMap::update()
{
  QPtrListIterator<KSpreadSheet> it( m_lstTables );
  for( ; it.current(); ++it )
    it.current()->recalc();
}

KSpreadSheet* KSpreadMap::findTable( const QString & _name )
{
    KSpreadSheet * t;

    for ( t = m_lstTables.first(); t != 0L; t = m_lstTables.next() )
    {
        if ( _name == t->tableName() )
            return t;
    }

    return 0L;
}

KSpreadSheet * KSpreadMap::nextTable( KSpreadSheet * currentTable )
{
    KSpreadSheet * t;

    if( currentTable == m_lstTables.last())
      return currentTable;

    for ( t = m_lstTables.first(); t != 0L; t = m_lstTables.next() )
    {
        if ( t  == currentTable )
            return m_lstTables.next();
    }

    return 0L;
}

KSpreadSheet * KSpreadMap::previousTable( KSpreadSheet * currentTable )
{
    KSpreadSheet * t;

    if( currentTable == m_lstTables.first())
      return currentTable;

    for ( t = m_lstTables.first(); t != 0L; t = m_lstTables.next() )
    {
        if ( t  == currentTable )
            return m_lstTables.prev();
    }

    return 0L;
}

bool KSpreadMap::saveChildren( KoStore * _store )
{
  QPtrListIterator<KSpreadSheet> it( m_lstTables );
  for( ; it.current(); ++it )
  {
    // set the child document's url to an internal url (ex: "tar:/0/1")
    if ( !it.current()->saveChildren( _store, it.current()->tableName() ) )
      return false;
  }
  return true;
}

bool KSpreadMap::loadChildren( KoStore * _store )
{
  QPtrListIterator<KSpreadSheet> it( m_lstTables );
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

KSpreadDoc * KSpreadMap::doc() const
{
    return m_pDoc;
}

void KSpreadMap::takeTable( KSpreadSheet * table )
{
    int pos = m_lstTables.findRef( table );
    m_lstTables.take( pos );
    m_lstDeletedTables.append( table );
}

void KSpreadMap::insertTable( KSpreadSheet * table )
{
    int pos = m_lstDeletedTables.findRef( table );
    if ( pos != -1 )
        m_lstDeletedTables.take( pos );
    m_lstTables.append(table);
}

// FIXME cache this for faster operation
QStringList KSpreadMap::visibleSheets() const
{
    QStringList result;

    QPtrListIterator<KSpreadSheet> it( m_lstTables );
    for( ; it; ++it )
    {
        KSpreadSheet* sheet = it.current();
        if( !sheet->isHidden() )
            result.append( sheet->tableName() );
    }

    return result;
}

// FIXME cache this for faster operation
QStringList KSpreadMap::hiddenSheets() const
{
    QStringList result;

    QPtrListIterator<KSpreadSheet> it( m_lstTables );
    for( ; it; ++it )
    {
        KSpreadSheet* sheet = it.current();
        if( sheet->isHidden() )
            result.append( sheet->tableName() );
    }

    return result;
}
