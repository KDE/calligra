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

#ifndef __kspread_map_h__
#define __kspread_map_h__

class KSpreadChanges;
class KSpreadMap;
class KSpreadDoc;
class KSpreadSheet;

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

#include <koffice_export.h>

/**
  A map is a simple container for all sheets. Usually a complete map
  is saved in one file.
 */
class KSPREAD_EXPORT KSpreadMap : public QObject
{
Q_OBJECT
public:
  /**
   * Created an empty map.
   */
  KSpreadMap ( KSpreadDoc* doc, const char* name = 0);
  /**
   * This deletes all sheets contained in this map.
   */
  virtual ~KSpreadMap();
  
  KSpreadDoc* doc();

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
   * The sheet named @p _from is being moved to the sheet @p _to.
   * If @p  _before is true @p _from is inserted before (after otherwise)   
   * @p  _to.
   */
  void moveSheet( const QString & _from, const QString & _to, bool _before = true );

  KSpreadSheet* findSheet( const QString & _name );
  KSpreadSheet* nextSheet( KSpreadSheet* );
  KSpreadSheet* previousSheet( KSpreadSheet* );

  KSpreadSheet* initialActiveSheet()const { return m_initialActiveSheet; }
  int initialMarkerColumn() const { return m_initialMarkerColumn; }
  int initialMarkerRow()    const { return m_initialMarkerRow; }
  double initialXOffset()   const { return m_initialXOffset; }
  double initialYOffset()   const { return m_initialYOffset; }


  /**
   * @return a pointer to a new KSpreadSheet. The KSpreadSheet is not added
   *         to the map nor added to the GUI.
   */
  KSpreadSheet * createSheet();
  /** add sheet to the map, making it active */
  void addSheet( KSpreadSheet *_sheet );

  /** add a new sheet to the map, returning a pointer to it */
  KSpreadSheet *addNewSheet ();
  
  /**
   * Use the @ref #nextSheet function to get all the other sheets.
   * Attention: Function is not reentrant.
   *
   * @return a pointer to the first sheet in this map.
   */
  KSpreadSheet* firstSheet() { return m_lstSheets.first();  }

  /**
   * Use the previousSheet() function to get all the other sheets.
   * Attention: Function is not reentrant.
   *
   * @return a pointer to the last sheet in this map.
   */
  KSpreadSheet* lastSheet() { return m_lstSheets.last();  }

  /**
   * Call @ref #firstSheet first. This will set the list pointer to
   * the first sheet. Attention: Function is not reentrant.
   *
   * @return a pointer to the next sheet in this map.
   */
  KSpreadSheet* nextSheet() { return m_lstSheets.next();  }

  QPtrList<KSpreadSheet>& sheetList() { return m_lstSheets; }

  /**
   * @return amount of sheets in this map.
   */
  int count()const { return m_lstSheets.count(); }

  void update();

  /**
   * Needed for the printing Extension KOffice::Print
   */
    // void draw( QPaintDevice* _dev, long int _width, long int _height,
    // float _scale );

  virtual DCOPObject* dcopObject();

  void takeSheet( KSpreadSheet * sheet );
  void insertSheet( KSpreadSheet * sheet );

  QStringList visibleSheets() const;
  QStringList hiddenSheets() const;

    static bool respectCase;

signals:

  /**
   * Emitted if a new table is added to the document.
   */
  void sig_addSheet( KSpreadSheet *_table );
private:

  KSpreadDoc* m_doc;

  /**
   * List of all sheets in this map. The list has autodelete turned on.
   */
  QPtrList<KSpreadSheet> m_lstSheets;
  QPtrList<KSpreadSheet> m_lstDeletedSheets;

  /**
   * Password to protect the map from being changed.
   */
  QCString m_strPassword;
  /**
   * Set from the XML
   */
  KSpreadSheet * m_initialActiveSheet;
  int m_initialMarkerColumn;
  int m_initialMarkerRow;
  double m_initialXOffset;
  double m_initialYOffset;
  
  // used to give every KSpreadSheet a unique default name.
  int tableId;

  DCOPObject* m_dcop;
};

#endif
