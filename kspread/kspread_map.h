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

class KSpreadChanges;
class KSpreadDoc;
class KSpreadMap;

class KoStore;

class DCOPObject;

class QDomElement;
class QDomDocument;
class KoXmlWriter;
class KoGenStyles;
class KoOasisSettings;

#include <qcstring.h>
#include <qptrlist.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qintdict.h>
#include <qobject.h>

#include "kspread_sheet.h"

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

    void saveOasisSettings( KoXmlWriter &settingsWriter );
    void loadOasisSettings( KoOasisSettings &settings );

    bool saveOasis( KoXmlWriter & xmlWriter, KoGenStyles & mainStyles );

    bool loadOasis( const QDomElement& mymap, KoOasisStyles& oasisStyles );
  bool loadXML( const QDomElement& mymap );
  bool loadChildren( KoStore* _store );

  bool saveChildren( KoStore* _store );

  void password( QCString & passwd ) const { passwd = m_strPassword; }
  bool isProtected() const { return !m_strPassword.isNull(); }
  void setProtected( QCString const & passwd );
  bool checkPassword( QCString const & passwd ) const { return ( passwd == m_strPassword ); }

  /**
   * The table named @param _from is being moved to the table @param _to.
   * If @param _before is true @param _from is inserted before (after otherwise)   * @param _to.
   */
  void moveTable( const QString & _from, const QString & _to, bool _before = true );

  KSpreadSheet* findTable( const QString & _name );
  KSpreadSheet* nextTable( KSpreadSheet* );
  KSpreadSheet* previousTable( KSpreadSheet* );

  KSpreadSheet* initialActiveTable()const { return m_initialActiveTable; }
  int initialMarkerColumn()const { return m_initialMarkerColumn; }
  int initialMarkerRow()const { return m_initialMarkerRow; }

  void addTable( KSpreadSheet *_table );

  /**
   * Use the @ref #nextTable function to get all the other tables.
   * Attention: Function is not reentrant.
   *
   * @return a pointer to the first table in this map.
   */
  KSpreadSheet* firstTable() { return m_lstTables.first();  }

  /**
   * Use the @ref #previousTable function to get all the other tables.
   * Attention: Function is not reentrant.
   *
   * @return a pointer to the last table in this map.
   */
  KSpreadSheet* lastTable() { return m_lstTables.last();  }

  /**
   * Call @ref #firstTable first. This will set the list pointer to
   * the first table. Attention: Function is not reentrant.
   *
   * @return a pointer to the next table in this map.
   */
  KSpreadSheet* nextTable() { return m_lstTables.next();  }

  QPtrList<KSpreadSheet>& tableList() { return m_lstTables; }

  /**
   * @return amount of tables in this map.
   */
  int count()const { return m_lstTables.count(); }

  void update();

  /**
   * Needed for the printing Extension KOffice::Print
   */
    // void draw( QPaintDevice* _dev, long int _width, long int _height,
    // float _scale );

  virtual DCOPObject* dcopObject();

  KSpreadDoc * doc()const;

  void takeTable( KSpreadSheet * table );
  void insertTable( KSpreadSheet * table );

  QStringList visibleSheets() const;
  QStringList hiddenSheets() const;

    static bool respectCase;

private:
  /**
   * List of all tables in this map. The list has autodelete turned on.
   */
  QPtrList<KSpreadSheet> m_lstTables;
  QPtrList<KSpreadSheet> m_lstDeletedTables;

  /**
   * Pointer to the part which holds this map.
   */
  KSpreadDoc *m_pDoc;

  /**
   * Password to protect the map from being changed.
   */
  QCString m_strPassword;
  /**
   * Set from the XML
   */
  KSpreadSheet * m_initialActiveTable;
  int m_initialMarkerColumn;
  int m_initialMarkerRow;

  DCOPObject* m_dcop;
};

#endif
