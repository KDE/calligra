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

#ifndef __kspread_map_h__
#define __kspread_map_h__

class KSpreadMap;
class KSpreadDoc;

#include <iostream.h>
#include <komlParser.h>

#include <qlist.h>
#include <qstring.h>
#include <qintdict.h>

#include "kspread.h"
#include "kspread_table.h"

/**
  A map is a simple container for all tables. Usually a complete map
  is saved in one file.
 */
class KSpreadMap : virtual public KSpread::Book_skel
{
public:
  /**
   * Created an empty map.
   */
  KSpreadMap( KSpreadDoc *_doc );
  /**
   * This deletes all tables contained in this map.
   */
  virtual ~KSpreadMap();
  
  // IDL
  virtual KSpread::TableSeq* tables();
  virtual KSpread::Table_ptr table( const char* name );
  
  // C++
  virtual bool save( ostream& );
  virtual bool load( KOMLParser&, vector<KOMLAttrib>& );
  virtual bool loadChildren( KOStore::Store_ptr _store );
  
  void makeChildList( KOffice::Document_ptr _doc, const char *_path );
  /*
   * @return true if one of the direct children wants to
   *              be saved embedded. If there are no children or if
   *              every direct child saves itself into its own file
   *              then false is returned.
   * 
   */
  bool hasToWriteMultipart();
  
  /**
   * @param _table becomes added to the map.
   */
  void addTable( KSpreadTable *_table );
  
  /**
   * @param _tables becomes removed from the map. This wont delete the table.
   */
  void removeTable( KSpreadTable *_table );

  /**
   * @param _from is being moved to @param _to.
   */ 
  void moveTable( const char* _from, const char* to );
 
  KSpreadTable* findTable( const char *_name );
        
  /**
   * Use the @ref #nextTable function to get all the other tables.
   * Attention: Function is not reentrant.
   *
   * @return a pointer to the first table in this map.
   */
  KSpreadTable* firstTable() { return m_lstTables.first();  }

  /**
   * Call @ref #firstTable first. This will set the list pointer to
   * the first table. Attention: Function is not reentrant.
   *
   * @return a pointer to the next table in this map.
   */
  KSpreadTable* nextTable() { return m_lstTables.next();  }
  
  QList<KSpreadTable>& tableList() { return m_lstTables; }
  
  /**
   * @return amount of tables in this map.
   */
  int count() { return m_lstTables.count(); }
    
  void update();
  
  /**
   * Needed for the printing Extension KOffice::Print
   */
  void draw( QPaintDevice* _dev, CORBA::Long _width, CORBA::Long _height,
	       CORBA::Float _scale );
  
protected:
  /**
   * List of all tables in this map. The list has autodelete turned on.
   */
  QList<KSpreadTable> m_lstTables;

  /**
   * Pointer to the part which holds this map.
   */
  KSpreadDoc *m_pDoc;
};

#endif
