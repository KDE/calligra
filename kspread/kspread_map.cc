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

#include <komlWriter.h>
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

void KSpreadMap::moveTable( const char* _from, const char* _to, bool _before )
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

bool KSpreadMap::save( ostream& out )
{
  out << otag << "<MAP>" << endl;
  
  QListIterator<KSpreadTable> it( m_lstTables );
  for( ; it.current(); ++it )
    it.current()->save( out );
  
  out << etag << "</MAP>" << endl;

  return true;
}

bool KSpreadMap::load( KOMLParser& parser, vector<KOMLAttrib>& )
{
  string tag, name;
  vector<KOMLAttrib> lst;
  
  // TABLE
  while( parser.open( 0L, tag ) )
  {
    KOMLParser::parseTag( tag.c_str(), name, lst );
 
    if ( name == "TABLE" )
    {
      KSpreadTable *t = m_pDoc->createTable();
      addTable( t );
      if ( !t->load( parser, lst ) )
	return false;
    }
    else
      cerr << "Unknown tag '" << tag << "'" << endl;

    if ( !parser.close( tag ) )
    {
      cerr << "ERR: Closing Child" << endl;
      return false;
    }
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

bool KSpreadMap::saveChildren( KOStore::Store_ptr _store, const char *_path )
{
  QListIterator<KSpreadTable> it( m_lstTables );
  for( ; it.current(); ++it )
  {
    // set the child document's url to an internal url (ex: "tar:/0/1")
    QString path = QString( "%1/%2" ).arg( _path ).arg( it.current()->name() );
    if ( !it.current()->saveChildren( _store, path ) )
      return false;
  }
  return true;
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

