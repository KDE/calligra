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

class KoStore;

class DCOPObject;

class QDomElement;
class QDomDocument;

#include <qlist.h>
#include <qstring.h>
#include <qintdict.h>
#include <qobject.h>

#include "kspread_table.h"

/**
  A map is a simple container for all tables. Usually a complete map
  is saved in one file.
 */
class KSpreadMap : public QObject
{
public:
  /**
   * Created an empty map.
   */
  KSpreadMap( KSpreadDoc *_doc, const char* name = 0 );
  /**
   * This deletes all tables contained in this map.
   */
  virtual ~KSpreadMap();

  QDomElement save( QDomDocument& doc );
  bool loadXML( const QDomElement& mymap );
  bool loadChildren( KoStore* _store );

  bool saveChildren( KoStore* _store, const QString &_path );

  /**
   * @param _table becomes added to the map.
   */
  void addTable( KSpreadTable *_table );

  /**
   * @param _tables becomes removed from the map. This won't delete the table.
   */
  void removeTable( KSpreadTable *_table );

  /**
   * The table named @param _from is being moved to the table @param _to.
   * If @param _before is true @param _from is inserted before (after otherwise)   * @param _to.
   */
  void moveTable( const QString & _from, const QString & _to, bool _before = true );

  KSpreadTable* findTable( const QString & _name );
  KSpreadTable* nextTable( KSpreadTable* );
  KSpreadTable* previousTable( KSpreadTable* );

  KSpreadTable* initialActiveTable() { return m_initialActiveTable; }
  int initialMarkerColumn() { return m_initialMarkerColumn; }
  int initialMarkerRow() { return m_initialMarkerRow; }

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
    // void draw( QPaintDevice* _dev, long int _width, long int _height,
    // float _scale );

  virtual DCOPObject* dcopObject();

  KSpreadDoc* doc();

private:
  /**
   * List of all tables in this map. The list has autodelete turned on.
   */
  QList<KSpreadTable> m_lstTables;

  /**
   * Pointer to the part which holds this map.
   */
  KSpreadDoc *m_pDoc;

  /**
   * Set from the XML
   */
  KSpreadTable * m_initialActiveTable;
  int m_initialMarkerColumn;
  int m_initialMarkerRow;

  DCOPObject* m_dcop;
};

#endif
