/* This file is part of the KDE project
   Copyright (C) 2002 Norbert Andres, nandres@web.de

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

#include "kspread_cell.h"
#include "kspread_changes.h"
#include "kspread_map.h"
#include "kspread_sheet.h"

#include <kconfig.h>
#include <kmdcodec.h>
#include <qdom.h>

FilterSettings::FilterSettings()
  : m_dateSet( false ),
    m_dateUsage( 0 ),
    m_firstTime( QDateTime::currentDateTime() ),
    m_secondTime( QDateTime::currentDateTime() ),
    m_authorSet( false ),
    m_commentSet( false ),
    m_rangeSet( false ),
    m_showChanges( true ),
    m_showAccepted( false ),
    m_showRejected( false )
{
}

bool FilterSettings::loadXml( QDomElement const & settings )
{
  return false;
}

void FilterSettings::saveXml( QDomDocument & doc, QDomElement & parent ) const
{
}

void FilterSettings::setShowChanges( bool b )
{
  m_showChanges = b;
}

void FilterSettings::setShowAccepted( bool b )
{
  m_showAccepted = b;
}

void FilterSettings::setShowRejected( bool b )
{
  m_showRejected = b;
}


KSpreadChanges::KSpreadChanges( KSpreadMap * map )
  : m_counter( 0 ),
    m_map( map ),
    m_locked( false )
{
  m_dependancyList.setAutoDelete( false );
  m_authors.setAutoDelete( true );

  KConfig * emailCfg = new KConfig( "emaildefaults", true );
  emailCfg->setGroup( "Defaults" );
  m_name = emailCfg->readEntry( "FullName" );
}

KSpreadChanges::~KSpreadChanges()
{
  RecordMap::iterator iter = m_changeRecords.begin();
  RecordMap::iterator end  = m_changeRecords.end();
  
  while ( iter != end )
  {
    ChangeRecord * record = iter.data();

    delete record;

    ++iter;
  }
}

void KSpreadChanges::setProtected( QCString const & hash )
{
  m_strPassword = hash;
}

void KSpreadChanges::saveXml( QDomDocument & doc, QDomElement & map )
{
  kdDebug() << "Entering saveXML" << endl;
  if ( m_changeRecords.size() == 0 )
    return;

  QDomElement records = doc.createElement( "tracked-changes" );

  if ( !m_strPassword.isNull() )
  {
    if ( m_strPassword.size() > 0 )
    {
      QCString str = KCodecs::base64Encode( m_strPassword ); 
      records.setAttribute( "protected", QString( str.data() ) );
    }
    else
      records.setAttribute( "protected", "" );      
  }

  kdDebug() << "Saving authors" << endl;

  saveAuthors( doc, records );
  kdDebug() << "Saving changes" << endl;
  saveChanges( doc, records );

  kdDebug() << "Saving done" << endl;
  map.appendChild( records );
}

void KSpreadChanges::saveAuthors( QDomDocument & doc, QDomElement & changes )
{
  if ( m_authors.first() == 0 )
    return;

  QDomElement authors = doc.createElement( "authors" );
  QPtrListIterator<AuthorInfo> it( m_authors );
  for ( ; it.current(); ++it )
  {
    QDomElement author = doc.createElement( "author" );
    author.setAttribute( "id",   QString::number( it.current()->id() ) );
    author.setAttribute( "name", it.current()->name() );
    authors.appendChild( author );
  }
  changes.appendChild( authors );
}

void KSpreadChanges::saveChanges( QDomDocument & doc, QDomElement & changes )
{
  QDomElement records = doc.createElement( "changes" );

  RecordMap::iterator iter = m_changeRecords.begin();
  RecordMap::iterator end  = m_changeRecords.end();
  
  while ( iter != end )
  {
    ChangeRecord * record = iter.data();

    record->saveXml( doc, records );

    ++iter;
  }
  changes.appendChild( records );
}

bool KSpreadChanges::loadXml( QDomElement const & changes )
{
  if ( changes.hasAttribute( "protected" ) )
  {
    QString passwd = changes.attribute( "protected" );
    
    if ( passwd.length() > 0 )
    {
      QCString str( passwd.latin1() );
      m_strPassword = KCodecs::base64Decode( str );        
    }
    else
      m_strPassword = QCString( "" );
  }

  QDomNode n = changes.firstChild();
  while( !n.isNull() )
  {
    QDomElement e = n.toElement();
    if ( !e.isNull() && e.tagName() == "changes" )
    {
      if ( !loadChanges( e ) )
        return false;
    }
    else
    if ( !e.isNull() && e.tagName() == "authors" )
    {
      if ( !loadAuthors( e ) )
        return false;      
    }
    n = n.nextSibling();
  }

  RecordMap::iterator iter = m_changeRecords.begin();
  RecordMap::iterator end  = m_changeRecords.end();
  
  while ( iter != end )
  {
    ChangeRecord * record = iter.data();

    if ( record->dependancies() == 0 )
      m_dependancyList.append( record );

    ++iter;
  }  

  return true;
}

bool KSpreadChanges::loadAuthors( QDomElement const & authors )
{
  QDomNode n = authors.firstChild();
  while( !n.isNull() )
  {
    QDomElement e = n.toElement();
    if ( !e.isNull() && e.tagName() == "author" )
    {
      int id;
      bool ok = false;
      if ( e.hasAttribute( "id" ) )
        id = e.attribute( "id" ).toInt( &ok );

      if ( ok &&  e.hasAttribute( "name" ) )
      {          
        AuthorInfo * info = new AuthorInfo( id, e.attribute( "name" ) );
        m_authors.append( info );
      }
      else 
        return false;
    }
    n = n.nextSibling();
  }

  return true;
}

bool KSpreadChanges::loadChanges( QDomElement const & changes )
{
  QDomElement e = changes.firstChild().toElement();
  while( !e.isNull() )
  {
    if ( !e.hasAttribute( "id" ) )
    {
      e = e.nextSibling().toElement();
      continue;
    }
    bool ok = false;
    int id = e.attribute( "id" ).toInt( &ok );

    if ( ok )
    {
      ChangeRecord * record = m_changeRecords[id];

      if ( !record )
        record = new ChangeRecord();
      
      if ( record->loadXml( e, m_map, m_changeRecords ) )
        m_changeRecords[ record->id() ] = record;
      else
      {
        delete record;
        return false;
      }
    }

    e = e.nextSibling().toElement();
  }

  return true;
}

void KSpreadChanges::fillDependancyList()
{
  RecordMap::iterator iter = m_changeRecords.begin();
  RecordMap::iterator end  = m_changeRecords.end();
  
  while ( iter != end )
  {
    ChangeRecord * record = iter.data();
    
    if ( record->dependancies() == 0 )
      m_dependancyList.append( record );
    
    ++iter;
  }
}

void KSpreadChanges::addChange( KSpreadSheet * table, KSpreadCell * cell, QPoint const & point,
                                QString const & oldFormat, QString const & oldValue, bool hasDepandancy )
{
  if ( m_locked )
    return;

  ++m_counter;
  CellChange * change  = new CellChange();
  change->authorID     = addAuthor();
  change->formatString = oldFormat;
  change->oldValue     = oldValue;
  change->cell         = cell;
  QPoint cellRef( cell->column(), cell->row() );

  ChangeRecord * record = new ChangeRecord( m_counter, ChangeRecord::PENDING, ChangeRecord::CELL,
                                            table, cellRef, change );
  m_changeRecords[ record->id() ] = record;

  if ( hasDepandancy )
  {
    // find records we depend on
    ChangeRecord * r = 0;

    for ( r = m_dependancyList.last(); r; r = m_dependancyList.prev() )
    {
      if ( r->isDependant( table, cellRef ) )
      {
        r->addDependant( record, cellRef );
        return;
      }
    }
  }

  // nothing, so we add ourself to the end
  m_dependancyList.append( record );
}

int KSpreadChanges::addAuthor()
{
  int id = m_authors.count();
  QPtrListIterator<AuthorInfo> it( m_authors );
  for( ; it.current(); ++it )
  {
    if ( it.current()->name() == m_name )
      return it.current()->id();
  }

  AuthorInfo * info  = new AuthorInfo( id, m_name );

  m_authors.append( info );

  return id;
}

QString KSpreadChanges::getAuthor( int id )
{
  QPtrListIterator<AuthorInfo> it( m_authors );
  for( ; it.current(); ++it )
  {
    if ( it.current()->id() == id )
      return it.current()->name();
  }

  return "";
}


/*
 * class ChangeRecord
 */

KSpreadChanges::ChangeRecord::ChangeRecord()
  : m_dependancies( 0 )
{
  m_dependants.setAutoDelete( false );
}

KSpreadChanges::ChangeRecord::ChangeRecord( int id, State state, ChangeType type, KSpreadSheet * table, 
                                            QPoint const & cellRef, Change * change )
  : m_id( id ), m_state( state ), m_type( type ), m_table ( table ),
    m_cell( cellRef ), m_change( change ), m_dependancies( 0 )
{
  m_dependants.setAutoDelete( false );
}

KSpreadChanges::ChangeRecord::~ChangeRecord()
{
  switch( m_type )
  {
   case CELL:
    delete (CellChange * ) m_change;
    break;

   default:
    delete m_change;
  }
}

void KSpreadChanges::ChangeRecord::saveXml( QDomDocument & doc, QDomElement & changes ) const
{
  kdDebug() << "Entering ChangeRecord::saveXML" << endl;

  QDomElement change = doc.createElement( "record" );
  change.setAttribute( "y",     QString::number( m_cell.y() ) );
  change.setAttribute( "x",     QString::number( m_cell.x() ) );
  change.setAttribute( "id",    QString::number( m_id ) );
  change.setAttribute( "state", QString::number( (int) m_state ) );
  change.setAttribute( "type",  QString::number( (int) m_type ) );
  change.setAttribute( "table", m_table->tableName() );

  kdDebug() << "save dependants entries" << endl;

  QPtrListIterator<ChangeRecord> it( m_dependants );
  for ( ; it.current(); ++it )
  {
    QDomElement dep = doc.createElement( "dependant" );
    dep.setAttribute( "id", it.current()->id() );
    change.appendChild( dep );
  }  

  kdDebug() << "saving change entry: " << m_change << endl;
  m_change->saveXml( doc, change );

  changes.appendChild( change );
}

bool KSpreadChanges::ChangeRecord::loadXml( QDomElement & change, KSpreadMap * map, RecordMap & records )
{
  int y = 0;
  int x = 0;
  bool ok = false;
  if ( change.hasAttribute( "y" ) )
    y = change.attribute( "y" ).toInt( &ok );
  else
    return false;
  if ( !ok ) return false;

  if ( change.hasAttribute( "x" ) )
    x = change.attribute( "x" ).toInt( &ok );
  else
    return false;
  if ( !ok ) return false;

  m_cell.setX( x );
  m_cell.setY( y );

  if ( change.hasAttribute( "id" ) )
    y = change.attribute( "id" ).toInt( &ok );
  else
    return false;
  if ( !ok ) return false;
  
  m_id = y;

  if ( change.hasAttribute( "state" ) )
    y = change.attribute( "state" ).toInt( &ok );
  else
    return false;
  if ( !ok ) return false;
  
  m_state = (State) y;

  if ( change.hasAttribute( "type" ) )
    y = change.attribute( "type" ).toInt( &ok );
  else
    return false;
  if ( !ok ) return false;
  
  m_type = (ChangeType) y;

  if ( change.hasAttribute( "table" ) )
  {
    m_table = map->findTable( change.attribute( "table" ) );
    if ( !m_table )
      return false;
  }
  else
    return false;

  switch ( m_type )
  {
   case CELL:
    {
      QDomElement cell = change.namedItem( "cell" ).toElement();
      if ( cell.isNull() )
        return false;
      CellChange * ch = new CellChange();
      ch->loadXml( cell, m_table, m_cell );
      
      m_change = ch;
    }
    break;

   default:
    return false;
  }

  QDomElement dependant = change.namedItem( "dependant" ).toElement();
  while ( !dependant.isNull() )
  {
    if ( dependant.tagName() == "dependant" )
    {
      int id = 0;
      if ( dependant.hasAttribute( "id" ) )
      {
        bool ok = false;
        id = dependant.attribute( "id" ).toInt( &ok );
        if ( ok )
        {
          ChangeRecord * record = records[id];
          if ( !record )
          {
            record = new ChangeRecord();
            records[id] = record;
          }
          record->increaseDependancyCounter();
          m_dependants.append( record );
        }
      }
    }
    dependant = dependant.nextSibling().toElement();
  }
  
  return true;
}

bool KSpreadChanges::ChangeRecord::isDependant( KSpreadSheet const * const table, QPoint const & cell ) const
{
  if ( table != m_table )
    return false;

  if ( cell.x() == m_cell.x() && cell.y() == m_cell.y() )
    return true;

  if ( cell.x() != 0 && ( cell.x() == m_cell.x() && m_cell.y() == 0 ) )
    return true;

  if ( cell.y() != 0 && ( cell.y() == m_cell.y() && m_cell.x() == 0 ) )
    return true;

  return false;
}

void KSpreadChanges::ChangeRecord::addDependant( ChangeRecord * record, QPoint const & cellRef )
{
  bool added = false;
  QPtrListIterator<ChangeRecord> it( m_dependants );
  for ( ; it.current(); ++it )
  {
    if ( it.current()->isDependant( record->table(), cellRef ) )
    {
      added = true;
      it.current()->addDependant( record, cellRef );
    }
  }
  if ( !added )
    m_dependants.append( record );
}

/*
 * class Change
 */

KSpreadChanges::Change::~Change()
{
  delete comment;
}

/*
 * class CellChange
 */

KSpreadChanges::CellChange::~CellChange()
{
  delete comment;
  comment = 0;
}

bool KSpreadChanges::CellChange::loadXml( QDomElement const & change, 
                                          KSpreadSheet const * const table,
                                          QPoint const & cellRef )
{
  kdDebug() << "Loading CellChange object" << endl;
  bool ok = false;
  if ( change.hasAttribute( "author" ) )
  {
    authorID = change.attribute( "author" ).toInt( &ok );
    if ( !ok ) 
      return false;
  }

  if ( change.hasAttribute( "time" ) )
  {
    uint n = (uint) change.attribute( "time" ).toInt( &ok );

    if ( !ok )
      return false;

    timestamp.setTime_t( n );
  }

  if ( change.hasAttribute( "comment" ) )
    comment = new QString( change.attribute( "comment" ) );

  if ( change.hasAttribute( "format" ) )
    formatString = change.attribute( "format" );
  else return false;

  if ( change.hasAttribute( "oldValue" ) )
    oldValue = change.attribute( "oldValue" );
  else return false;

  cell = table->cellAt( cellRef.x(), cellRef.y() );
  if ( !cell )
    return false;

  return true;
}

void KSpreadChanges::CellChange::saveXml( QDomDocument & doc, QDomElement & change ) const
{
  kdDebug() << "Saving CellChange object" << endl;
  QDomElement cellChange = doc.createElement( "cell" );
  cellChange.setAttribute( "author", QString::number( authorID ) );
  cellChange.setAttribute( "time",   QString::number( (int) timestamp.toTime_t() ) );
  if ( comment && !comment->isNull() )
    cellChange.setAttribute( "comment", *comment );
  cellChange.setAttribute( "format", formatString );
  cellChange.setAttribute( "oldValue", oldValue );  

  change.appendChild( cellChange );
}




