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

#include "kspread_map.h"
#include "kspread_doc.h"
#include "kspread_util.h"

#include <time.h>
#include <qmsgbox.h>
#include <stdio.h>
#include <stdlib.h>

#include <userpaths.h>

KSpreadMap::KSpreadMap( KSpreadDoc *_doc )
{
  m_pDoc = _doc;
    
  m_lstTables.setAutoDelete( true );
}

KSpreadMap::~KSpreadMap()
{
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

QDomElement KSpreadMap::save( QDomDocument& doc )
{
  QDomElement mymap = doc.createElement( "map" );

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
  QDomNode n = mymap.firstChild();
  while( !n.isNull() )
  {
    QDomElement e = n.toElement();
    if ( !e.isNull() && e.tagName() == "table" )
    {
      printf("----------------- ADDING TABLE ------------------\n");
      KSpreadTable *t = m_pDoc->createTable();
      addTable( t );
      if ( !t->loadXML( e ) )
	return false;
    }
    n = n.nextSibling();
  }

  return true;
}

void KSpreadMap::update()
{
  QListIterator<KSpreadTable> it( m_lstTables );
  for( ; it.current(); ++it )
    it.current()->update();
}

KSpread::TableSeq* KSpreadMap::tables()
{
  KSpread::TableSeq* seq = new KSpread::TableSeq;
  seq->length( m_lstTables.count() );

  KSpreadTable *t;
  int i = 0;
  for ( t = m_lstTables.first(); t != 0L; t = m_lstTables.next() )
    (*seq)[i++] = KSpread::Table::_duplicate( t );
    
  return seq;
}

KSpread::Table_ptr KSpreadMap::table( const char* _name )
{
  KSpreadTable *t = findTable( _name );
  if ( t == 0L )
    return 0L;
  
  return KSpread::Table::_duplicate( t );
}

KSpreadTable* KSpreadMap::findTable( const char *_name )
{
    KSpreadTable *t;
 
    for ( t = m_lstTables.first(); t != 0L; t = m_lstTables.next() )
    {
	if ( strcmp( _name, t->name() ) == 0 )
	    return t;
    }

    return 0L;
}

void KSpreadMap::makeChildList( KOffice::Document_ptr _doc, const char *_path )
{
  QListIterator<KSpreadTable> it( m_lstTables );
  for( ; it.current(); ++it )
  {
    QString path( _path );
    path += "/";
    path += it.current()->name();
    it.current()->makeChildList( _doc, path );
  }
}

bool KSpreadMap::loadChildren( KOStore::Store_ptr _store )
{
  QListIterator<KSpreadTable> it( m_lstTables );
  for( ; it.current(); ++it )
    if ( !it.current()->loadChildren( _store ) )
      return false;
  
  return true;
}

bool KSpreadMap::hasToWriteMultipart()
{
  QListIterator<KSpreadTable> it( m_lstTables );
  for( ; it.current(); ++it )
  {
    if ( it.current()->hasToWriteMultipart() )
      return true;
  }

  return false;
}

void KSpreadMap::draw( QPaintDevice* _dev, CORBA::Long _width, CORBA::Long _height,
		       CORBA::Float _scale )
{
  // Only the view knows about the active table. So we can just assume that
  // embedded KSpread documents do only have one table
  if ( m_lstTables.first() )
    m_lstTables.first()->draw( _dev, _width, _height, _scale );
}

