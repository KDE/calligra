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

#include <qprinter.h>
#include <qdom.h>
#include <qmessagebox.h>

#include "kspread_map.h"
#include "kspread_doc.h"
#include "kspread_view.h"
#include "kspread_canvas.h"
#include "kspread_table.h"
#include "kspread_util.h"

#include "KSpreadMapIface.h"

#include <time.h>
#include <stdlib.h>

KSpreadMap::KSpreadMap( KSpreadDoc *_doc, const char* name )
    : QObject( _doc, name )
{
  m_pDoc = _doc;
  m_dcop = 0;
  m_initialActiveTable = 0L;
  m_initialMarkerColumn = 0;
  m_initialMarkerRow = 0;

  m_lstTables.setAutoDelete( true );
}

KSpreadMap::~KSpreadMap()
{
    delete m_dcop;
}

void KSpreadMap::addTable( KSpreadTable *_table )
{
  m_lstTables.append( _table );
}

void KSpreadMap::removeTable( KSpreadTable *_table )
{
  m_lstTables.setAutoDelete( false );
  m_lstTables.removeRef( _table );
  m_lstTables.setAutoDelete( true );
}

void KSpreadMap::moveTable( const QString & _from, const QString & _to, bool _before )
{
  KSpreadTable* tablefrom = findTable( _from );
  KSpreadTable* tableto = findTable( _to );

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

QDomElement KSpreadMap::save( QDomDocument& doc )
{
  QDomElement mymap = doc.createElement( "map" );
  // Save visual info for the first view, such as active table and active cell
  // It looks like a hack, but reopening a document creates only one view anyway (David)
  KSpreadView * view = static_cast<KSpreadView*>(this->doc()->firstView());
  if ( view ) // no view if embedded document
  {
    KSpreadCanvas * canvas = view->canvasWidget();
    mymap.setAttribute( "activeTable", canvas->activeTable()->name() );
    mymap.setAttribute( "markerColumn", canvas->markerColumn() );
    mymap.setAttribute( "markerRow", canvas->markerRow() );
  }

  QListIterator<KSpreadTable> it( m_lstTables );
  for( ; it.current(); ++it )
  {
    QDomElement e = it.current()->save( doc );
    if ( e.isNull() )
      return e;
    mymap.appendChild( e );
  }

  return mymap;
}

bool KSpreadMap::loadXML( const QDomElement& mymap )
{
  QString activeTable = mymap.attribute( "activeTable" );
  m_initialMarkerColumn = mymap.attribute( "markerColumn" ).toInt();
  m_initialMarkerRow = mymap.attribute( "markerRow" ).toInt();
  QDomNode n = mymap.firstChild();
  while( !n.isNull() )
  {
    QDomElement e = n.toElement();
    if ( !e.isNull() && e.tagName() == "table" )
    {
      KSpreadTable *t = m_pDoc->createTable();
      m_pDoc->addTable( t );
      if ( !t->loadXML( e ) )
        return false;
    }
    n = n.nextSibling();
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
  QListIterator<KSpreadTable> it( m_lstTables );
  for( ; it.current(); ++it )
    it.current()->update();
}

KSpreadTable* KSpreadMap::findTable( const QString & _name )
{
    KSpreadTable *t;

    for ( t = m_lstTables.first(); t != 0L; t = m_lstTables.next() )
    {
        if ( _name == t->tableName() )
            return t;
    }

    return 0L;
}

KSpreadTable* KSpreadMap::nextTable( KSpreadTable* currentTable )
{
    KSpreadTable *t;

    if( currentTable == m_lstTables.last())
      return currentTable;

    for ( t = m_lstTables.first(); t != 0L; t = m_lstTables.next() )
    {
        if ( t  == currentTable )
            return m_lstTables.next();
    }

    return 0L;
}

KSpreadTable* KSpreadMap::previousTable( KSpreadTable* currentTable )
{
    KSpreadTable *t;

    if( currentTable == m_lstTables.first())
      return currentTable;

    for ( t = m_lstTables.first(); t != 0L; t = m_lstTables.next() )
    {
        if ( t  == currentTable )
            return m_lstTables.prev();
    }

    return 0L;
}

bool KSpreadMap::saveChildren( KoStore* _store, const QString &_path )
{
  QListIterator<KSpreadTable> it( m_lstTables );
  for( ; it.current(); ++it )
  {
    // set the child document's url to an internal url (ex: "tar:/0/1")
    QString path = QString( "%1/%2" ).arg( _path ).arg( it.current()->tableName() );
    if ( !it.current()->saveChildren( _store, path ) )
      return false;
  }
  return true;
}

bool KSpreadMap::loadChildren( KoStore* _store )
{
  QListIterator<KSpreadTable> it( m_lstTables );
  for( ; it.current(); ++it )
    if ( !it.current()->loadChildren( _store ) )
      return false;

  return true;
}

DCOPObject* KSpreadMap::dcopObject()
{
    if ( !m_dcop )
        m_dcop = new KSpreadMapIface( this );

    return m_dcop;
}

KSpreadDoc* KSpreadMap::doc()
{
    return m_pDoc;
}
